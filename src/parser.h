#ifndef PARSER_H
#define PARSER_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "./list.implementations.h"
#include "./process_manager.c"
#include "./utils.c"
#include "./utils.macro.h"
#include "./nodes.h"
#include "./buffer.h"

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

static inline const char* get_current_address(const Parse_Context *context)
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

static inline char peek_char(const Parse_Context *context)
{
  return context->source[context->index];
}

static inline char peek_next_char(const Parse_Context *context)
{
  if (context->index + 1 >= context->length) return '\0';
  return context->source[context->index + 1];
}

static inline char peek_char_forward(const Parse_Context *context, unsigned index)
{
  if (context->index + index >= context->length) return '\0';
  return context->source[context->index + index];
}

static inline void eat_char(Parse_Context *context)
{
  context->index++;
}

static inline bool is_finished(const Parse_Context *context)
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
    // @todo João, essa memória vai parar na função `launch_process` como o args.
    // Desalocar após a execução seria o mais correto até está sendo feito isso,
    // porém, muito fácil de esquecer e reintroduzir o bug.
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

void try_parse_pipe(Parse_Context *context, Token *token, bool *success)
{
  if (peek_char(context) == '|')
  {
    if (is_whitespace(peek_next_char(context)))
    {
      token->token_index_start = context->index;
      eat_char(context);
      *success = true;
      token->type = PIPE;
      token->data.pipe = (Pipe_Token) { .cstring = NULL };
      token->data.pipe.cstring = copy("|");
      return;
    }

    context->error = copy("Esperava espaço em branco após a pipe.");
    context->error_start_index = context->index + 1;
  }

  *success = false;
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
  bool appending = false;
  unsigned n_char_forward = 0;
  int fd = -1;
  
  if ((peek_char(context) == '1' || peek_char(context) == '2') && peek_next_char(context) == '>')
  {
    fd = peek_char(context) - '0';
    n_char_forward++;
  }

  if (peek_char_forward(context, n_char_forward) == '>')
  {
    n_char_forward ++;
    if (peek_char_forward(context, n_char_forward) == '>')
    {
      appending = true;
      n_char_forward++;
    }

    if (is_whitespace(peek_char_forward(context, n_char_forward)) || peek_char_forward(context, n_char_forward) == '\0')
    {
      token->token_index_start = context->index;
      eat_char(context);
      if (fd > -1)
      {
        eat_char(context);
      }
      if (appending)
      {
        eat_char(context);
      }
      *success = true;
      token->type = REDIRECT;
      token->data.redirect = (Redirect_Token) { .cstring = NULL };
      token->data.redirect.cstring = copy(">");
      token->data.redirect.appending = appending;
      return;
    }

    context->error = copy("Esperava espaço em branco após >.");
    context->error_start_index = context->index + n_char_forward;
  }

  *success = false;
}

typedef void (*Parse_Function)(Parse_Context *, Token *, bool *);

const Parse_Function parse_functions[] = {
  &try_parse_globbing,
  &try_parse_redirect,
  &try_parse_pipe,
  &try_parse_string,
};

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

// @todo João, acho que é melhor retonar uma referência alocada no heap
Execute_Command_Node parse_execute_command_node(Parse_Context *context, const unsigned first_token_index, const Sequence_Of_Tokens *tokens)
{
  List_Of_Strings *list_of_args = create_list_of_strings(1024, 1024);
  bool has_redirec_token = false;
  bool redirect_expect_file_name = false;
  const char *output_filename = NULL;
  signed token_index_start = -1;
  bool append_mode = false;
  bool piped = false;
  Execute_Command_Node *pipe = NULL;
  
  for (unsigned i = first_token_index; i < tokens->index; i++)
  {
    Token token = tokens->data[i];
    assert(token.token_index_start > -1);

    if (token.type == STRING && token.data.string.cstring)
    {
      if (redirect_expect_file_name)
      {
        redirect_expect_file_name = false;
        output_filename = token.data.string.cstring;
        token_index_start = token.token_index_start;
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
      append_mode = token.data.redirect.appending;
      redirect_expect_file_name = true;
    }

    if (token.type == PIPE  && token.data.pipe.cstring)
    {
      // @note João, a princípio não deve acontecer pois é acionado o parsemento recursivo,
      // mas por precaução essa lógica fica
      if (piped)
      {
        parse_context_report_error(context, "Token | encontrado mais de uma vez.", token.token_index_start);
        break;
      }
      else
      {
        Execute_Command_Node *execute_command_sub_node = ALLOC(Execute_Command_Node, 1);
        *execute_command_sub_node = parse_execute_command_node(context, i + 1, tokens);
        pipe = execute_command_sub_node;
        piped = true;

        break;
      }
    }
  }

  if (redirect_expect_file_name && context->error == NULL)
  {
    parse_context_report_error(context, "Nome do arquivo que deve receber o output não foi especificado.", context->length);
  }

  Null_Terminated_Pointer_Array args = convert_list_to_argv(list_of_args);

  destroy_list_of_strings(list_of_args);

  return (Execute_Command_Node) { .args = args, .output_filename = output_filename, .token_index_start = token_index_start, .append_mode = append_mode, .pipe = pipe, };
}

#endif // PARSER_H
