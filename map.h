#pragma once
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>

#define INIT_SIZE 16


typedef uint32_t (*Map_HashFunc) (void *key, size_t size);
typedef int (*Map_Comparator) (void *lhs, void *rhs);

typedef enum {
    MAP_OK,
    MAP_ERR
} MapStatus;

typedef struct MapEntry {
    void *key;
    size_t keysize;
    uint32_t hash;
    void *val;
    struct MapEntry *next;
} MapEntry;

typedef struct {
    MapHashFunc hash;
    MapComparator cmp;
    MapEntry **buckets;
    size_t nbuckets;
    size_t size;
} Map;


uint32_t hash_fnv(void *elem, size_t size) {
    uint32_t r = 2166136261;
    uint8_t *p = elem;
    while (size--) {
        r ^= *p++;
        r *= 16777619;
    }
    return r;
}

uint32_t hash_djb2(void *elem, size_t size) {
    uint32_t r = 5381;
    uint8_t *p = elem;
    while (size--) {
        r = ((r << 5) + r) + *p++;
    }
    return r;
}
