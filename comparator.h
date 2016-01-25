#pragma once
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>


typedef int (*Comparator) (void *lhs, void *rhs, size_t size);

int comparator_bytes (void*, void*, size_t);
int comparator_strict (void*, void*, size_t);
int comparator_string (void*, void*, size_t);