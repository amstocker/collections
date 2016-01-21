#include <stdio.h>
#include <stdlib.h>
#include "list.h"


typedef struct {
  int a;
  ListNode node;
} Test;

Test*
make_test (int a)
{
  Test *t = malloc(sizeof(Test));
  t->a = a;
  list_node_init (&t->node, Test, node);
  return t;
}

List*
construct_list ()
{
  printf("\nconstructing new list...\n");
  
  Test *t1 = make_test (42);
  Test *t2 = make_test (666);
  Test *t3 = make_test (747);
  Test *t4 = make_test (69);
  Test *t5 = make_test (111);
  
  List *l = list_new (Test, node);
  
  list_append (l, t1);
  printf("appending %i... length: %lu\n", t1->a, l->size);
  
  list_append (l, t2);
  printf("appending %i... length: %lu\n", t2->a, l->size);
  
  list_append_after (l, t1, t3);
  printf("appending after %i: %i... length: %lu\n", t1->a, t3->a, l->size);
  
  list_append_before (l, t1, t4);
  printf("appending before %i: %i... length: %lu\n", t1->a, t4->a, l->size);
  
  list_prepend (l, t5);
  printf("prepending %i... length: %lu\n", t5->a, l->size);
  
  return l;
}


int
main()
{
  List *l = construct_list();
 
  printf("forward iter...\n");
  LIST_FOREACH (Test, elem, l) {
      printf("elem->a: %i\n", elem->a);
  }
  
  printf("reverse iter...\n");
  LIST_FOREACH_REV (Test, elem, l) {
      printf("elem->a: %i\n", elem->a);
  }

  printf("popping heads...\n");
  Test *next = list_pop_head(l);
  while (next) {
      printf("next->a: %i, length: %lu\n", next->a, l->size);
      free(next);
      next = list_pop_head(l);
  }
  free(l);


  l = construct_list();

  printf("popping tails...\n");
  next = list_pop_tail(l);
  while (next) {
      printf("next->a: %i, length %lu\n", next->a, l->size);
      free(next);
      next = list_pop_tail(l);
  }
  free(l);


  return 0;
}
