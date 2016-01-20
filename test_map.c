#include <stdio.h>
#include "map.h"


#define BUF_SIZE 64

typedef struct {
    int count;
    char key[BUF_SIZE];
    MapNode node;
} Counter;

Counter *make_counter(char *key) {
    printf("make_counter(\"%s\"):\n", key);
    Counter *c = malloc(sizeof(Counter));
    c->count = 0;
    strcpy(c->key, key);
    MapNode_init(&c->node);
    printf("\t%p\n", c);
    return c;
}


char *read_until_space(char *buf, char *str) {
    while (*str && *str != ' ') {
        *buf++ = *str++;
    }
    return str;
}

void add_words(Map *m, char *string) {
    printf("add_words(\"%s\"):\n", string);
    char buf[BUF_SIZE];
    char *c = string;
    Counter *counter;
    while (*c) {
        memset(buf, 0, sizeof(buf));
        c = read_until_space(buf, c);
        
        counter = Map_get(m, buf);
        if (!counter) {
            counter = make_counter(buf);
            Map_add(m, counter);
        } else {
            printf("\tcounter for '%s' already exists!\n", buf);
        }
        counter->count++;
        printf("\tadded: %s, count: %i, mapsize: %lu\n", buf, counter->count, m->nelements);
        if (*c) c++;
    }
}

void print_words(Map *m) {
    void **elems = Map_items(m);
    Counter *counter;
    printf("map contents:\n");
    for (int i = 0; i < m->nelements; i++) {
        counter = elems[i];
        printf("\t%s: %i\n", counter->key, counter->count);
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

    Counter *counter = Map_remove(m, "super");
    printf("MAIN: removed 'super'; mapsize: %lu\n", m->nelements);
    free(counter);

    print_words(m);

    add_words(m, "andrew does not suck lol LOL haha he super rules");

    print_words(m);

    free_all(m);
    return 0;
}
