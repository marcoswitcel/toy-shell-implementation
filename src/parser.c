#ifndef _PARSER_C_
#define _PARSER_C_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#include "./list.implementations.h"
#include "./sorting.c"
#include "./process_manager.c"
#include "./utils.c"
#include "./utils.macro.h"
#include "./nodes.h"
#include "./debug_log.c"
#include "./tokens.h"
#include "./buffer.h"

typedef struct Parse_Context {
  const char *source;
  unsigned length;
  unsigned index;
  const char *error;
  signed error_start_index;
  unsigned token_index;
} Parse_Context;


Parse_Context create_parse_context(const char *source)
{
  return (Parse_Context) {
    .source = source,
    .length = strlen(source),
    .index = 0,
    .error = NULL,
    .error_start_index = -1,
    .token_index = 0,
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
      // @note João, avaliar se vai voltar pro copy aqui
      token->data.pipe.cstring = "|";
      return;
    }

    context->error = copy("Esperava espaço em branco após a pipe.");
    context->error_start_index = context->index + 1;
  }

  *success = false;
}

void try_parse_and(Parse_Context *context, Token *token, bool *success)
{
  if (peek_char(context) == '&')
  {
    if (peek_next_char(context) == '&')
    {
      if (is_whitespace(peek_char_forward(context, 2)))
      {
        token->token_index_start = context->index;
        eat_char(context);
        eat_char(context);
        *success = true;
        token->type = AND;
        token->data.and = (And_Token) { .cstring = NULL };
        // @note João, para fins de teste, vou deixar essa memória como sendo um referência para
        // a string "&&", até aonde sei, essa string vai ganhar um endereço readonly fora da stack.
        // Teoricamente é seguro passar adiante desde que eu não tente chamar free nessa referência.
        // token->data.and.cstring = copy("&&");
        token->data.and.cstring = "&&";
        return;
      }

      // @note João, se o input terminou não terá espaço em branco, é mais instrutivo
      // explicar para o usuário que após o && deveria vir algum comando, isso também pode ser feito
      // na função `parse_execute_command_node`
      if (peek_char_forward(context, 2) == '\0')
      {
        context->error = copy("Comando incompleto, esperava uma continuação.");
      }
      else {
        context->error = copy("Esperava espaço em branco após o &&.");
      }

      context->error_start_index = context->index + 2;
    }
    else
    {
      context->error = copy("Esperava encontrar mais um &.");
      context->error_start_index = context->index + 1;
    }
  }

  *success = false;
}

void try_parse_query_last_status(Parse_Context *context, Token *token, bool *success)
{
  if (peek_char(context) == '$')
  {
    if (peek_next_char(context) == '?')
    {
      if (is_whitespace(peek_char_forward(context, 2)) || peek_char_forward(context, 2) == '\0')
      {
        token->token_index_start = context->index;
        eat_char(context);
        eat_char(context);
        *success = true;
        token->type = QUERY_LAST_STATUS;
        token->data.query_last_status = (Query_Last_Status_Token) { .cstring = NULL };
        // @note João, avaliar se vai voltar pro copy aqui
        token->data.and.cstring = "$?";
        return;
      }

      context->error = copy("Esperava espaço em branco após o $?.");
      context->error_start_index = context->index + 2;
    }
    else
    {
      context->error = copy("Esperava encontrar mais um ?.");
      context->error_start_index = context->index + 1;
    }
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
      // @note João, avaliar se vai voltar pro copy aqui
      token->data.globbing.cstring = "*";
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
      // @note João, avaliar se vai voltar pro copy aqui
      token->data.redirect.cstring = ">";
      token->data.redirect.fd = fd;
      token->data.redirect.appending = appending;
      return;
    }

    context->error = copy("Esperava espaço em branco após >.");
    context->error_start_index = context->index + n_char_forward;
  }

  *success = false;
}

/**
 * @brief Protótipo/assinatura para as funções que realizam o 'parse' dos tokens.
 * 
 * Algumas observações gerais para fins de documentação:
 * - o booleano de sucesso deve sempre ser atualizado com o status, false para 
 *   fracasso e true para sucesso.
 * - caso seja sinalizado o sucesso o token deve ser escrito para o endereço provido.
 * - em caso de fracasso no 'parse' se for adicionado um erro ao contexto será 
 *   considerado com um erro impeditivo, se não for, será considerado como um não 
 *   'match' e será executado o próximo 'parser'.
 * 
 * @param context contexto de parse atual
 * @param token endereço de memória do token que deve ser preenchido
 * @param success endereço do 'boolean' que que deve ser atualizado com o status de sucesso do 'parse'
 */
typedef void (*Parse_Function)(Parse_Context *, Token *, bool *);

const Parse_Function parse_functions[] = {
  &try_parse_globbing,
  &try_parse_redirect,
  &try_parse_pipe,
  &try_parse_and,
  &try_parse_query_last_status,
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

        if (DEBUG_INFO) Debug_Log_Line("consumido token '%s', tipo atribuído: '%s'", token_to_string(&token), type_of_token_as_cstring(&token));
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

/**
 * @brief Função que faz o release das memórias alocadas dinâmicamentes 
 * 
 * @note Deveria apenas chamar alguma função tipo `destroy_token_type` pra manter agrupadas
 * as reponsabilidades.
 * @note Essa função não está sendo usada por hora por as STRINGS são passadas adiante para os `Node`
 * e vira responabilidade deles fazerem a limpeza.
 * 
 * @param tokens 
 */
void release_cstrings_from_tokens(Sequence_Of_Tokens *tokens)
{
  for (unsigned i = 0; i < tokens->index; i++)
  {
    if (tokens->data[i].type == STRING)
    {
      FREE_AND_NULLIFY(tokens->data[i].data.string.cstring);
    }
  }
}

Execute_Command_Node parse_execute_command_node_internal(Parse_Context *context, const Sequence_Of_Tokens *tokens, bool piping)
{
  List_Of_Strings *list_of_args = create_list_of_strings(1024, 1024);
  bool has_stdout_redirect_token = false;
  bool has_stderr_redirect_token = false;
  bool stdout_redirect_expect_file_name = false;
  bool stderr_redirect_expect_file_name = false;
  const char *stdout_redirect_filename = NULL;
  const char *stderr_redirect_filename = NULL;
  signed token_index_start = -1;
  bool append_mode_stdout = false;
  bool append_mode_stderr = false;
  bool piped = false;
  Execute_Command_Node *pipe = NULL;
  bool next_command_found = false;
  Execute_Command_Node *next_command_node = NULL;
  
  for (unsigned i = context->token_index; i < tokens->index; i++)
  {
    context->token_index = i;
    
    Token token = tokens->data[i];
    assert(token.token_index_start > -1);
    assert(token_to_string(&token) != NULL);
    assert(token.type != UNINITIALIZED);

    /**
     * @todo João, aqui o início do processo eu posso checar todos os tokens que são mandatórios, por exemplo:
     * Quando estiver esperando um nome de arquivo, graças a um comando de redirect, seria interessante checar isso
     * logo no começo do loop e reportar o erro na hora, isso para não precisar replicar essa lógica para dentro de cada
     * `if` abaixo.
     * @note Primeiro if trazido para cá, embora ainda é necessário mantê-lo após o termino do loop também
     */
    if ((stdout_redirect_expect_file_name || stderr_redirect_expect_file_name) && token.type != STRING)
    {
      parse_context_report_error(context, "Esperando um nome de arquivo e encontrou.", token.token_index_start);
      break;
    }

    if (token.type == STRING)
    {
      if (stdout_redirect_expect_file_name || stderr_redirect_expect_file_name)
      {
        if (stdout_redirect_expect_file_name)
        {
          stdout_redirect_expect_file_name = false;
          stdout_redirect_filename = token.data.string.cstring;
          token_index_start = token.token_index_start; // @todo João, resetando em casos de setar separado
        }

        if (stderr_redirect_expect_file_name)
        {
          stderr_redirect_expect_file_name = false;
          stderr_redirect_filename = token.data.string.cstring;
          token_index_start = token.token_index_start; // @todo João, resetando em casos de setar separado
        }
      }
      else
      {
        list_of_strings_push(list_of_args, token.data.string.cstring);
      }
    }

    if (token.type == GLOBBING)
    {
      if (stdout_redirect_expect_file_name || stderr_redirect_expect_file_name)
      {
        parse_context_report_error(context, "Token * não é um argumento válido para o redirect.", token.token_index_start);
        break;
      }
      else
      {
        list_of_strings_push(list_of_args, static_globbing_symbol);
      }
    }

    if (token.type == REDIRECT)
    {
      int fd = token.data.redirect.fd;

      if ((fd == STDOUT_FILENO && has_stdout_redirect_token) ||
          (fd == STDERR_FILENO && has_stderr_redirect_token) ||
          (fd != STDOUT_FILENO && fd != STDERR_FILENO && (has_stdout_redirect_token  || has_stderr_redirect_token)))
      {
        parse_context_report_error(context, "Token > encontrado mais de uma vez.", token.token_index_start);
        break;
      }
      // @note João, os dois `else if` a seguir possivelmente precisarão existir em outros comandos
      // a regra de parsing começa a ficar cada vez mais complexa aqui. @duplicado @especifico
      else if (list_of_args->index == 0)
      {
        parse_context_report_error(context, "Token de redirect > encontrado antes de qualquer argumento.", token.token_index_start);
        break;
      }
      else if (context->token_index+1 >= tokens->index)
      {
        parse_context_report_error(context, "Nome do arquivo que deve receber o redirecionamento não foi especificado e não há nada mais para parsear.", token.token_index_start);
        break;
      }
      
      if (fd == STDOUT_FILENO)
      {
        has_stdout_redirect_token = true;
        stdout_redirect_expect_file_name = true;
        append_mode_stdout = token.data.redirect.appending;
      }
      else if (fd == STDERR_FILENO)
      {
        has_stderr_redirect_token = true;
        stderr_redirect_expect_file_name = true;
        append_mode_stderr = token.data.redirect.appending;
      }
      else
      {
        has_stdout_redirect_token = true;
        stdout_redirect_expect_file_name = true;
        has_stderr_redirect_token = true;
        stderr_redirect_expect_file_name = true;
        append_mode_stdout = token.data.redirect.appending;
        append_mode_stderr = token.data.redirect.appending;
      } 
    }

    if (token.type == PIPE)
    {
      // @note João, a princípio não deve acontecer pois é acionado o parsemento recursivo,
      // mas por precaução essa lógica fica
      if (piped)
      {
        parse_context_report_error(context, "Token | encontrado mais de uma vez.", token.token_index_start);
        break;
      } else if (has_stdout_redirect_token)
      {
        parse_context_report_error(context, "Token | encontrado, porém ele conflita com uma regra de redirecionamento de stdout anterior.", token.token_index_start);
        break;
      }
      // @note João, os dois `else if` a seguir possivelmente precisarão existir em outros comandos
      // a regra de parsing começa a ficar cada vez mais complexa aqui. @duplicado @especifico
      else if (list_of_args->index == 0)
      {
        parse_context_report_error(context, "Token | encontrado antes de qualquer argumento.", token.token_index_start);
        break;
      }
      else if (context->token_index+1 >= tokens->index)
      {
        parse_context_report_error(context, "Token | encontrado mas não há nada mais para parsear.", token.token_index_start);
        break;
      }
      else
      {
        Execute_Command_Node *execute_command_sub_node = ALLOC(Execute_Command_Node, 1);
        context->token_index += 1;
        assert(context->token_index == (i + 1)); // @note temporário enquanto faço a transição do formato de passagem dos parâmetros
        *execute_command_sub_node = parse_execute_command_node_internal(context, tokens, true);
        pipe = execute_command_sub_node;
        piped = true;

        // @todo João, testar se não dá pra dar apenas um continue e deixar o loop principal encerrar, ficaria menos código aqui
        // @note Visando remover o todo acima, adicionei mais um teste. Revisar novamente e caso não houver falhar, apenas remover
        // os todos acia e o if e o else, deixando apenas o 'update' da variável 'i' e o 'continue'. Revisar.
        if (context->token_index < tokens->index)
        {
          i = context->token_index;
          continue;
        }
        else
        {
          break;
        }
      }
    }

    if (token.type == AND)
    {
      if (piping)
      {
        context->token_index -= 1;
        break;
      }

      // @note João, a princípio não deve acontecer pois é acionado o parsemento recursivo,
      // mas por precaução essa lógica fica
      if (next_command_found)
      {
        parse_context_report_error(context, "Token && encontrado mais de uma vez.", token.token_index_start);
        break;
      }
      // @note João, os dois `else if` a seguir possivelmente precisarão existir em outros comandos
      // a regra de parsing começa a ficar cada vez mais complexa aqui. @duplicado @especifico
      else if (list_of_args->index == 0)
      {
        parse_context_report_error(context, "Token && encontrado antes de qualquer argumento.", token.token_index_start);
        break;
      }
      else if (context->token_index+1 >= tokens->index)
      {
        parse_context_report_error(context, "Token && encontrado mas não há nada mais para parsear.", token.token_index_start);
        break;
      }
      else
      {
        Execute_Command_Node *execute_command_sub_node = ALLOC(Execute_Command_Node, 1);
        context->token_index += 1;
        assert(context->token_index == (i + 1)); // @note temporário enquanto faço a transição do formato de passagem dos parâmetros
        *execute_command_sub_node = parse_execute_command_node_internal(context, tokens, false);
        next_command_node = execute_command_sub_node;
        next_command_found = true;

        break;
      }
    }

    if (token.type == QUERY_LAST_STATUS)
    {
      if (stdout_redirect_expect_file_name || stderr_redirect_expect_file_name)
      {
        parse_context_report_error(context, "Token $? não é um argumento válido para o redirect.", token.token_index_start);
        break;
      }
      else
      {
        list_of_strings_push(list_of_args, static_query_last_status_code_symbol);
      }
    }
  }

  if ((stdout_redirect_expect_file_name || stderr_redirect_expect_file_name) && context->error == NULL)
  {
    parse_context_report_error(context, "Nome do arquivo que deve receber o redirecionamento não foi especificado.", context->length);
  }

  // @todo João, analisar se considerando a necessidade de expandir a lista de argumentos na função `replace_static_symbols_with_query_info`
  // não seria interessante que o atributo `args` fosse na verdade uma lista de strings ao invés de um char ** terminado com null.
  // Por hora decidi montar uma lista nova na função e reconstruir o array
  // @todo João, notei que estou alocando um bloco de memória nos casos de lista vazia, na prática o que faço é criar uma lista
  // de um item com `null` como valor
  Null_Terminated_Pointer_Array args = convert_list_to_argv(list_of_args);

  destroy_list_of_strings(list_of_args);

  return (Execute_Command_Node) { .args = args, .stdout_redirect_filename = stdout_redirect_filename, .stderr_redirect_filename = stderr_redirect_filename, .token_index_start = token_index_start, .append_mode_stdout = append_mode_stdout, .append_mode_stderr = append_mode_stderr, .pipe = pipe, .next_command = next_command_node };
}

// @todo João, acho que é melhor retornar uma referência alocada no heap
// @todo João, coloquei uma anotação na função `try_parse_and` sobre reportar de forma mais clara que após o && deve haver algum
// input (checar em outros dos métodos de parse). Hoje o sistema apenas avisa sobre falta de espaço, mas se realizar essa correção ainda estará errado. Cai no assert da função
// `shell_execute_command`.
Execute_Command_Node parse_execute_command_node(Parse_Context *context, const Sequence_Of_Tokens *tokens)
{
  return parse_execute_command_node_internal(context, tokens, false);
}

#endif // _PARSER_C_
