#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "./list.implementations.h"
#include "./process_manager.c"
#include "./utils.c"
#include "./nodes.h"

typedef struct Parse_Context {
  const char *source;
  unsigned length;
  unsigned index;
  const char *error;
  signed error_start_index;
} Parse_Context;


Parse_Context create_parse_context(const char *source)
{
  return (Parse_Context) {
    .source = source,
    .length = strlen(source),
    .index = 0,
    .error = NULL,
    .error_start_index = -1,
  };
}

void parse_context_report_error(Parse_Context *context, const char *message, signed error_start_index)
{
  context->error = copy(message);
  context->error_start_index = error_start_index;
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

static inline char peek_next_char(Parse_Context *context)
{
  if (context->index + 1 >= context->length) return '\0';
  return context->source[context->index + 1];
}

static inline void eat_char(Parse_Context *context)
{
  context->index++;
}

static inline bool is_finished(Parse_Context *context)
{
  return context->index >= context->length;
}

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

    if (escaped_quote && (current_char != '\\' && current_char != type_of_quote))
    {
      char message[] = "Escape inválido, deve ser seguido de uma \\ ou ?";
      message[47] = type_of_quote;

      context->error = copy(message);
      context->error_start_index = internal_context.index - 1;
      break;
    }

    if (current_char == '\\')
    {
      if (quoted)
      {
        if (escaped_quote)
        {
          escaped_quote = false;
          eat_char(&internal_context);
          buffer_push(buffer, current_char);
          continue;
        }

        escaped_quote = true;
        eat_char(&internal_context);
        continue;
      }
      else
      {
        context->error = copy("Caractere inesperado.");
        context->error_start_index = internal_context.index;
        break;
      }
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
    token->data.string = (String_Token) { .cstring = NULL };
    // @todo João, ajustar leak aqui, ninguém está desalocando essa string
    // essa memória vai para na função `launch_process` como o args. Desalocar após a execução seria o mais correto
    token->data.string.cstring = copy(buffer_ensure_null_terminated_view(buffer));
    token->token_index_start = context->index;

    *context = internal_context;
  }
  else if (quoted && is_finished(&internal_context))
  {
    context->error = copy("String incompleta, àspas de fechamento faltando.");
    context->error_start_index = internal_context.index;
  }

  destroy_buffer(buffer);
  *success = completed_string;
}

void try_parse_globbing(Parse_Context *context, Token *token, bool *success)
{
  if (peek_char(context) == '*')
  {
    if (is_whitespace(peek_next_char(context)) || peek_next_char(context) == '\0')
    {
      token->token_index_start = context->index;
      eat_char(context);
      *success = true;
      token->type = GLOBBING;
      token->data.globbing = (Globbing_Token) { .cstring = NULL };
      token->data.globbing.cstring = copy("*");
      return;
    }

    context->error = copy("Esperava espaço em branco após *.");
    context->error_start_index = context->index + 1;
  }

  *success = false;
}

void try_parse_redirect(Parse_Context *context, Token *token, bool *success)
{
  if (peek_char(context) == '>')
  {
    token->token_index_start = context->index;
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

#define SIZE_OF_ARRAY(array) (sizeof(array) / sizeof(array[0]))

// @note João, bug para corrigir, possivelmente mais bugs relacionados
// |>echo asd "asdads \' asd"
// asd
Sequence_Of_Tokens *tokenize(Parse_Context *context)
{
  Sequence_Of_Tokens *tokens = create_sequence_of_tokens(64, 64);

  while (!is_finished(context))
  {
    bool success_parsing = false;
    Token token = STATIC_TOKEN(UNINITIALIZED);

    skip_whitespace(context);

    bool progressed = false;
    for (unsigned i = 0; i < SIZE_OF_ARRAY(parse_functions); i++)
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

Execute_Command_Node parse_execute_command_node(Parse_Context *context, const Sequence_Of_Tokens *tokens)
{
  // Aqui a tokenização acaba e começa o análise léxica acaba e começa a análise sintática
  // após um token '>' deve sempre vir uma string, com o nome do arquivo, mas aí já é semântica
  List_Of_Strings *list_of_args = create_list_of_strings(1024, 1024);
  bool has_redirec_token = false;
  bool redirect_expect_file_name = false;
  const char *output_filename = NULL;
  for (unsigned i = 0; i < tokens->index; i++)
  {
    Token token = tokens->data[i];
    assert(token.token_index_start > -1); // @note só para garantir que não estou errando nada

    if (token.type == STRING && token.data.string.cstring)
    {
      if (redirect_expect_file_name)
      {
        redirect_expect_file_name = false;
        output_filename = token.data.string.cstring;
      }
      else
      {
        list_of_strings_push(list_of_args, token.data.string.cstring);
      }
    }
    if (token.type == GLOBBING && token.data.globbing.cstring)
    {
      if (redirect_expect_file_name)
      {
        parse_context_report_error(context, "Token * não é um argumento válido para o redirect.", token.token_index_start);
        break;
      }
      else
      {
        get_all_files_for_dir(".", list_of_args, false);
      }
    }
    if (token.type == REDIRECT && token.data.redirect.cstring)
    {
      if (has_redirec_token)
      {
        parse_context_report_error(context, "Token > encontrado mais de uma vez.", token.token_index_start);
        break;
      }
      has_redirec_token = true;
      redirect_expect_file_name = true;
    }
  }

  if (redirect_expect_file_name && context->error == NULL)
  {
    parse_context_report_error(context, "Nome do arquivo que deve receber o output não foi especificado.", context->length);
  }

  Null_Terminated_Pointer_Array args = convert_list_to_argv(list_of_args);

  destroy_list_of_strings(list_of_args);

  return (Execute_Command_Node) { .args = args, .output_filename = output_filename, };
}
