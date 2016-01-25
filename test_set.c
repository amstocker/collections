#include <stdio.h>
#include "set.h"


#define BUF_SIZE 64
#define ELEM(S, N) ((void *) ((size_t) N - S->node_offset))

typedef struct {
  char name[BUF_SIZE];
  int age;
  SetNode node;
} Person;

Person *person_new(char *name, int age) {
  Person *p = malloc(sizeof(Person));
  strcpy(p->name, name);
  p->age = age;
  return p;
}

Set *s;


void print_set_helper (SetNode *n, int indent) {
  static const char ph = '-';

  for (int i = 0; i < indent; i++) putc(ph, stdout);
  if (!n) {
    printf("((null))\n");
    return;
  }
  Person *p = ELEM(s, n);
  printf("Person(\"%s\", %i)\n", p->name, p->age);
  for (int i = 0; i < indent; i++) putc(ph, stdout);
  printf("%cLEFT:\n", ph);
  print_set_helper(n->left, indent+2);
  for (int i = 0; i < indent; i++) putc(ph, stdout);
  printf("%cRIGHT:\n", ph);
  print_set_helper(n->right, indent+2);
}

void print_set (Set *s) {
  print_set_helper(s->root, 1);
}


int main() {
  s = string_set_new(Person, node, name);

  Person *p;

  puts("making 'Chris' ...");  
  p = person_new("Chris", 21);
  set_insert(s, p);
  print_set(s);

  puts("making 'Dave' ...");  
  p = person_new("Dave", 22);
  set_insert(s, p);
  print_set(s);

  puts("making 'Andrew' ...");  
  p = person_new("Andrew", 25);
  set_insert(s, p);
  print_set(s);
  
  SET_FOREACH(Person, p, s)
    printf("Person: %s (%i)\n", p->name, p->age);

  return 0;
}
