#include "set.h"


#define KEY(S, E) ((void*) ((size_t) E + S->key_offset))
#define NODE(S, E) ((SetNode*) ((size_t) E + S->node_offset))
#define ELEM(S, N) ((void *) ((size_t) N - S->node_offset))

#define ATTACH(P, N, dir) { P->dir = N; N->parent = P; }

static void insert (Set *s, SetNode *root, SetNode *node);


void
set_node_init (SetNode *n)
{
  n->color = SET_BLACK;
  n->parent = NULL;
  n->left = NULL;
  n->right = NULL;
}


Set*
set_new_with_offsets (size_t node_offset, size_t key_offset, size_t key_size)
{
  Set *s = malloc(sizeof(Set));
  if (!s)
    return NULL;
  s->cmp = set_default_comparator;
  s->root = NULL;
  s->node_offset = node_offset;
  s->key_offset = key_offset;
  s->key_size = key_size;
  s->size = 0;
  return s;
}


Set*
string_set_new_with_offsets (size_t node_offset, size_t key_offset)
{
  Set *s = set_new_with_offsets(node_offset, key_offset, 0);
  s->cmp = set_string_comparator;
  return s;
}


SetStatus
set_insert (Set *s, void *elem)
{
  if (!elem)
    return SET_ERR;
  SetNode *node = NODE(s, elem);
  set_node_init(node);
  node->key = KEY(s, elem);
  if (!s->root) {
    s->root = node;
    return SET_OK;
  }
  node->color = SET_RED;
  insert(s, s->root, node);
  return SET_OK;
}


void*
set_head (Set *s)
{
  SetNode *node = s->root;
  if (!node)
    return NULL;
  while (node->left)
    node = node->left;
  return ELEM(s, node);
}


void*
set_next (Set *s, void *elem)
{
  SetNode *node = NODE(s, elem);
  if (node->right) {
    node = node->right;
    while (node->left)
      node = node->left;
    return ELEM(s, node);
  }
  if (node != node->parent->left) {
    node = node->parent;
    while (node->parent && node != node->parent->left)
      node = node->parent;
    if (!node->parent)
      return NULL;
  }
  return ELEM(s, node->parent);
}


static void insert (Set *s, SetNode *root, SetNode *node)
{
  if (s->cmp(node->key, root->key, s->key_size) < 0) {
    if (!root->left)
      ATTACH(root, node, left)
    else
      return insert(s, root->left, node);
  } else {
    if (!root->right)
      ATTACH(root, node, right)
    else
      return insert(s, root->right, node);
  }
}
