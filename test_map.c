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

void add_words(Map *m, char *string) {
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
        printf("added: %s, count: %i, mapsize: %lu\n", buf, counter->count, m->nelements);
        if (*c) c++;
    }
}

void print_words(Map *m) {
    void **elems = Map_items(m);
    Counter *counter;
    for (int i = 0; i < m->nelements; i++) {
        counter = elems[i];
        printf("%s: %i\n", counter->key, counter->count);
    }
    free(elems);
}

void free_all(Map *m) {
    void **elems = Map_items(m);
    Counter *counter;
    for (int i = 0; i < m->nelements; i++) {
        counter = elems[i];
        free(counter);
    }
    free(elems);
    Map_free(m);
}


int main() {
    Map *m = StringMap_new(Counter, node, key);
    
    add_words(m, "andrew rules lol he is super cool!");
    print_words(m);

    Counter *counter = Map_remove(m, "andrew");
    printf("removed 'andrew'; mapsize: %lu\n", m->nelements);
    free(counter);

    print_words(m);

    free_all(m);
    return 0;
}
