#include "list.h"


#define NODE_FROM_ELEM(L, E) ((E == (void*) L) \
                             ? &L->root \
                             : (ListNode*) ((size_t) E + L->offset))

#define ELEM_FROM_NODE(L, N) ((N == &L->root) \
                             ? L \
                             : (void*) ((size_t) N - L->offset))


static inline void insert_between(List *l, ListNode *a, ListNode *b, void *elem) {
    ListNode *n = NODE_FROM_ELEM(l, elem);
    n->next = b->prev->next;
    n->prev = a;
    a->next = elem;
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
    NODE_FROM_ELEM(l, node->next)->prev = node->prev;
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
    return List_next(l, l);
}

void *List_tail(List *l) {
    return List_prev(l, l);
}

void *List_next(List *l, void *elem) {
    ListNode *n = NODE_FROM_ELEM(l, elem);
    if (n->next == (void*) l) {
        return NULL;
    }
    return n->next;
}

void *List_prev(List *l, void *elem) {
    ListNode *n = NODE_FROM_ELEM(l, elem);
    if (n->prev == &l->root) {
        return NULL;
    }
    return n->prev->prev->next;
}


ListStatus List_append(List *l, void *elem) {
    ListNode *last = l->root.prev;
    insert_between(l, last, &l->root, elem);
    return LIST_OK;
}

ListStatus List_prepend(List *l, void *elem) {
    ListNode *first = NODE_FROM_ELEM(l, l->root.next);
    insert_between(l, &l->root, first, elem);
    return LIST_OK;
}

ListStatus List_appendAfter(List *l, void *target, void *elem) {
    ListNode *a = NODE_FROM_ELEM(l, target);
    ListNode *b = NODE_FROM_ELEM(l, a->next);
    insert_between(l, a, b, elem);
    return LIST_OK;
}

ListStatus List_appendBefore(List *l, void *target, void *elem) {
    ListNode *b = NODE_FROM_ELEM(l, target);
    insert_between(l, b->prev, b, elem);
    return LIST_OK;
}


void *List_remove(List *l, void *elem) {
    ListNode *n = NODE_FROM_ELEM(l, elem);
    if (ListNode_unlink(l, n) == LIST_ERR) {
        return NULL;
    }
    return elem;
}

void *List_popHead(List *l) {
    return List_remove(l, l->root.next);
}

void *List_popTail(List *l) {
    return List_remove(l, l->root.prev->prev->next);
}
