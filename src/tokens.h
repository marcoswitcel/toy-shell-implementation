#ifndef TOKENS_H
#define TOKENS_H

#include <stdbool.h>
#include <assert.h>

typedef enum Token_Type {
  UNINITIALIZED = 0, STRING = 1, GLOBBING = 2, REDIRECT = 3, PIPE = 4, AND = 5, QUERY_LAST_STATUS = 6,
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

typedef struct And_Token {
  const char *cstring;
} And_Token;

typedef struct Query_Last_Status_Token {
  const char *cstring;
} Query_Last_Status_Token;

typedef struct Token {
  Token_Type type;
  signed token_index_start;
  union {
    String_Token string;
    Globbing_Token globbing;
    Redirect_Token redirect;
    Pipe_Token pipe;
    And_Token and;
    Query_Last_Status_Token query_last_status;
  } data;
} Token;

/**
 * @brief representa um símbolo identificado pelo parser
 * @note Hoje essa string e adicionada ao array de argumentos e uma etapa anterior a identifica e substitui pelo valor.
 * A etapa que faz a substituição tira proveito do fato que o endereço é fixo e conhecido para identificar símbolos que
 * precisam ser substituídos, não sei se é uma solução duradoura, mas temporariamente serve.
 */
static const char static_query_last_status_code_symbol[] = "$?";
static const char static_globbing_symbol[] = "*";

#define  STATIC_TOKEN(ENUM_TYPE) (Token) { .type = ENUM_TYPE, .token_index_start = -1, }

const char *token_to_string(Token *token)
{
  switch (token->type)
  {
    case STRING:            return token->data.string.cstring;
    case GLOBBING:          return token->data.globbing.cstring;
    case REDIRECT:          return token->data.redirect.cstring;
    case PIPE:              return token->data.pipe.cstring;
    case AND:               return token->data.and.cstring;
    case QUERY_LAST_STATUS: return token->data.query_last_status.cstring;
    case UNINITIALIZED:     assert("Token não inicializado.");
  }

  assert("Token não reconhecido.");
  return NULL;
}

const char *type_of_token_as_cstring(Token *token)
{
  switch (token->type)
  {
    case STRING:            return "String";
    case GLOBBING:          return "Globbing";
    case REDIRECT:          return "Redirect";
    case PIPE:              return "Pipe";
    case AND:               return "And";
    case QUERY_LAST_STATUS: return "Query_Last_Status";
    case UNINITIALIZED:     assert("Token não inicializado.");
  }

  assert("Tipo de token não reconhecido.");
  return NULL;
}

#endif // TOKENS_H
