#include "comparator.h"

  
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
comparator_strict (void *lhs, void *rhs, size_t _)
{
  (void) _;  // unused
  return (int) !(lhs == rhs);
}


int
comparator_string (void *lhs, void *rhs, size_t _)
{
  (void) _;  // unused
  return strcmp((char*) lhs, (char*) rhs);
}
