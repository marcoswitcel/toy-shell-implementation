#ifndef TOKENS_H
#define TOKENS_H

typedef enum Token_Type {
  UNINITIALIZED = 0, STRING = 1, GLOBBING = 2, REDIRECT = 3,
} Token_Type;

typedef struct String_Token {
  const char *cstring;
} String_Token;

typedef struct Globbing_Token {
  const char *cstring;
} Globbing_Token;

typedef struct Redirect_Token {
  const char *cstring;
} Redirect_Token;

typedef struct Token {
  Token_Type type;
  signed token_index_start;
  union {
    String_Token string;
    Globbing_Token globbing;
    Redirect_Token redirect;
  } data;
} Token;

/*
void restore_token_to_default(Token *token)
{
  *token = (Token) {
    .type = UNINITIALIZED,
    .token_index_start = -1,
    .data = { 0 }, // @note continuar aqui
  };
}

Token create_default_empty_token()
{
  Token token;

  restore_token_to_default(&token);

  return  token;
}
*/

#endif // TOKENS_H
