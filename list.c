#include <stdlib.h>
#include "list.h"


ListStatus ListNode_initWithOffset(ListNode *n, size_t offset) {
    n->prev = n;
    n->next = (void*) ((size_t) n - offset);
    return LIST_OK;
}

List *List_newWithOffset(size_t offset) {
    List *l = malloc(sizeof(List));
    if (!l) {
        return NULL;
    }
    l->offset = offset;
    ListNode_initWithOffset(&l->root, offsetof(List, root));
    return l;
}


static inline ListNode *get_node_from_elem(List *l, void *elem) {
    if ((void*) l == elem) {
        return &l->root;
    }
    return (ListNode*) ((size_t) elem + l->offset);
}

static inline void *get_elem_from_node(List *l, ListNode *node) {
    if (node == &l->root) {
        return l;
    }
    return (void*) ((size_t) node - l->offset);
}

ListStatus insert_between(List *l, ListNode *a, ListNode *b, void *elem) {
    ListNode *n = get_node_from_elem(l, elem);
    a->next = elem;
    n->prev = a;
    n->next = b;
    b->prev = n;
    return LIST_OK;
}


void *List_head(List *l) {
    return List_next(l, &l->root);
}

void *List_tail(List *l) {
    return List_prev(l, &l->root);
}

void *List_next(List *l, void *elem) {
    ListNode *n = get_node_from_elem(l, elem);
    if (n->next == l) {
        return NULL;
    }
    return n->next;
}

void *List_prev(List *l, void *elem) {
    ListNode *n = get_node_from_elem(l, elem);
    if (n->prev == &l->root) {
        return NULL;
    }
    return get_elem_from_node(l, n->prev);
}

void *List_remove(List *l, void *elem) {
    ListNode *n = get_node_from_elem(l, elem);
    n->prev->next = n->next;
    get_node_from_elem(l, n->next)->prev = n->prev;
    ListNode_initWithOffset(n, l->offset);
    return elem;
}

ListStatus List_append(List *l, void *elem) {
    ListNode *last = l->root.prev;
    return insert_between(l, last, &l->root, elem);
}

ListStatus List_prepend(List *l, void *elem) {
    ListNode *first = get_node_from_elem(l, l->root.next);
    return insert_between(l, &l->root, first, elem);
}

ListStatus List_appendAfter(List *l, void *target, void *elem) {
    ListNode *a = get_node_from_elem(l, target);
    ListNode *b = get_node_from_elem(l, a->next);
    return insert_between(l, a, b, elem);
}

ListStatus List_appendBefore(List *l, void *target, void *elem) {
    ListNode *b = get_node_from_elem(l, target);
    return insert_between(l, b->prev, b, elem);
}
