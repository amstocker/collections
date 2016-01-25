// models tree in an example in this article:
//  https://www.topcoder.com/community/data-science/data-science-tutorials/an-introduction-to-binary-search-and-red-black-trees/
#include <stdio.h>
#include "set.h"
#include "comparator.h"

typedef struct {
  int n;
  SetNode node;
} Val;

void insert (Set *s, int n) {
  Val *v = malloc(sizeof(Val));
  v->n = n;
  set_insert(s, v);
}


int main() {
  Set *s = set_new(Val, node, n);
  s->cmp = comparator_int;

  insert(s, 11);
  insert(s, 14);
  insert(s, 15);
  insert(s, 2);
  insert(s, 1);
  insert(s, 7);
  insert(s, 5);
  insert(s, 8);

  puts("ascending order:");
  SET_FOREACH (Val, v, s) {
    printf("%i ", v->n);
    fflush(stdout);
  }
  putc('\n', stdout);

  puts("descending order:");
  SET_FOREACH_DESC (Val, v, s) {
    printf("%i ", v->n);
    fflush(stdout);
  }
  putc('\n', stdout);

  return 0;
}
