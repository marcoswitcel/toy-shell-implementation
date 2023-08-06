#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "./list.implementations.h"

typedef struct Parse_Context {
  const char *source;
  unsigned length;
  unsigned index;
  const char *error;
} Parse_Context;


Parse_Context create_parse_context(const char *source)
{
  return (Parse_Context) {
    .source = source,
    .length = strlen(source),
    .index = 0,
    .error = NULL,
  };
}

static inline const char* get_current_address(Parse_Context *context)
{
  return &context->source[context->index];
}

static inline bool is_whitespace(char value)
{
  return (value == ' ' || value == '\t' || value == '\r' || value == '\n');
}

static inline bool is_digit(char value)
{
  return value > ('0' - 1) && value < ('9' + 1);
}

static inline bool is_digit_not_zero(char value)
{
  return value > '0' && value < ('9' + 1);
}

static inline bool is_valid_string_char(char value)
{
  return isprint(value);
}

void skip_whitespace(Parse_Context *context)
{
  const char *source = get_current_address(context);
  char value;

  while ((value = *source) && is_whitespace(value))
  {
    source++;
  }

  context->index += source - get_current_address(context);
}

static inline char peek_char(Parse_Context *context)
{
  return context->source[context->index];
}

static inline void eat_char(Parse_Context *context)
{
  context->index++;
}

static inline bool is_finished(Parse_Context *context)
{
  return context->index >= context->length;
}

// @todo João, testar mais a fundo essa função
void try_parse_string(Parse_Context *context, Token *token, bool *success)
{
  Parse_Context internal_context = *context;
  bool quoted = peek_char(&internal_context) == '\'' || peek_char(&internal_context) == '"';
  char type_of_quote = quoted ? peek_char(&internal_context) : '\0';
  bool completed_string = false;
  bool escaped_quote = false;
  Buffer *buffer = create_buffer(1024, 1024);

  if (quoted)
  {
    eat_char(&internal_context);
  }
  else if (is_whitespace(peek_char(&internal_context)) || is_finished(&internal_context))
  {
    destroy_buffer(buffer);
    *success = false;
    return;
  }

  while (!is_finished(&internal_context))
  {
    char current_char = peek_char(&internal_context);

    if (current_char == '\\')
    {
      if (escaped_quote) break;

      escaped_quote = true;
      eat_char(&internal_context);
      continue;
    }
    else if (quoted && current_char == type_of_quote)
    {
      if (escaped_quote)
      {
        escaped_quote = false;
        eat_char(&internal_context);
        buffer_push(buffer, current_char);
        continue;
      }
      else
      {
        eat_char(&internal_context);
        completed_string = true;
        break;
      }
    }
    else if (is_whitespace(current_char))
    {
      if (quoted)
      {
        eat_char(&internal_context);
        buffer_push(buffer, current_char);
        continue;
      }
      else
      {
        completed_string = true;
        break;
      }
    }
    else if (is_valid_string_char(current_char))
    {
      eat_char(&internal_context);
      buffer_push(buffer, current_char);
      continue;
    }

    assert(true && "Por hora devo ter coberto todos os casos nas ramificações do IFs");
  }

  if (internal_context.index == internal_context.length)
  {
    if (!quoted)
    {
      completed_string = true;
    }
  }

  if (completed_string)
  {
    token->type = STRING;
    token->data.string = (String_Token) { .cstring = NULL }; // @todo João, terminar aqui
    token->data.string.cstring = copy(buffer_ensure_null_terminated_view(buffer)); // @todo João, ajustar leak aqui, ninguém está desalocando essa string

    *context = internal_context;
  }
  else if (quoted)
  {
    context->error = copy("String incompleta, àspas de fechamento faltando.");
  }

  destroy_buffer(buffer);
  *success = completed_string;
}

void try_parse_globbing(Parse_Context *context, Token *token, bool *success)
{
  if (peek_char(context) == '*')
  {
    eat_char(context);
    *success = true;
    token->type = GLOBBING;
    token->data.globbing = (Globbing_Token) { .cstring = NULL };
    token->data.globbing.cstring = copy("*");
    return;
  }

  *success = false;
}

void try_parse_redirect(Parse_Context *context, Token *token, bool *success)
{
  if (peek_char(context) == '>')
  {
    eat_char(context);
    *success = true;
    token->type = REDIRECT;
    token->data.redirect = (Redirect_Token) { .cstring = NULL };
    token->data.redirect.cstring = copy(">");
    return;
  }

  *success = false;
}

typedef void (*Parse_Function)(Parse_Context *, Token *, bool *);

const Parse_Function parse_functions[] = {
  &try_parse_globbing,
  &try_parse_redirect,
  &try_parse_string,
};

#define SIZEO_OF_ARRAY(array) (sizeof(array) / sizeof(array[0]))

// @note João, bug para corrigir, possivelmente mais bugs relacionados
// |>echo asd "asdads \' asd"
// asd
Sequence_Of_Tokens *tokenize(Parse_Context *context)
{
  Sequence_Of_Tokens *tokens = create_sequence_of_tokens(64, 64);

  while (!is_finished(context))
  {
    bool success_parsing = false;
    Token token = { 0 };

    skip_whitespace(context);

    bool progressed = false;
    for (unsigned i = 0; i < SIZEO_OF_ARRAY(parse_functions); i++)
    {
      Parse_Function parse_function = parse_functions[i];

      parse_function(context, &token, &success_parsing);

      if (success_parsing)
      {
        progressed = true;
        sequence_of_tokens_push(tokens, token);
        if (DEBUG_INFO && token.type == STRING) printf("[[ Token: '%s' ]]\n", token.data.string.cstring);
        break;
      }
      else if (context->error)
      {
        break;
      }
    }
    if (!progressed) break;
  }

  return tokens;
}
