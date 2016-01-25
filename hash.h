#pragma once
#include <stddef.h>
#include <stdint.h>


typedef uint32_t (*HashFunc) (void *key, size_t size);

uint32_t hash_fnv (void *, size_t);
uint32_t hash_djb2 (void *, size_t);
