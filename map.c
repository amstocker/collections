#include "map.h"


#define KEY(M, E) ((void*) ((size_t) E + M->key_offset))
#define NODE(M, E) ((MapNode*) ((size_t) E + M->node_offset))
#define ELEM(M, N) ((void *) ((size_t) N - M->node_offset))
#define HASH(M, K) ((size_t) (M->hash(K, M->key_size)))

static size_t chain_len(MapNode *bucket);
static size_t eq_len(MapNode *bucket);
static MapStatus add_node(Map *m, MapNode *node);
static void *remove_elem(Map *m, void *key, Map_Comparator cmp, int all);
static MapStatus maybe_rehash(Map *m);


int Map_default_comparator(void *lhs, void *rhs, size_t size)
{
    int r = 0;
    while (size--) {
        r += *(uint8_t*) lhs - *(uint8_t*) rhs;
        lhs++; rhs++;
    }
    return r;
}


int Map_strict_comparator(void *lhs, void *rhs, size_t _)
{
    return (int) ((size_t) lhs - (size_t) rhs);
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


/**
 * Adds an element to the map.  It either become the head of a bucket or
 * gets added in a linked list.  If the key is equal to the key of an existing
 * element it will be added to a linked list accessible by:
 *
 *  MapNode *next = elem->node->eq_next;
 *
 **/
MapStatus Map_add(Map *m, void *elem)
{
    //if (maybe_rehash(m) == MAP_ERR) return MAP_ERR;
    void *key = KEY(m, elem);
    MapNode *node = NODE(m, elem);
    uint32_t hash  = HASH(m, key);
    node->key = key;
    node->hash = hash;

    if (add_node(m, node) == MAP_ERR) return MAP_ERR;

    m->nelements++;
    return MAP_OK;
}

static MapStatus add_node(Map *m, MapNode *node)
{
    size_t index = node->hash % m->nbuckets;
    MapNode *bucket = m->buckets[index];
    MapNode *prev = NULL;
    if (!bucket) {
        m->buckets[index] = node;
    } else {
        while (bucket) {
            if (!m->cmp(node->key, bucket->key, m->key_size)) {
                if (!prev) {
                    m->buckets[index] = node;
                } else {
                    prev->chain_next = node;
                }
                node->chain_next = bucket->chain_next;
                bucket->chain_next = NULL;
                node->eq_next = bucket;
                break;
            }
            prev = bucket;
            bucket = bucket->chain_next;
        }
        prev->chain_next = node;
    }
    return MAP_OK;
}


/**
 * Gets the head of a linked list of elements with equal keys.  The head of the
 * list will be the most recently inserted.
 */
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


/**
 * Removes a single element corresponding to a given key, this will be
 * the last element added with that key.
 */
void *Map_remove(Map *m, void *key)
{
    return remove_elem(m, key, m->cmp, 0);
}


/**
 * Removes the entire linked list corresponding to one key.
 */
void *Map_remove_all(Map *m, void *key)
{
    return remove_elem(m, key, m->cmp, 1);
}


/**
 * Updates an element.
 */
MapStatus Map_update(Map *m, void *elem)
{
    elem = remove_elem(m, KEY(m, elem), Map_strict_comparator, 0);
    if (elem) {
        return Map_add(m, elem);
    }
    return MAP_ERR;
}


/**
 * Returns an array of all items in the Map.
 */
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


static size_t chain_len(MapNode *bucket) {
    size_t len = 0;
    while (bucket) {
        len++;
        bucket = bucket->chain_next;
    }
    return len;
}


static size_t eq_len(MapNode *bucket) {
    size_t len = 0;
    while (bucket) {
        len++;
        bucket = bucket->eq_next;
    }
    return len;
}


static void *remove_elem(Map *m, void *key, Map_Comparator cmp, int all)
{
    uint32_t index = HASH(m, key) % m->nbuckets;
    MapNode *bucket = m->buckets[index];
    MapNode *prev = NULL;
    while (bucket) {
        if (cmp(key, bucket->key, m->key_size) == 0) {
            if (!prev) {
                if (all) {
                    m->buckets[index] = bucket->chain_next;
                    bucket->chain_next = NULL;
                } else {
                    if (bucket->eq_next) {
                        m->buckets[index] = bucket->eq_next;
                        bucket->eq_next->chain_next = bucket->chain_next;
                        bucket->eq_next = NULL;
                        bucket->chain_next = NULL;
                    } else {
                        m->buckets[index] = bucket->chain_next;
                    }
                }
            } else {
                if (all) {
                    prev->chain_next = bucket->chain_next;
                } else {
                    if (bucket->eq_next) {
                        prev->chain_next = bucket->eq_next;
                        bucket->eq_next->chain_next = bucket->chain_next;
                        bucket->eq_next = NULL;
                    } else {
                        prev->chain_next = bucket->chain_next;
                    }
                }
                bucket->chain_next = NULL;       
            }
            if (all) {
                m->nelements -= eq_len(bucket);
            } else {
                m->nelements--;
            }
            return ELEM(m, bucket);
        }
        prev = bucket;
        bucket = bucket->chain_next;
    }
    return NULL;
}


static MapStatus maybe_rehash(Map *m)
{
    return MAP_OK;    
}
