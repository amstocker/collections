#include <stdio.h>
#include <stdint.h>
#include "map.h"


typedef struct {
    int x;
    int y;
} Key;

typedef struct {
    char *val;
    Key key;
    MapNode node;
} Coord;


Coord *make_coord(int x, int y, char *val) {
    Coord *c = malloc(sizeof(Coord));
    c->key.x = x;
    c->key.y = y;
    c->val = val;
    map_node_init(&c->node);
    return c;
}


int main() {
    Map *m = map_new(Coord, node, key);

    Coord *c;

    c = make_coord(10, 42, "hello");
    printf("adding (10, 42)\n");
    map_add(m, c);

    printf("adding (666, 747)\n");
    c = make_coord(666, 747, "world!");
    map_add(m, c);

    Key key;
    
    key.x = 10;
    key.y = 42;
    c = map_get(m, &key);
    printf("get (10, 42): %s\n", c->val);
    
    key.x = 666;
    key.y = 747;
    c = map_get(m, &key);
    printf("get (666, 747): %s\n", c->val);

    // should fail
    key.x = -1;
    c = map_get(m, &key);
    printf("get (-1, 747): %p\n", c);

    return 0;
}
