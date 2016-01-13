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
    printf("appending 42... length: %lu\n", l->size);

    List_append(l, t2);
    printf("appending 666... length: %lu\n", l->size);
    
    printf("forward iter...\n");
    List_iter(Test *elem, l) {
        printf("elem->a: %i\n", elem->a);
    }
    printf("backwards iter...\n");
    List_iterBackwards(Test *elem, l) {
        printf("elem->a: %i\n", elem->a);
    }

    printf("popping heads...\n");
    Test *next = List_popHead(l);
    while (next) {
        printf("next->a: %i, length: %lu\n", next->a, l->size);
        next = List_popHead(l);
    }
    return 0;
}
