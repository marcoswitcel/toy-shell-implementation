#ifndef _SHELL_C_
#define _SHELL_C_

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

#include "./compilation_definitions.h"
#include "./list.implementations.h"
#include "./parser.c"
#include "./process_manager.c"
#include "./types.h"
#include "./nodes.h"
#include "./shell_builtins.c"
#include "./terminal.c"
#include "./utils.macro.h"
#include "./dev-utils.c"
#include "./utils.c"
#include "./debug_log.c"
#include "./sorting.c"
#include "./ansi-escape-sequences.h"
#include "./shell.h"

#define HISTORY_MAX_ELEMENTS 20

Shell_Context_Data *the_shell_context;

Shell_Context_Data create_shell_context_data()
{
  return (Shell_Context_Data) {
    .colorful = false,
    .soundful = true,
    .last_typed_commands = create_list_of_strings(64, 64),
    .next_typed_command_to_show = -1,
    .last_status_code = 0,
    .input_mark = default_input_mark,
    .input_mark_length = SIZE_OF_STATIC_STRING(default_input_mark),
  };
}

#define LINE_BUFFER_SIZE 1024

static inline void print_input_mark(Shell_Context_Data *context, const char *cstring)
{
  // @note lindando com a bufferização por enquanto
  // @reference https://stackoverflow.com/questions/41120879/mixing-syscall-write-with-printf-on-linux
  // fflush(stdout); // @note esta linha e a abaixo resolvem o problema de bufferização
  setbuf(stdout, NULL);

  if (context->colorful) write(STDOUT_FILENO, GREEN, SIZE_OF_STATIC_STRING(GREEN));

  write(STDOUT_FILENO, context->input_mark, context->input_mark_length);

  if (context->colorful) write(STDOUT_FILENO, RESET, SIZE_OF_STATIC_STRING(RESET));

  if (cstring)
  {
    write(STDOUT_FILENO, cstring, strlen(cstring));
  }
}

void should_emmit_ring_bell(Shell_Context_Data *context)
{
  if (context->soundful)
  {
    emmit_ring_bell();
  }
}

typedef enum Key_Pressed {
  ARROW_UP,
  ARROW_DOWN,
  ARROW_LEFT,
  ARROW_RIGHT,
  ALT_ARROW_UP,
  ALT_ARROW_DOWN,
  ALT_ARROW_LEFT,
  ALT_ARROW_RIGHT,
  DELETE,
  HOME,
  END,
  UNKNOWN,
} Key_Pressed;

static const char* key_pressed_name(Key_Pressed key)
{
  switch (key)
  {
    case ARROW_UP:        return "ARROW_UP";
    case ARROW_DOWN:      return "ARROW_DOWN";
    case ARROW_LEFT:      return "ARROW_LEFT";
    case ARROW_RIGHT:     return "ARROW_RIGHT";
    case ALT_ARROW_UP:    return "ALT_ARROW_UP";
    case ALT_ARROW_DOWN:  return "ALT_ARROW_DOWN";
    case ALT_ARROW_LEFT:  return "ALT_ARROW_LEFT";
    case ALT_ARROW_RIGHT: return "ALT_ARROW_RIGHT";
    case DELETE:          return "DELETE";
    case HOME:            return "HOME";
    case END:             return "END";
    case UNKNOWN:         return "UNKNOWN";
  }

  return "(código desconhecido)";
}

/**
 * @brief converte sequências de escape em um representação interna para a sequência
 * @reference https://viewsourcecode.org/snaptoken/kilo/03.rawInputAndOutput.html#the-home-and-end-keys
 * @note Para fins de depuração adicionei log dos bytes consumidos
 * 
 * @return Key_Pressed 
 */
static Key_Pressed try_process_escape_sequence()
{
  Debug_Log_Line(" -- ");

  int c = getchar();
  Debug_Log_Line("byte: '%c'", c);
  if (c == '[')
  {
    c = getchar();
    Debug_Log_Line("byte: '%c'", c);
    switch(c)
    {
      case 'A': return ARROW_UP;
      case 'B': return ARROW_DOWN;
      case 'C': return ARROW_RIGHT;
      case 'D': return ARROW_LEFT;
      case 'H': return HOME;
      case 'F': return END;
      case '1':
      {
        c = getchar();
        Debug_Log_Line("byte: '%c'", c);
        if (c == '~') return HOME;
        if (c == ';')
        {
          c = getchar();
          Debug_Log_Line("byte: '%c'", c);
          /**
           * @note Pelo que li tem mais sequências que poderia ser enviadas para representar
           * um alt + arrow_up/arrow_down/arrow_left/arrow_right. Por hora estou tratando só essa.
           * <ESC>[1;3A  <ESC>[1;3B  <ESC>[1;3C  <ESC>[1;3D
           */
          if (c == '3')
          {
            c = getchar();
            Debug_Log_Line("byte: '%c'", c);
            switch (c)
            {
              case 'A':
              {
                return ALT_ARROW_UP;
              }
              case 'B':
              {
                return ALT_ARROW_DOWN;
              }
              case 'C':
              {
                return ALT_ARROW_RIGHT;
              }
              case 'D':
              {
                return ALT_ARROW_LEFT;
              }
            }  
          }
        }
      } break;
      case '3':
      {
        c = getchar();
        Debug_Log_Line("byte: '%c'", c);
        if (c == '~') return DELETE;
      } break;
      case '4':
      {
        c = getchar();
        Debug_Log_Line("byte: '%c'", c);
        if (c == '~') return END;
      } break;
      case '7':
      {
        c = getchar();
        Debug_Log_Line("byte: '%c'", c);
        if (c == '~') return HOME;
      } break;
      case '8':
      {
        c = getchar();
        Debug_Log_Line("byte: '%c'", c);
        if (c == '~') return END;
      } break;
    }

    return UNKNOWN;
  }
  else if (c == 'O')
  {
    c = getchar();
    Debug_Log_Line("byte: '%c'", c);
    switch(c)
    {
      case 'H': return HOME;
      case 'F': return END;
    }

    assert(false && "não lidando com sequências inválidas ainda");
    return UNKNOWN;
  }
  else
  {
    assert(false && "não lidando com sequências inválidas ainda");
    return UNKNOWN;
  }
}

/**
 * @brief Função que avança uma "palavra" para a esquerda
 * 
 * @param buffer 
 * @param size 
 * @param start_position 
 * @return unsigned 
 */
unsigned skip_word_to_the_left(const char *buffer, const unsigned size, unsigned start_position)
{
  unsigned cursor_position = start_position;

  // caso esteja no último caractere antes de espaços pula para os espaços
  if (!is_whitespace(buffer[cursor_position]) && cursor_position > 0 && is_whitespace(buffer[cursor_position - 1]))
  {
    cursor_position -= 1;
  }

  // se estiver lidando com espaços, pula todos
  while (is_whitespace(buffer[cursor_position]) && cursor_position > 0)
  {
    cursor_position -= 1;
  }

  // se tiver parado em um caractere que não seja "white space", pula até o primeiro "white space" ou o final
  bool skipped = !is_whitespace(buffer[cursor_position]) && cursor_position > 0;
  while (!is_whitespace(buffer[cursor_position]) && cursor_position > 0)
  {
    cursor_position -= 1; 
  }

  // se tiver pulado, pode estar no primeiro caracteter "white space" antes do texto, avança se for o caso
  if (skipped && is_whitespace(buffer[cursor_position]) && cursor_position < size - 1)
  {
    cursor_position += 1;
  }

  return cursor_position;
}

/**
 * @brief Função que avança uma palavra para direita 
 * 
 * @param buffer 
 * @param size 
 * @param start_position 
 * @return unsigned 
 */
unsigned skip_word_to_the_right(const char *buffer, const unsigned size, unsigned start_position)
{
  unsigned cursor_position = start_position;

  // caso esteja no último caractere antes de espaços pula para os espaços
  if (!is_whitespace(buffer[cursor_position]) && cursor_position < size && is_whitespace(buffer[cursor_position + 1]))
  {
    cursor_position += 1;
  }

  // se estiver lidando com espaços, pula todos
  while (is_whitespace(buffer[cursor_position]) && cursor_position < size)
  {
    cursor_position += 1;
  }

  // se tiver parado em um caractere que não seja "white space", pula até o primeiro "white space" ou o final
  while (!is_whitespace(buffer[cursor_position]) && cursor_position < size)
  {
    cursor_position += 1; 
  }

  return cursor_position;
}

/**
 * @brief lida com teclas de controle de edição e outras sequencias de escape
 * @todo João, falta lidar com sequências inválidas e keys desconhecidas
 * 
 * @param buffer 
 * @param key 
 * @param cursor_position 
 * @return true 
 * @return false 
 */
static bool handle_control_key_pressed(Shell_Context_Data *context, Buffer *buffer, int key, unsigned *cursor_position)
{
  switch (key)
  {
    case ARROW_UP: {
      if (context->last_typed_commands->index > 0
        && context->next_typed_command_to_show > -2
        && ((unsigned) context->next_typed_command_to_show + 1) <  context->last_typed_commands->index)
      {
        context->next_typed_command_to_show++;
        assert(context->next_typed_command_to_show > -1);

        // @duplicado-copia
        buffer_clear(buffer);
        const char *source = context->last_typed_commands->data[context->last_typed_commands->index - 1 - context->next_typed_command_to_show];
        buffer_push_all(buffer, source, strlen(source));
        *cursor_position = buffer->index;

        return true;
      }
      else
      {
        should_emmit_ring_bell(context);
      }

    }; break;
    case ARROW_DOWN: {
      if (context->next_typed_command_to_show > 0)
      {
        context->next_typed_command_to_show--;

        // @duplicado-copia
        buffer_clear(buffer);
        const char *source = context->last_typed_commands->data[context->last_typed_commands->index - 1 - context->next_typed_command_to_show];
        buffer_push_all(buffer, source, strlen(source));
        *cursor_position = buffer->index;

        return true;
      }
      else
      {
        should_emmit_ring_bell(context);
      }
    }; break;
    case ARROW_RIGHT:
    {
      if (*cursor_position < buffer->index) (*cursor_position)++;
    }
    break;
    case ARROW_LEFT:
    {
      if (*cursor_position > 0) (*cursor_position)--;
    }
    break;
    case DELETE:
    {
      if (*cursor_position < buffer->buffer_size - 1)
      {
        buffer_pop_at(buffer, *cursor_position);
        return true;
      }
    }
    break;
    case HOME:
    {
      *cursor_position = 0;
    }
    break;
    case END:
    {
      *cursor_position = buffer->index;
    }
    break;
    case ALT_ARROW_UP:
    {
      // @note Não faz nada por hora
    } break;
    case ALT_ARROW_DOWN:
    {
      // @note Não faz nada por hora
    } break;
    case ALT_ARROW_LEFT:
    {
      unsigned new_cursor_position = skip_word_to_the_left(buffer->buffer, buffer->index, *cursor_position);

      if (new_cursor_position != *cursor_position)
      {
        *cursor_position = new_cursor_position;
        return true;
      }
    } break;
    case ALT_ARROW_RIGHT:
    {
      unsigned new_cursor_position = skip_word_to_the_right(buffer->buffer, buffer->index, *cursor_position);

      if (new_cursor_position != *cursor_position)
      {
        *cursor_position = new_cursor_position;
        return true;
      }
    } break;
    case UNKNOWN:
    {
      // @todo João, acredito que seria interessante não tratar escape sequences inválidas
      // como erro, elas não são adicionadas ao buffer de input e não serão interpretadas.
      // O que acho interessante seria analisar a hipótese de criar um file descriptor separado
      // para logar quando encontrar essas sequências inválidas. Pois o stderr já é reservado para
      // os erros reais do console. Talvez um file descriptor de debug/desenvolvimento.
      // @note Achei uma discussão interessante aqui: https://stackoverflow.com/questions/33754988/how-to-reserve-a-file-descriptor
      Debug_Log_Line("key: UNKNOWN ignorada.");
      assert(false && "Escape sequence desconhecida não tratada.");
    }
    break;
    default: assert(false && "Escape sequence inválida não tratada.");
  }

  return false;
}

/**
 * @brief Espera por um byte vindo do stdin
 * 
 * @todo João, essa não é a versão final, mas precisava dar um passo
 * para deixar esse código mais explícito e possível de alterar o stdin
 * @note VMIN e VTIME estão configurados para retornar sem esperar o usuário digitar, por isso
 * do loop.
 * @note Fiz a configuração acima para setar um timeout na função de leitura, coloquei um monte de asserts para garantir
 * que minha compreensão do fluxo estava correta, não entendia bem como essa função deveria se comportar
 * @link https://viewsourcecode.org/snaptoken/kilo/02.enteringRawMode.html#a-timeout-for-read
 * 
 * @param byte 
 */
static inline void wait_to_read_a_byte(char *byte)
{
  int readed = 0;
  do
  {
    assert(readed == 0);
    readed = read(STDIN_FILENO, byte, 1);
    if (readed == -1) assert(false && "ainda não tratei os possíveis erros");
  }
  while (readed != 1);
  assert(readed == 1);
}

// @todo João, muita complexidade e duplicidade nessa rotina, seria interessante simplificar,
// mas antes talvez cobrir com testes. Pra poder testar vou precisar abstrair os comandos "printf"
// pra escrever para file descriptors previamente setados no processo ao invés de implicitamente
// escrever pro stdout.
char *shell_wait_command_input(Shell_Context_Data *context)
{
  Buffer *buffer = create_buffer(LINE_BUFFER_SIZE, LINE_BUFFER_SIZE);
  context->next_typed_command_to_show = -1;
  char c;
  unsigned cursor_position = 0;
  /**
   * Por hora essa variável controla se preciso atrasar o update em função
   * de ter mais input no stdin para ser lido
   * @suportando-colar-texto-terminal
   */
  bool delayed_update = false;

  print_input_mark(context, NULL);
  while (true)
  {
    bool should_update_cursor = false;
    
    // @note João, por hora fica assim, a variável `next_char` não é necessária, mas adicionei pra
    // tentar deixar mais explícito esse processo.
    // @suportando-colar-texto-terminal
    if (!delayed_update) wait_to_read_a_byte(&c);
    
    Debug_Log_Line("byte read: %d : '%c'", c, c);

    delayed_update = false;

    if (c == ESC)
    {
      int key = try_process_escape_sequence();
      Debug_Log_Line("Key: %s", key_pressed_name(key))
      if (handle_control_key_pressed(context, buffer, key, &cursor_position))
      {
        erase_line();
        print_input_mark(context, buffer_ensure_null_terminated_view(buffer));
      }

      should_update_cursor = true;
    }
    else if (c == EOF || c == NEW_LINE || c == CARRIAGE_RETURN)
    {
      write(STDOUT_FILENO, EXPAND_STRING_REF_AND_COUNT("\r\n"));
      break;
    }
    else if (c == '\t')
    {
      List_Of_Strings *list = create_list_of_strings(64, 64);
      
      get_all_files_for_dir(".", list, true);
      
      quick_sort_list(list->data, 0, list->index - 1);

      if (list->index > 0)
      {
        // @note Talvez usar um buffer aqui também @speedup 
        write(STDOUT_FILENO, "\r\n", 2);
        for (unsigned i = 0; i < list->index; i++)
        {
          write(STDOUT_FILENO, list->data[i], strlen(list->data[i]));
          write(STDOUT_FILENO, ESRAC(" "));
        }

        write(STDOUT_FILENO, "\r\n", 2);
        print_input_mark(context, buffer_ensure_null_terminated_view(buffer));
      }
      
      destroy_list_of_strings(list);
    }
    else if (c == FORM_FEED) // Crtl + L
    {
      clear_terminal();
      print_input_mark(context, buffer_ensure_null_terminated_view(buffer));
      should_update_cursor = true;
    }
    else if (iscntrl(c))
    {
      if (c == BACKSPACE)
      {
        if (NO_UNDERFLOW_SUBTRACTING(cursor_position, 1) && buffer_pop_at(buffer, cursor_position - 1))
        {
          cursor_position--;
          erase_line();
          print_input_mark(context, buffer_ensure_null_terminated_view(buffer));

          should_update_cursor = true;
        }
      }
      else if (c == CTRL_KEY('a'))
      {
        cursor_position = 0;
        should_update_cursor = true;
      }
      else if (c == CTRL_KEY('e'))
      {
        cursor_position = buffer->index;
        should_update_cursor = true;
      }
      else if (c == CTRL_KEY('d'))
      {
        if (buffer->index == 0)
        {
          context->exit_requested = true;
          write(STDOUT_FILENO, "\r\n", 2);
          break;
        }
        else
        {
          should_emmit_ring_bell(context);
        }
      }
      else if (c == CTRL_KEY('c'))
      {
        // @todo João, isso acaba sobreescrevendo alguns caracteres quando é dado ctrl-c no meio do texto,
        // mas não causa nenhum bug de usabilidade apenas visual, por hora fica assim.
        write(STDOUT_FILENO, EXPAND_STRING_REF_AND_COUNT("^C\r\n"));

        buffer_clear(buffer);
        print_input_mark(context, NULL);

        cursor_position = 0;
        should_update_cursor = true;
      }
    }
    else
    {
      buffer_push_at(buffer, c, cursor_position);
      cursor_position++;
      if (cursor_position == buffer->index)
      {
        write(STDOUT_FILENO, &c, 1);
      }
      else
      {
        erase_line();
        print_input_mark(context, buffer_ensure_null_terminated_view(buffer));
      }

      // @note João, se não requisitar a atualização do cursor os comandos de 'copy' e 'paste' passam
      // a funcionar corretamente. O problema ocorre ao chamar `get_cursor_position`, provavelmente porque
      // para receber a resposta da posição do cursor o sistema escreve para o stdin, possivelmente apagando
      // o que já está no buffer.
      // Parece que vou precisar reestruturar toda a forma como leio caracteres e quando atualizo.
      // Sempre precisarei checar se tem mais um char no buffer antes de mandar o update.
      should_update_cursor = true;

      // Checa se tem mais um char imediatamente disponível, isso pode ser sinal
      // de que o usuário está colando texto no terminal
      // @suportando-colar-texto-terminal
      // @note João, acredito que esse check precisaria estar fora desse 'if', junto ao if
      // do update abaixo, mas isso causa algumas inconveniências, tipo, se o usuário segurar 
      // seta para esquerda só atualiza quando ele soltar a seta. Além de no geral ter deixado mais lento 
      // o processamento das teclas.
      delayed_update = read(STDIN_FILENO, &c, 1) == 1;
    }

    if (should_update_cursor && !delayed_update)
    {
      int row = 1, col = 1;
      // @todo João, no caso de o usuário colar texto no terminal, todos os caracteres após
      // o primeiro estão sendo lidos indevidamente pela função `get_cursor_position`
      if (get_cursor_position(&row, &col) > -1)
      {
        set_cursor_position(row, (int) cursor_position + 1 + context->input_mark_length);
      }
    }
  }

  char *result = copy(buffer_ensure_null_terminated_view(buffer));
  destroy_buffer(buffer);

  if (DEBUG_INFO) Debug_Log_Line("linha lida: '%s'", result);

  return result;
}

/**
 * @brief Função que percorre e verifica se os argumentos contém algum dos símbolos estáticos; caso encontre,
 *  realiza a substituição com a informação provida.
 * 
 * Símbolos estáticos atualmente suportados: $? e * 
 * 
 * @param execute_command_node 
 * @param last_status_code 
 */
void replace_static_symbols_with_query_info(Execute_Command_Node *execute_command_node, int last_status_code)
{
  assert(execute_command_node->args != NULL);

  // @todo João, poderia pular todo código abaixo se não houvesse símbolos pra processar,
  // não tenho certeza se a economia de tempo e processamente compensam a lógica extra.

  char **pointer_array = execute_command_node->args;
  List_Of_Strings *list_of_args = create_list_of_strings(1024, 1024);

  while (*pointer_array != NULL)
  {
    if (*pointer_array == static_query_last_status_code_symbol) 
    {
      list_of_strings_push(list_of_args, int_to_cstring(last_status_code));
      pointer_array++;
      continue;
    }

    // @note João, me parece que no que toca ao processamento de símbolos, seria interessante que o atributo
    // `args` fosse uma lista de strings ao invés de array estático.
    if (*pointer_array == static_globbing_symbol) 
    {
      List_Of_Strings *file_names = get_all_files_for_dir(".", NULL, false);

      quick_sort_list(file_names->data, 0, file_names->index - 1);

      list_of_strings_push_all(list_of_args, file_names->data, file_names->index);

      destroy_list_of_strings(file_names);

      pointer_array++;
      continue;  
    }


    list_of_strings_push(list_of_args, *pointer_array);
    pointer_array++;
  }

  FREE_AND_NULLIFY(execute_command_node->args);
  Null_Terminated_Pointer_Array args = convert_list_to_argv(list_of_args);
  destroy_list_of_strings(list_of_args);

  execute_command_node->args = args;
}

/**
 * @brief converte o `Execute_Command_Node` em um `Process_Parameter`
 * @note João, vou precisar analisar como lidar com o atributo `pipe`, desda ordem de precendência de parse
 * até, a forma como vou executar e alocar recurso, abrir arquivos de redirect, etc...
 * 
 * @param execute_command_node 
 * @param tried_opening_file_and_failed 
 * @return Process_Parameter 
 */
Process_Parameter shell_convert_execute_command_into_process_parameter(Execute_Command_Node *execute_command_node, bool *tried_opening_file_and_failed)
{
  assert(execute_command_node->args != NULL);

  Process_Parameter process = STATIC_PROCESS_PARAMETER(execute_command_node->args);
  
  if (execute_command_node->stdout_redirect_filename)
  {
    int oflags = O_RDWR|O_CREAT;
    if (execute_command_node->append_mode_stdout) oflags |= O_APPEND;
    
    int fd = open(execute_command_node->stdout_redirect_filename, oflags, 0600);
    
    if (fd == -1)
    {
      *tried_opening_file_and_failed = true;
    }
    else
    {
      process.fd_stdout = fd;
    }
  }

  if (execute_command_node->stderr_redirect_filename)
  {
    if (execute_command_node->stdout_redirect_filename && strcmp(execute_command_node->stderr_redirect_filename, execute_command_node->stdout_redirect_filename) == 0)
    {
      process.fd_stderr = process.fd_stdout;
    }
    else
    {
      int oflags = O_RDWR|O_CREAT;
      if (execute_command_node->append_mode_stderr) oflags |= O_APPEND;
      
      int fd = open(execute_command_node->stderr_redirect_filename, oflags, 0600);
      
      if (fd == -1)
      {
        *tried_opening_file_and_failed = true;
      }
      else
      {
        process.fd_stderr = fd;
      }
    }
  }

  if (execute_command_node->pipe)
  {
    // @todo João, terminar de implementar tratativa de erros, como por exemplo, "não pode abrir o arquivo" (validar)
    // Até avisa que não pode abrir, mas não consegue reportar o índice, isso porque do ponto de vista do `calle`
    // quem está sendo executado é `execute_command_node` e não `execute_command_node->pipe`, talvez fosse melhor
    // executar de uma forma mais segmentada para poder reportar melhor, ou só, retornar alguma indicação de qual
    // node estava sendo executado
    bool failed = false;

    process.pipe_through = ALLOC(Process_Parameter, 1);
    *process.pipe_through = shell_convert_execute_command_into_process_parameter(execute_command_node->pipe, &failed);

    int fd[2];
    pipe(fd);

    process.fd_stdout = fd[1];
    process.pipe_through->fd_stdin = fd[0];

    if (failed) *tried_opening_file_and_failed = true;
  }

  return process;
}

/**
 * @brief versão de debug da função `print_null_terminated_pointer_array`
 * 
 * @param pointer_array 
 * @param label 
 */
void debug_print_null_terminated_pointer_array(Null_Terminated_Pointer_Array pointer_array, const char* label)
{
  char **token = pointer_array;
  while (*token != NULL)
  {
    Debug_Log_Line("%s: '%s'", label, *token);
    token++;
  }
}

/**
 * @brief Parseia o comando, se não houver erros retorna os nós que representam a ordem de execução.
 * 
 * @note No passado cogitei retornar um ponteiro alocado o heap, para facilitar a função
 * `release_execute_command_nodes`, mas considerando que pode ser interessante fazer um solução ainda
 * mais robusta para o gerenciamento de memória (um alocador em arena que possa ser "liberado" no final do ciclo do REPL),
 * vou documentar essa ideia com essa nota, mas deixar por isso até a solução nova chegar, ou decidir mudar.
 * 
 * @param context 
 * @return Execute_Command_Node 
 */
Execute_Command_Node shell_parse_command(Parse_Context *context)
{
  // @note João, provavelmente seria interessante tokenizar sobre demanda, consumir um token e passar
  // direto para a função de análise sintática/semântica, pra evitar tokenizar tudo e depois falhar no primeiro 
  // token na próxima etapa. Mas tudo isso falta fazer, apenas anotando
  Sequence_Of_Tokens *tokens = tokenize(context);

  if (tokens->index == 0 && context->error == NULL)
  {
    context->error = copy("Nenhum comando encontrado, possivelmente por se tratar de uma linha apenas com espaços.");
  }

  if (context->error)
  {
    destroy_sequence_of_tokens(tokens);
    return STATIC_EXECUTE_COMMAND_NODE();
  }

  if (DEBUG_INFO) Debug_Log_Line("tokens size: %d", tokens->index);

  Execute_Command_Node execute_command_node = parse_execute_command_node(context, tokens);

  destroy_sequence_of_tokens(tokens);

  if (context->error)
  {
    return STATIC_EXECUTE_COMMAND_NODE();
  }
  
  if (DEBUG_INFO)
  {
    // @todo João, só print os argumentos do primeiro comando
    debug_print_null_terminated_pointer_array(execute_command_node.args, "Argumento extraído");
  }

  return execute_command_node;
}

int shell_execute_command(const Process_Parameter process_parameter)
{
  char **args = process_parameter.args;

  if (args[0] == NULL)
  {
    // @note De fato acontece?
    assert(false && "Não deveria ser atingido nunca");
    return 1;
  }

  // @todo João, investigar se não tem alguma forma de distorção de output nesse caso
  // já que não estou reativando a flag opost aqui. Me parece que por emitirmos os
  // caracteres manualmente tudo já está funcionando corretamente.
  Builtin_Function builtin_func = has_builtin_for(args[0]);
  if (builtin_func)
  {
    int result = builtin_func(&process_parameter);

    if (process_parameter.pipe_through)
    {
      close(process_parameter.fd_stdout);
      // @todo João, aqui temos um problema pois estou ignorando um erro potencial.
      // Além do que, esse código está de certa forma duplicado e incompleto, pois, se o 
      // comando builtin aparecer no meio de uma pipe complexa, apresentará mal funcionamento.
      shell_execute_command(*process_parameter.pipe_through);
    }
    return result;
  }

  // @todo João, não necessariamente aqui, mas seria importante validar se o comando será encontrado
  // antes de tentar executá-lo, seria mais rápido do que fazer o fork para falhar e seria mais fácil
  // apresentar mensagens úteis.
  // @note Não tenho mais certeza se seria de fato melhor checar, considerando que aparentemente não há
  // formar padrão de fazer isso. @reference https://stackoverflow.com/questions/890894/portable-way-to-find-out-if-a-command-exists-c-c
  // Por hora seria melhor apenas melhorar a mensagem de retorno (parcialmente melhorado)
  return launch_process(process_parameter, true);
}

void shell_report_error(Shell_Context_Data *shell_context, const char*error, signed error_start_index)
{
  // RECORD_TIME(report_error);
  Buffer *buffer = create_buffer(LINE_BUFFER_SIZE, LINE_BUFFER_SIZE);

  if (error_start_index > -1)
  {
    // @note verificar uma forma mais eficiente de imprimir várias vezes o mesmo caracter,
    // isso acontece em vários lugares
    for (unsigned i = 0; i < shell_context->input_mark_length; i++)
    {
      buffer_push(buffer, ' ');
    }
    
    for (signed i = 0; i < error_start_index+1; i++)
    {
      if (i == error_start_index)
      {
        buffer_push(buffer, '^');
      }
      else
      {
        buffer_push(buffer, '-');
      }
    }
    buffer_push_all(buffer, "\r\n", 2);
  }

  for (unsigned i = 0; i < shell_context->input_mark_length; i++)
  {
    buffer_push(buffer, ' ');
  }
  
  buffer_push_all(buffer, EXPAND_STRING_REF_AND_COUNT("Problema: "));
  buffer_push_all(buffer, error, strlen(error));
  buffer_push_all(buffer, "\r\n", 2);

  // @todo João, passar o contexto aqui, pra pegar o stdout configurado
  write(STDOUT_FILENO, buffer->buffer, buffer->index);
  
  destroy_buffer(buffer);
  // MEASURE_TIME(report_error, "teste");
}

void shell_report_parse_error(Shell_Context_Data *shell_context, Parse_Context *context)
{
  shell_report_error(shell_context, context->error, context->error_start_index);
}

void read_eval_shell_loop(bool colorful, bool no_sound)
{
  Shell_Context_Data shell_context = create_shell_context_data();
  the_shell_context = &shell_context;

  shell_context.colorful = colorful;
  shell_context.soundful = !no_sound;

  while (!shell_context.exit_requested)
  {
    char *readed_line = shell_wait_command_input(&shell_context);

    // não processa inputs vazios, também não adiciona no histórico
    if (is_only_spaces_or_empty(readed_line)) continue;

    if (ends_with_char(readed_line, '\\'))
    {
      // @todo João, terminar de implementar o suporte a comandos de multiplas linhas 
      shell_report_error(&shell_context, "Comandos de multiplas linhas não suportados ainda.", strlen(readed_line) - 1);
      continue;
    }

    Parse_Context context = create_parse_context(readed_line);
    Execute_Command_Node execute_command_node = shell_parse_command(&context);

    if (context.error == NULL)
    {
      Execute_Command_Node *current_command = &execute_command_node;
      unsigned command_counter = 0;
      bool should_interrupt = false;
      while (current_command && !should_interrupt)
      {
        bool tried_opening_file_and_failed = false;

        // @note Aqui antes de executar o comando eu faço a substituição do $? pelo status, porque depende
        // do resultado da execução do comando anterior
        replace_static_symbols_with_query_info(current_command, shell_context.last_status_code);

        Process_Parameter process_parameter = shell_convert_execute_command_into_process_parameter(current_command, &tried_opening_file_and_failed);
        if (tried_opening_file_and_failed)
        {
          if (current_command->next_command) context.error = "O arquivo não pôde ser aberto. Parando execução em sequência.";
          else                               context.error = "O arquivo não pôde ser aberto.";

          context.error_start_index = current_command->token_index_start;
          
          // @todo João, me parece estranho esse código parar aqui, mas é necessário para fazer o report alinhado ao input 
          write(STDOUT_FILENO, "\r\n", 2);
          print_input_mark(&shell_context, readed_line);
          write(STDOUT_FILENO, "\r\n", 2);

          shell_report_parse_error(&shell_context, &context);
          should_interrupt = true;
        }
        else
        {
          int result = shell_execute_command(process_parameter);
          shell_context.last_status_code = result;

          if (result)
          {
            should_interrupt = true;
          }
          else
          {
            command_counter++;
          }
        }

        // @note João, reportar em qual comando o problema ocorreu? adicionar parâmetro para suprimir isso
        // o bash não faz isso, não sei se acho interessante adicionar output

        current_command = current_command->next_command;

        release_process_parameters(&process_parameter, false, false);
      }
    }
    else if (shell_context.exit_requested)
    {
      write(STDOUT_FILENO, EXPAND_STRING_REF_AND_COUNT("saindo, até mais!!!"));
      FREE_AND_NULLIFY(context.error);
    }
    else // @note se `shell_context.exit_requested` for false, no momento ele reporta o erro por tentar parsear input vazio
    {
      shell_report_parse_error(&shell_context, &context);
      FREE_AND_NULLIFY(context.error);
    }

    // Faz o release dos nós e dos args, a estrutura `Process_Parameter` pega emprestada a referência
    // mas a essa altura da execução ela já não está mais usando. O if e o loop acima podem ser abortados
    // prematuramente, por isso o release fica pra depois de terminar toda execução.
    release_execute_command_nodes(&execute_command_node, false);

    // @todo João, por hora vou copiar a string, mas poderia muito bem pegar ela emprestada, pois esse é o ponto aonde ela não é mais necessária
    list_of_strings_push(shell_context.last_typed_commands, copy(readed_line));
    FREE_AND_NULLIFY(readed_line);

    if (shell_context.last_typed_commands->index > HISTORY_MAX_ELEMENTS)
    {
      FREE_AND_NULLIFY(shell_context.last_typed_commands->data[0]);
      list_of_strings_pop_at(shell_context.last_typed_commands, 0);
    }
  }
}

#endif // _SHELL_C_
