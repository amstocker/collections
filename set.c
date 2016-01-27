#include "set.h"


#define LEFT 0
#define RIGHT 1

#define KEY(S, E) ((void*) ((size_t) E + S->key_offset))
#define NODE(S, E) ((SetNode*) ((size_t) E + S->node_offset))
#define ELEM(S, N) ((void *) ((size_t) N - S->node_offset))

static SetStatus insert (Set *s, SetNode *root, SetNode *node);
static SetNode *rotate_single (SetNode *root, int dir);
static SetNode *rotate_double (SetNode *root, int dir);


void
set_node_init (SetNode *n)
{
  n->color = SET_BLACK;
  n->parent = NULL;
  n->link[LEFT] = NULL;
  n->link[RIGHT] = NULL;
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
set_free (Set *s)
{
  if (!s)
    return SET_ERR;
  free(s);
  return SET_OK;
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
  return insert(s, s->root, node);
}


void*
set_first (Set *s, int dir)
{
  SetNode *node = s->root;
  if (!node)
    return NULL;
  while (node->link[!dir])
    node = node->link[!dir];
  return ELEM(s, node);
}


void*
set_traverse (Set *s, void *elem, int dir)
{
  SetNode *node = NODE(s, elem);
  if (node->link[dir]) {
    node = node->link[dir];
    while (node->link[!dir])
      node = node->link[!dir];
    return ELEM(s, node);
  }
  while (node->parent &&
         node == node->parent->link[dir])
    node = node->parent;
  if (!node->parent)
    return NULL;
  return ELEM(s, node->parent);
}


static SetStatus
insert (Set *s, SetNode *root, SetNode *node)
{
  if (node == root)
    // reject duplicate node
    return SET_ERR;
  int dir = (s->cmp(node->key, root->key, s->key_size) > 0);
  if (!root->link[dir]) {
    root->link[dir] = node;
    node->parent = root;
    return SET_OK;
  }
  else
    return insert(s, root->link[dir], node);
}


static SetNode*
rotate_single (SetNode *root, int dir)
{
  SetNode *tmp = root->link[!dir];
  root->link[!dir] = tmp->link[dir];
  root->link[!dir]->parent = root;
  tmp->link[dir] = root;
  root->parent = tmp;
  root->color = SET_RED;
  tmp->color = SET_BLACK;
  return tmp;
}


static SetNode*
rotate_double (SetNode *root, int dir)
{
  root->link[!dir] = rotate_single(root->link[!dir], !dir);
  return rotate_single(root, dir);
}
