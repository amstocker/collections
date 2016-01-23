#pragma once
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "hash.h"


typedef uint32_t (*Map_HashFunc) (void *key, size_t size);
#define map_default_hash hash_fnv
#define map_string_hash hash_djb2

typedef int (*Map_Comparator) (void *lhs, void *rhs, size_t size);
int map_default_comparator (void*, void*, size_t);
int map_strict_comparator (void*, void*, size_t);
int map_string_comparator (void*, void*, size_t);


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
  size_t cur;
  void *elem;
} MapIter;

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

#define map_new(T, NODE, KEY) map_new_with_offsets(offsetof(T, NODE), \
                                                 offsetof(T, KEY), \
                                                 sizeof(((T*)0)->KEY))

#define string_map_new(T, NODE, KEY) string_map_new_with_offsets( \
                                                 offsetof(T, NODE), \
                                                 offsetof(T, KEY))

#define map_size(M) (M->nelements)
#define map_capacity(M) ((size_t) (M->nbuckets * MAP_HIGH_WATERMARK))
#define map_filled(M) ((float) M->nelements / M->nbuckets)

#define MAP_FOREACH(T, V, M) for (T *V = map_head(M); \
                                  V != NULL; \
                                  V = map_next(M, V))


MapStatus map_node_init (MapNode *n);
Map *map_new_with_offsets (size_t node_offset, size_t key_offset, size_t key_size);
Map *string_map_new_with_offsets (size_t node_offset, size_t key_offset);
MapStatus map_free (Map *m);
MapStatus map_add (Map *m, void *elem);
void *map_get (Map *m, void *key);
void *map_remove (Map *m, void *key);
void **map_items (Map *m);
void *map_head (Map *m);
void *map_next (Map *m, void *elem);
