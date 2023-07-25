#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct Parse_Context {
  const char *source;
  unsigned length;
  unsigned index;
} Parse_Context;


Parse_Context create_parse_context(const char *source)
{
  return (Parse_Context) {
    .source = source,
    .length = strlen(source),
    .index = 0,
  };
}