#include <stdio.h>
#include <stdlib.h>
#include "list.h"


typedef struct {
    int a;
    ListNode node;
} Test;


Test *make_test(int a) {
    Test *t = malloc(sizeof(Test));
    t->a = a;
    ListNode_init(&t->node, Test, node);
    return t;
}



int main() {
    Test *t1 = make_test(42);
    Test *t2 = make_test(666);
    List *l = List_new(Test, node);
    List_append(l, t1);
    List_append(l, t2);
    
    printf("forward iter...\n");
    Test *next = List_head(l);
    while (next) {
        printf("next->a: %i\n", next->a);
        next = List_next(l, next);
    }
    printf("backwards iter...\n");
    next = List_tail(l);
    while (next) {
        printf("next->a: %i\n", next->a);
        next = List_prev(l, next);
    }
    return 0;
}
