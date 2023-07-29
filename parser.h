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

  while (internal_context.index < internal_context.length)
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
        if (buffer->index > 0)
        {
          completed_string = true;
        }
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

  destroy_buffer(buffer);
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
    // @todo João, falta parsear símbolos como ">" ">>" separadamente para poder tratar como comandos

    if (success_parsing)
    {
      push(tokens, token);
      if (DEBUG_INFO && token.type == STRING) printf("[[ Token: '%s' ]]\n", token.data.string.cstring);
    }
    else
    {
      // @note por hora encerra aqui como não tem mais tipos de tokens
      break;
    }

  }

  return tokens;
}
