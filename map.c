#include "map.h"
#include "hash.h"


#define KEY(M, E) ((void*) ((size_t) E + M->key_offset))
#define NODE(M, E) ((MapNode*) ((size_t) E + M->node_offset))
#define ELEM(M, N) ((void *) ((size_t) N - M->node_offset))
#define HASH(M, K) ((size_t) (M->hash(K, M->key_size)))
#define REHASH(M, E) ((size_t) (NODE(M, E)->hash \
                                ? NODE(M, E)->hash % M->node_offset \
                                : HASH(M, KEY(M, E)) % M->node_offset))


int Map_default_comparator(void *lhs, void *rhs, size_t size)
{
    int r = 0;
    while (size--) {
        r += *(uint8_t*) lhs - *(uint8_t*) rhs;
        lhs++; rhs++;
    }
    return r;
}


int Map_string_comparator(void *lhs, void *rhs, size_t _)
{
    return strcmp((char*) lhs, (char*) rhs);
}


MapStatus MapNode_init(MapNode *n)
{
    n->key = NULL;
    n->hash = 0;
    n->chain_next = NULL;
    n->eq_next = NULL;
    return MAP_OK;
}


Map *Map_newWithOffsets(size_t node_offset,
                        size_t key_offset,
                        size_t key_size)
{
    Map *m = malloc(sizeof(Map));
    if (!m) {
        return NULL;
    }
    m->hash = Map_default_hash;
    m->cmp = Map_default_comparator;
    m->buckets = calloc(MAP_INIT_SIZE, sizeof(MapNode*));
    m->node_offset = node_offset;
    m->key_offset = key_offset;
    m->key_size = key_size;
    m->nbuckets = MAP_INIT_SIZE;
    m->nelements = 0;
    return m;
}


Map *StringMap_newWithOffsets(size_t node_offset, size_t key_offset)
{
    Map *m = Map_newWithOffsets(node_offset, key_offset, 0);
    m->hash = Map_string_hash;
    m->cmp = Map_string_comparator;
    return m;
}


MapStatus Map_free(Map *m)
{
    if (!m || !m->buckets) {
        return MAP_ERR;
    }
    free(m->buckets);
    free(m);
    return MAP_OK;
}


MapStatus Map_add(Map *m, void *elem)
{
    //if (maybe_rehash(m) == MAP_ERR) return MAP_ERR;
    void *key = KEY(m, elem);
    MapNode *node = NODE(m, elem);
    uint32_t hash  = HASH(m, key);
    node->key = key;
    node->hash = hash;

    size_t index = hash % m->nbuckets;
    MapNode *bucket = m->buckets[index];
    if (!bucket) {
        m->buckets[index] = node;
        goto ok;
    }
    MapNode *next = bucket->chain_next;
    if (!next && !m->cmp(key, bucket->key, m->key_size)) {
        m->buckets[index] = node;
        node->eq_next = bucket;
        goto eq;
    }
    while (next) {
        if (!m->cmp(key, bucket->key, m->key_size)) {
            bucket->chain_next = node;
            node->chain_next = next->chain_next;
            node->eq_next = next;
            next->chain_next = NULL;
            goto eq;
        }
        bucket = next;
        next = bucket->chain_next;
    }
    bucket->chain_next = node;

ok:
    m->nelements++;
eq:
    return MAP_OK;
}


void *Map_get(Map *m, void *key)
{
    MapNode *bucket = m->buckets[ HASH(m, key) % m->nbuckets ];
    while (bucket) {
        if (m->cmp(key, bucket->key, m->key_size) == 0) {
            return ELEM(m, bucket);
        }
        bucket = bucket->chain_next;
    }
    return NULL;
}


MapStatus Map_setKey(Map *m, void *elem, void *new_key)
{
    return MAP_OK;
}


void **Map_items(Map *m) {
    void **elems = calloc(m->nelements, sizeof(void*));
    if (!elems) {
        return NULL;
    }
    void **elem = elems;
    MapNode *n;
    for (int i = 0; i < m->nbuckets; i++) {
        n = m->buckets[i];
        while (n) {
            *elem++ = ELEM(m, n);
            n = n->chain_next;
        }
    }
    return elems;
}
