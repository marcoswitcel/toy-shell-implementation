#ifndef TOKENS_H
#define TOKENS_H

#include <stdbool.h>

typedef enum Token_Type {
  UNINITIALIZED = 0, STRING = 1, GLOBBING = 2, REDIRECT = 3, PIPE = 4,
} Token_Type;

typedef struct String_Token {
  const char *cstring;
} String_Token;

typedef struct Globbing_Token {
  const char *cstring;
} Globbing_Token;

typedef struct Redirect_Token {
  const char *cstring;
  bool appending;
  int fd;
} Redirect_Token;

typedef struct Pipe_Token {
  const char *cstring;
} Pipe_Token;

typedef struct Token {
  Token_Type type;
  signed token_index_start;
  union {
    String_Token string;
    Globbing_Token globbing;
    Redirect_Token redirect;
    Pipe_Token pipe;
  } data;
} Token;

#define  STATIC_TOKEN(ENUM_TYPE) (Token) { .type = ENUM_TYPE, .token_index_start = -1, }

#endif // TOKENS_H
