//#pragma once
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "hash.h"


int Map_default_comparator(void *lhs, void *rhs) {
    return lhs < rhs ? -1 : lhs > rhs ? 1 : 0;
}

int Map_string_comparator(void *lhs, void *rhs) {
    return strcmp((char*) lhs, (char*) rhs);
}


typedef uint32_t (*Map_HashFunc) (void *key, size_t size);
typedef int (*Map_Comparator) (void *lhs, void *rhs);

const Map_HashFunc Map_hash_default = Hash_fnv;
const Map_HashFunc Map_hash_string = Hash_djb2;


typedef enum {
    MAP_OK,
    MAP_ERR
} MapStatus;

typedef struct MapNode {
    void *key;
    uint32_t hash;
    struct MapNode *chain_next;
    struct MapNode *eq_next;
} MapNode;

typedef struct {
    Map_HashFunc hash;
    Map_Comparator cmp;
    MapNode **buckets;
    size_t node_offset;
    size_t key_offset;
    size_t key_size;
    size_t nbuckets;
    size_t nelements;
} Map;


#define MAP_INIT_SIZE 11

#define Map_new(T, NODE, KEY) Map_newWithOffsets(offsetof(T, NODE), \
                                                 offsetof(T, KEY), \
                                                 sizeof(((T*)0)->KEY))

#define StringMap_new(T, NODE, KEY) StringMap_newWithOffsets( \
                                                 offsetof(T, NODE), \
                                                 offsetof(T, KEY))

MapStatus MapNode_init(MapNode *n) {
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
    m->hash = Map_hash_default;
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
    m->hash = Map_hash_string;
    m->cmp = Map_string_comparator;
    return m;
}

MapStatus Map_free(Map *m) {
    if (!m || !m->buckets) {
        return MAP_ERR;
    }
    free(m->buckets);
    free(m);
    return MAP_OK;
}


#define KEY(M, E) ((void*) ((size_t) E + M->key_offset))
#define NODE(M, E) ((MapNode*) ((size_t) E + M->node_offset))
#define ELEM(M, N) ((void *) ((size_t) N - M->node_offset))
#define HASH(M, K) ((size_t) (M->hash(K, M->key_size)))
#define REHASH(M, E) ((size_t) (NODE(M, E)->hash \
                                ? NODE(M, E)->hash % M->node_offset \
                                : HASH(M, KEY(M, E)) % M->node_offset))


MapStatus Map_add(Map *m, void *elem) {
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
    if (!next && !m->cmp(key, bucket->key)) {
        m->buckets[index] = node;
        node->eq_next = bucket;
        goto ok;
    }
    while (next) {
        if (!m->cmp(key, bucket->key)) {
            bucket->chain_next = node;
            node->chain_next = next->chain_next;
            node->eq_next = next;
            next->chain_next = NULL;
            goto ok;
        }
        bucket = next;
        next = bucket->chain_next;
    }
    bucket->chain_next = node;
ok:
    m->nelements++;
    return MAP_OK;
}

void *Map_get(Map *m, void *key) {
    MapNode *bucket = m->buckets[ HASH(m, key) % m->nbuckets ];
    while (bucket && m->cmp(key, bucket->key) != 0) {
        bucket = bucket->chain_next;
    }
    return ELEM(m, bucket);
}

MapStatus Map_setKey(Map *m, void *elem, void *new_key) {
    return MAP_OK;
}
