#pragma once
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include "comparator.h"


#define set_default_comparator comparator_bytes
#define set_string_comparator comparator_string


typedef enum {
  SET_RED,
  SET_BLACK
} SetNodeColor;

typedef struct SetNode {
  void *key;
  SetNodeColor color;
  struct SetNode *parent;
  struct SetNode *link[2];
} SetNode;

typedef enum {
  SET_OK,
  SET_ERR
} SetStatus;

typedef struct {
  Comparator cmp;
  SetNode *root;
  size_t size;
  size_t node_offset;
  size_t key_offset;
  size_t key_size;
} Set;


#define set_new(T, NODE, KEY) set_new_with_offsets(offsetof(T, NODE), \
                                                 offsetof(T, KEY), \
                                                 sizeof(((T*)0)->KEY))

#define string_set_new(T, NODE, KEY) string_set_new_with_offsets( \
                                                 offsetof(T, NODE), \
                                                 offsetof(T, KEY))

#define SET_RIGHT 1  // ascending order
#define SET_LEFT 0   // descending order

#define set_head(S) set_first(S, SET_RIGHT)
#define set_tail(S) set_first(S, SET_LEFT)
#define set_next(S, E) set_traverse(S, E, SET_RIGHT)
#define set_prev(S, E) set_traverse(S, E, SET_LEFT)

#define SET_FOREACH(T, V, S) for (T *V = set_head(S); \
                                  V != NULL; \
                                  V = set_next(S, V))

#define SET_FOREACH_DESC(T, V, S) for (T *V = set_tail(S); \
                                       V != NULL; \
                                       V = set_prev(S, V))

void set_node_init (SetNode *n);
Set *set_new_with_offsets (size_t node_offset, size_t key_offset, size_t key_size);
Set *string_set_new_with_offsets (size_t node_offset, size_t key_offset);
SetStatus set_free (Set *s);
SetStatus set_insert (Set *s, void *elem);
void *set_first (Set *s, int dir);
void *set_traverse (Set *s, void *elem, int dir);
