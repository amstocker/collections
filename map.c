#include "map.h"


#define SIZEUP(N) (((N) * 2) - 1)

#define KEY(M, E) ((void*) ((size_t) E + M->key_offset))
#define NODE(M, E) ((MapNode*) ((size_t) E + M->node_offset))
#define ELEM(M, N) ((void *) ((size_t) N - M->node_offset))
#define HASH(M, K) ((size_t) (M->hash(K, M->key_size)))

static MapStatus add_node (Map *m, MapNode *node);
static void *remove_elem (Map *m, void *key);
static MapStatus maybe_rehash (Map *m);


int Map_default_comparator (void *lhs, void *rhs, size_t size)
{
    int r = 0;
    while (size--) {
        r += *(uint8_t*) lhs - *(uint8_t*) rhs;
        lhs++; rhs++;
    }
    return r;
}


int Map_strict_comparator (void *lhs, void *rhs, size_t _)
{
    (void) _;  // unused
    return (int) !(lhs == rhs);
}


int Map_string_comparator (void *lhs, void *rhs, size_t _)
{
    (void) _;  // unused
    return strcmp((char*) lhs, (char*) rhs);
}


MapStatus MapNode_init (MapNode *n)
{
    n->key = NULL;
    n->hash = 0;
    n->next = NULL;
    return MAP_OK;
}


Map *Map_newWithOffsets (size_t node_offset,
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


Map *StringMap_newWithOffsets (size_t node_offset, size_t key_offset)
{
    Map *m = Map_newWithOffsets(node_offset, key_offset, 0);
    m->hash = Map_string_hash;
    m->cmp = Map_string_comparator;
    return m;
}


MapStatus Map_free (Map *m)
{
    if (!m || !m->buckets) {
        return MAP_ERR;
    }
    free(m->buckets);
    free(m);
    return MAP_OK;
}


MapStatus Map_add (Map *m, void *elem)
{
    if (maybe_rehash(m) == MAP_ERR) return MAP_ERR;
    
    MapNode *node = NODE(m, elem);
    node->key = KEY(m, elem);
    node->hash = HASH(m, node->key);

    return add_node(m, node);
}


void *Map_get (Map *m, void *key)
{
    MapNode *bucket = m->buckets[ HASH(m, key) % m->nbuckets ];
    while (bucket) {
        if (m->cmp(key, bucket->key, m->key_size) == 0) {
            return ELEM(m, bucket);
        }
        bucket = bucket->next;
    }
    return NULL;
}


void *Map_remove (Map *m, void *key)
{
    return remove_elem(m, key);
}


void **Map_items (Map *m) {
    void **elems = calloc(m->nelements, sizeof(void*));
    if (!elems) {
        return NULL;
    }
    void **elem = elems;
    MapNode *n;
    for (size_t i = 0; i < m->nbuckets; i++) {
        n = m->buckets[i];
        while (n) {
            *elem++ = ELEM(m, n);
            n = n->next;
        }
    }
    return elems;
}


static MapStatus add_node (Map *m, MapNode *node)
{
    size_t index = node->hash % m->nbuckets;
    MapNode *bucket = m->buckets[index];
    MapNode *prev;
    if (!bucket) {
        m->buckets[index] = node;
    } else {
        while (bucket) {
            if (!m->cmp(node->key, bucket->key, m->key_size)) {
                return MAP_ERR;
            }
            prev = bucket;
            bucket = bucket->next;
        }
        prev->next = node;
    }
    m->nelements++;
    return MAP_OK;
}


static void *remove_elem (Map *m, void *key)
{
    uint32_t index = HASH(m, key) % m->nbuckets;
    MapNode *bucket = m->buckets[index];
    MapNode *prev = NULL;
    while (bucket) {
        if (m->cmp(key, bucket->key, m->key_size) == 0) {
            if (!prev) {
                m->buckets[index] = bucket->next;
            } else {
                prev->next = bucket->next;
            }
            bucket->next = NULL;       
            m->nelements--;
            return ELEM(m, bucket);
        }
        prev = bucket;
        bucket = bucket->next;
    }
    return NULL;
}


static MapStatus maybe_rehash (Map *m)
{
    if ((float) m->nelements / m->nbuckets < MAP_HIGH_WATERMARK) {
        return MAP_OK;
    }
    size_t old_size = m->nbuckets;
    size_t new_size = SIZEUP(old_size);
    MapNode **new_buckets = calloc(new_size, sizeof(MapNode*));
    if (!new_buckets) {
        return MAP_ERR;
    }
    MapNode **old_buckets = m->buckets;
    m->buckets = new_buckets;
    m->nbuckets = new_size;
    m->nelements = 0;

    MapNode *node, *next;
    for (size_t i = 0; i < old_size; i++) {
        node = old_buckets[i];
        while (node) {
            add_node(m, node);
            next = node->next;
            node->next = NULL;
            node = next;
        }
    }
    free(old_buckets);
    return MAP_OK;
}
