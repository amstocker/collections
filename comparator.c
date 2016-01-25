#include "comparator.h"


#define NUMERIC_COMPARATOR(T) \
  int \
  comparator_##T (void *lhs, void *rhs, size_t _) \
  { \
    (void) _; \
    return   *(T*) lhs < *(T*) rhs ? -1 \
           : *(T*) lhs > *(T*) rhs ?  1 \
           : 0; \
  }

NUMERIC_COMPARATOR(int);
NUMERIC_COMPARATOR(float);
NUMERIC_COMPARATOR(double);


int
comparator_bytes (void *lhs, void *rhs, size_t size)
{
  int r = 0;
  while (size--) {
    r += *(uint8_t*) lhs - *(uint8_t*) rhs;
    lhs++; rhs++;
  }
  return r;
}


int
comparator_pointer (void *lhs, void *rhs, size_t _)
{
  (void) _;  // unused
  return lhs < rhs ? -1 : lhs > rhs ? 1 : 0;
}


int
comparator_string (void *lhs, void *rhs, size_t _)
{
  (void) _;  // unused
  return strcmp((char*) lhs, (char*) rhs);
}
