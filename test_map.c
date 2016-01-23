// Word Count impl for testing hash map
#include <stdio.h>
#include <ctype.h>
#include "map.h"

#define BUF_SIZE 64


Map *m;
char buf[BUF_SIZE];

typedef struct {
  int count;
  char word[BUF_SIZE];
  MapNode node;
} Counter;


Counter *make_counter (char *word)
{
  Counter *c = malloc(sizeof(Counter));
  c->count = 0;
  strcpy(c->word, word);
  map_node_init(&c->node);
  return c;
}


int next_word (FILE *fp)
{
  memset(buf, 0, sizeof(buf));
  char *p = buf;
  char c;

  while ((c = fgetc(fp)) != EOF && !isalnum(c));
  if (c == EOF)
    return 0;

  *p++ = c;
  while ((c = fgetc(fp)) != EOF &&
         isalnum(c) &&
         (size_t) (p - buf) < BUF_SIZE)
    *p++ = c;

  return 1;
}


int main() {
  m = string_map_new(Counter, node, word);

  while (next_word(stdin)) {
    Counter *c = map_get(m, buf);
    if (!c) {
      c = make_counter(buf);
      map_add(m, c);
    }
    c->count++;
  }
  
  // print word counts
  MAP_FOREACH (Counter, c, m)
    printf("%s: %i\n", c->word, c->count);

  // free everything
  Counter *last = NULL;
  MAP_FOREACH (Counter, c, m) {
    if (last)
      free(last);
    last = c;
  }
  free(last);
  map_free(m);
  
  return 0;
}
