#include "hash.h"


uint32_t Hash_fnv(void *elem, size_t size) {
    uint32_t r = 2166136261;
    uint8_t *p = elem;
    while (size--) {
        r ^= *p++;
        r *= 16777619;
    }
    return r;
}


uint32_t Hash_djb2(void *elem, size_t _) {
    uint32_t r = 5381;
    uint8_t *p = elem;
    while (p) {
        r = ((r << 5) + r) + *p++;
    }
    return r;
}
