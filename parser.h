#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
void eat_char(Parse_Context *context)
{
  context->index++;
}

void try_parse_string(Parse_Context *context, Token *token, bool *success)
{
  Parse_Context internal_context = *context;
  bool quoted = peek_char(&internal_context) == '\'' || peek_char(&internal_context) == '"';
  char type_of_quote = quoted ? peek_char(&internal_context) : '\0';
  bool completed_string = false;

  if (quoted)
  {
    eat_char(&internal_context);
  }
  else if (!is_valid_string_char(peek_char(&internal_context)))
  {
    *success = false;
    return;
  }

  while (true)
  {
    if (is_valid_string_char(peek_char(&internal_context)))
    {
      eat_char(&internal_context);
      continue;
    }
    else if (quoted && peek_char(&internal_context) == type_of_quote)
    {
      eat_char(&internal_context);
      completed_string = true;
      break;
    }
    else if (is_whitespace(peek_char(&internal_context)) && !quoted)
    {

    }

    break;
  }

  if (completed_string)
  {
    token->type = STRING;
    token->data.string = (String_Token) { .cstring = "Oi" }; // @todo João, terminar aqui
  }

  *success = completed_string;
}

Sequence_Of_Tokens *parse(Parse_Context *context)
{
  Sequence_Of_Tokens *tokens = create_sequence_of_tokens(64, 64);

  while (context->index < context->length)
  {
    bool success_parsing = false;
    Token token = { 0 };

    skip_whitespace(context);

    try_parse_string(context, &token, &success_parsing);

    if (success_parsing)
    {
      push(tokens, token);
    }
    else
    {
      // @note por hora encerra aqui como não tem mais tipos de tokens
      break;
    }

  }

  return tokens;
}
