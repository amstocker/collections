#include <stdlib.h>
#include "list.h"


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

static inline void insert_between(List *l, ListNode *a, ListNode *b, void *elem) {
    ListNode *n = get_node_from_elem(l, elem);
    a->next = elem;
    n->prev = a;
    n->next = b;
    b->prev = n;
    l->size++;
}


void ListNode_initWithOffset(ListNode *n, size_t offset) {
    n->prev = n;
    n->next = (void*) ((size_t) n - offset);
}

ListStatus ListNode_unlink(List *l, ListNode *node) {
    if (!node || node == &l->root) {
        return LIST_ERR;
    }
    node->prev->next = node->next;
    get_node_from_elem(l, node->next)->prev = node->prev;
    ListNode_initWithOffset(node, l->offset);
    l->size--;
    return LIST_OK;
}

List *List_newWithOffset(size_t offset) {
    List *l = malloc(sizeof(List));
    if (!l) {
        return NULL;
    }
    l->size = 0;
    l->offset = offset;
    ListNode_initWithOffset(&l->root, offsetof(List, root));
    return l;
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

ListStatus List_append(List *l, void *elem) {
    ListNode *last = l->root.prev;
    insert_between(l, last, &l->root, elem);
    return LIST_OK;
}

ListStatus List_prepend(List *l, void *elem) {
    ListNode *first = get_node_from_elem(l, l->root.next);
    insert_between(l, &l->root, first, elem);
    return LIST_OK;
}

ListStatus List_appendAfter(List *l, void *target, void *elem) {
    ListNode *a = get_node_from_elem(l, target);
    ListNode *b = get_node_from_elem(l, a->next);
    insert_between(l, a, b, elem);
    return LIST_OK;
}

ListStatus List_appendBefore(List *l, void *target, void *elem) {
    ListNode *b = get_node_from_elem(l, target);
    insert_between(l, b->prev, b, elem);
    return LIST_OK;
}


void *List_remove(List *l, void *elem) {
    ListNode *n = get_node_from_elem(l, elem);
    if (ListNode_unlink(l, n) == LIST_ERR) {
        return NULL;
    }
    return elem;
}

void *List_popHead(List *l) {
    return List_remove(l, l->root.next);
}

void *List_popTail(List *l) {
    return List_remove(l, get_elem_from_node(l, l->root.next));
}
