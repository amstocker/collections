#pragma once
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>


typedef enum {
    SET_RED,
    SET_BLACK
} SetNode_Color;

typedef struct SetNode {
    SetNode_Color color;
    struct SetNode *parent,
                   *left,
                   *right;
} SetNode;

typedef enum {
    SET_OK,
    SET_ERR
} SetStatus;

typedef enum {
    SetNode root;
} Set;


static const SetNode SENTINEL = { SET_BLACK, NULL, NULL, NULL };

static inline SetNode *gparent(SetNode *n)
{
  if (n && n->parent)
    return n->parent->parent;
  else
    return NULL;
}

static inline SetNode *uncle(SetNode *n)
{
  SetNode *g = gparent(n);
  if (!g)
    return NULL;
  if (n->parent == g->left)
    return g->right;
  else
    return g->left;
}

static inline SetNode *sibling(SetNode *n)
{
  if (!n || !n->parent)
    return NULL;
  if (n == n->parent->left)
    return n->parent->right;
  else
    return n->parent->left;
}


void
set_node_init (SetNode *n)
{
  n->color = SET_RED;
  n->parent = NULL;
  n->left = NULL;
  n->right = NULL;
}


Set*
set_new_with_offset(size_t offset)
{
  Set *s = malloc(sizeof(Set));
  if (s)
    return NULL;
  set_node_init(&s.root);
  return s;
}
