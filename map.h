#pragma once
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "hash.h"


typedef uint32_t (*Map_HashFunc) (void *key, size_t size);
#define Map_default_hash Hash_fnv
#define Map_string_hash Hash_djb2

typedef int (*Map_Comparator) (void *lhs, void *rhs, size_t size);
int Map_default_comparator (void*, void*, size_t);
int Map_strict_comparator (void*, void*, size_t);
int Map_string_comparator (void*, void*, size_t);


typedef enum {
    MAP_OK,
    MAP_ERR
} MapStatus;

typedef struct MapNode {
    void *key;
    uint32_t hash;
    struct MapNode *next;
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


#define MAP_INIT_SIZE 4
#define MAP_HIGH_WATERMARK 0.9

#define Map_new(T, NODE, KEY) Map_newWithOffsets(offsetof(T, NODE), \
                                                 offsetof(T, KEY), \
                                                 sizeof(((T*)0)->KEY))

#define StringMap_new(T, NODE, KEY) StringMap_newWithOffsets( \
                                                 offsetof(T, NODE), \
                                                 offsetof(T, KEY))

#define Map_size(M) (M->nelements)
#define Map_capacity(M) ((size_t) (M->nbuckets * MAP_HIGH_WATERMARK))
#define Map_filled(M) ((float) M->nelements / M->nbuckets)


MapStatus MapNode_init (MapNode *n);
Map *Map_newWithOffsets (size_t node_offset, size_t key_offset, size_t key_size);
Map *StringMap_newWithOffsets (size_t node_offset, size_t key_offset);
MapStatus Map_free (Map *m);
MapStatus Map_add (Map *m, void *elem);
void *Map_get (Map *m, void *key);
void *Map_remove (Map *m, void *key);
void **Map_items (Map *m);
