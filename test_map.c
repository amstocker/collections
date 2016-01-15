#include <stdio.h>
#include "map.h"


typedef struct {
    int count;
    char key[64];
    MapNode node;
} Counter;

Counter *make_counter(char *key) {
    Counter *c = malloc(sizeof(Counter));
    c->count = 0;
    strcpy(c->key, key);
    MapNode_init(&c->node);
    printf("make counter: %s, %p, %p\n", key, c, &c->node);
    return c;
}

char *read_until_space(char *buf, char *str) {
    while (*str && *str != ' ') {
        *buf++ = *str++;
    }
    return str;
}


int main() {
    Map *m = StringMap_new(Counter, node, key);
    
    char *string = "andrew rules lol he is super cool!";

    char buf[64];
    char *c = string;
    Counter *counter;
    while (*c) {
        memset(buf, 0, sizeof(buf));
        c = read_until_space(buf, c);
        
        counter = Map_get(m, buf);
        if (!counter) {
            counter = make_counter(buf);
            Map_add(m, counter);
        }
        counter->count++;
        if (*c) c++;
    }

    void **elems = Map_items(m);
    for (int i = 0; i < m->nelements; i++) {
        counter = elems[i];
        printf("%s: %i\n", counter->key, counter->count);
    }

    counter = Map_remove(m, "andrew");
    printf("removed 'andrew': %s, %i\n", counter->key, counter->count);

    return 0;
}
