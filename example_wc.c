// Word Count impl for testing hash map and set
// gcc -std=c99 -Wall -O2 hash.c comparator.c map.c set.c example_wc.c -o wc
#include <stdio.h>
#include <ctype.h>
#include "map.h"
#include "set.h"
#include "comparator.h"

#define BUF_SIZE 64


typedef struct {
  int count;
  char word[BUF_SIZE];
  MapNode map_node;
  SetNode set_node;
} Counter;

char buf[BUF_SIZE];


int next_word (FILE *fp)
{
  memset(buf, 0, sizeof(buf));
  char *p = buf;
  char c;
  while
    ((c = fgetc(fp)) != EOF && !isalnum(c));
  if (c == EOF)
    return 0;
  do
    *p++ = c;
  while ((size_t) (p - buf) < BUF_SIZE &&
         (c = fgetc(fp)) != EOF &&
         isalnum(c));
  return 1;
}


int main(int argc, char *argv[])
{
  Map *m = string_map_new(Counter, map_node, word);
  Set *s = set_new(Counter, set_node, count);
  s->cmp = comparator_int;

  FILE *fp = fopen(argv[1], "r");
  while (next_word(fp)) {
    Counter *c = map_get(m, buf);
    if (!c) {
      c = malloc(sizeof(Counter));
      strcpy(c->word, buf);
      c->count = 0;
      map_add(m, c);
    }
    c->count++;
  }
  fclose(fp);

  // add words to set
  int cutoff;
  if (argc > 2)
    sscanf(argv[2], "%i", &cutoff);
  else
    cutoff = 0;
  MAP_FOREACH (Counter, c, m)
    if (c->count >= cutoff)
      set_insert(s, c);

  // print sorted word counts
  SET_FOREACH (Counter, c, s)
    printf("%s: %i\n", c->word, c->count);

  map_free_all(m, free);
  return 0;
}
