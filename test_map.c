#include <stdio.h>
#include "map.h"


#define BUF_SIZE 64

void print_diag(Map *m) {
    size_t tot_len = 0;
    MapNode *bucket;
    for (size_t i = 0; i < m->nbuckets; i++) {
        bucket = m->buckets[i];
        if (!bucket) {
            continue;
        }
        while ((bucket = bucket->next)) tot_len++;
    }
    printf("DIAG:\n\tsize: %lu, buckets: %lu, filled: %f, avg_chain: %f\n",
            map_size(m),
            m->nbuckets,
            map_filled(m),
            (float) tot_len / m->nbuckets);
}


typedef struct {
    int count;
    char key[BUF_SIZE];
    MapNode node;
} Counter;

Counter *make_counter(char *key) {
    Counter *c = malloc(sizeof(Counter));
    c->count = 0;
    strcpy(c->key, key);
    map_node_init(&c->node);
    return c;
}


char *read_until_space(char *buf, char *str) {
    while (*str && *str != ' ') {
        *buf++ = *str++;
    }
    return str;
}

void add_words(Map *m, char *string) {
    char buf[BUF_SIZE];
    char *c = string;
    Counter *counter;
    while (*c) {
        memset(buf, 0, sizeof(buf));
        c = read_until_space(buf, c);
        
        counter = map_get(m, buf);
        if (!counter) {
            counter = make_counter(buf);
            map_add(m, counter);
        }
        counter->count++;
        print_diag(m);
        if (*c) c++;
    }
}

void print_words(Map *m) {
    void **elems = map_items(m);
    Counter *counter;
    for (int i = 0; i < m->nelements; i++) {
        counter = elems[i];
        printf("print_words: %s: %i\n", counter->key, counter->count);
    }
    free(elems);
}

void free_all(Map *m) {
    void **elems = map_items(m);
    Counter *counter;
    for (int i = 0; i < m->nelements; i++) {
        counter = elems[i];
        free(counter);
    }
    free(elems);
    map_free(m);
}


int main() {
    Map *m = string_map_new(Counter, node, key);
    
    add_words(m, "andrew rules lol he is super cool!");
    Counter *counter = map_remove(m, "super");
    free(counter);
    printf("main: removed 'super', size: %lu\n", map_size(m));
    add_words(m, "andrew does not suck lol LOL haha he super rules");

    print_words(m);
    
    free_all(m);
    return 0;
}
