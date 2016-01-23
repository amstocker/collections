// Word Count impl for testing hash map
#include <stdio.h>
#include <ctype.h>
#include "map.h"

#define BUF_SIZE 64


typedef struct {
  int count;
  char word[BUF_SIZE];
  MapNode node;
} Counter;

char buf[BUF_SIZE];


int next_word (FILE *fp)
{
  memset(buf, 0, sizeof(buf));
  char *p = buf;
  char c;
  while ((c = fgetc(fp)) != EOF && !isalnum(c));
  if (c == EOF)
    return 0;
  *p++ = c;
  while ((size_t) (p - buf) < BUF_SIZE &&
         (c = fgetc(fp)) != EOF &&
         isalnum(c))
    *p++ = c;
  return 1;
}


int main() {
  Map *m = string_map_new(Counter, node, word);

  while (next_word(stdin)) {
    Counter *c = map_get(m, buf);
    if (!c) {
      c = malloc(sizeof(Counter));
      strcpy(c->word, buf);
      c->count = 0;
      map_add(m, c);
    }
    c->count++;
  }
  
  // print word counts
  MAP_FOREACH (Counter, c, m)
    printf("%s: %i\n", c->word, c->count);

  map_free_all(m, free);
  return 0;
}
