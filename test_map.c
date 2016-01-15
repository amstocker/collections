#include <stdio.h>
#include "map.h"

typedef struct {
    int count;
    char key;
    MapNode node;
} Counter;

Counter *make_counter(char key) {
    Counter *c = malloc(sizeof(Counter));
    c->count = 0;
    c->key = key;
    MapNode_init(&c->node);
    printf("init counter(%c): %p\n", key, c);
    return c;
}

int comparator(void *lhs, void *rhs) {
    char a = *(char*)lhs;
    char b = *(char*)rhs;
    return a < b ? -1 : a > b ? 1 : 0;
}


int main() {
    Counter *counters[128] = {NULL};
    Map *m = Map_new(Counter, node, key);
    m->cmp = comparator;

    for (int i = 0; i < 128; i++) {
        counters[i] = make_counter((char)i);
        Map_add(m, counters[i]);
    }
    
    char *string = "andrew rules! lol\n";

    char *c = string;
    Counter *counter;
    while (*c) {
        counter = Map_get(m, c);
        counter->count++;
        printf("next char: %c, counter: %p, count: %i\n", *c, counter, counter->count);
        c++;
    }

    for (int i = 0; i < 128; i++) {
        counter = Map_get(m, (char*)&i);
        printf("%c: %i\n", (char)i, counter->count);
    }

    return 0;
}
