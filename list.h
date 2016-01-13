#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>


typedef enum {
    LIST_OK,
    LIST_ERR
} ListStatus;

typedef struct ListNode {
    struct ListNode *prev;
    void *next;
} ListNode;

typedef struct {
    ListNode root;
    size_t size;
    size_t offset;
} List;


#define ListNode_init(P, T, M) ListNode_initWithOffset(P, offsetof(T, M))
#define List_new(T, M) List_newWithOffset(offsetof(T, M))

#define List_forEach(T, L) for(T = List_head(L); \
                               elem != NULL; \
                               elem = List_next(L, elem))

#define List_forEachReverse(T, L) for (T = List_tail(L); \
                                       elem != NULL; \
                                       elem = List_prev(L, elem))


void ListNode_initWithOffset(ListNode *n, size_t offset);
ListStatus ListNode_unlink(List *l, ListNode *node);
List *List_newWithOffset(size_t offset);
void *List_head(List *l);
void *List_tail(List *l);
void *List_next(List *l, void *elem);
void *List_prev(List *l, void *elem);
ListStatus List_append(List *l, void *elem);
ListStatus List_prepend(List *l, void *elem);
ListStatus List_appendAfter(List *l, void *target, void *elem);
ListStatus List_appendBefore(List *l, void *target, void *elem);
void *List_remove(List *l, void *elem);
void *List_popHead(List *l);
void *List_popTail(List *l);
