#ifndef _SHELL_C_
#define _SHELL_C_

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#include "./compilation_definitions.h"
#include "./list.implementations.h"
#include "./parser.h"
#include "./process_manager.c"
#include "./types.h"
#include "./nodes.h"
#include "./shell_builtins.c"
#include "./terminal.c"
#include "./utils.macro.h"

typedef struct Shell_Context_Data
{
  bool colorful;
  List_Of_Strings *last_typed_commands;
} Shell_Context_Data;

Shell_Context_Data create_shell_context_data()
{
  return (Shell_Context_Data) {
    .colorful = false,
    .last_typed_commands = create_list_of_strings(64, 64),
  };
}

#define LINE_BUFFER_SIZE 1024

static inline void print_input_mark(Shell_Context_Data *context, const char *cstring)
{
  // @note lindando com a bufferização por enquanto
  // @reference https://stackoverflow.com/questions/41120879/mixing-syscall-write-with-printf-on-linux
  // fflush(stdout); // @note esta linha e a abaixo resolvem o problema de bufferização
  setbuf(stdout, NULL);

  // @note https://stackoverflow.com/questions/3585846/color-text-in-terminal-applications-in-unix
  static const char green[] = "\x1B[32m";
  static const char reset[] = "\x1B[0m";

  if (context->colorful) write(STDOUT_FILENO, green, SIZE_OF_STATIC_STRING(green));

  write(STDOUT_FILENO, "|>", 2);

  if (context->colorful) write(STDOUT_FILENO, reset, SIZE_OF_STATIC_STRING(reset));

  if (cstring)
  {
    write(STDOUT_FILENO, cstring, strlen(cstring));
  }
}

typedef enum Key_Pressed {
  ARROW_UP,
  ARROW_DOWN,
  ARROW_LEFT,
  ARROW_RIGHT,
  DELETE,
  HOME,
  END,
  UNKNOWN,
} Key_Pressed;

/**
 * @brief converte sequências de escape em um representação interna para a sequência
 * @reference https://viewsourcecode.org/snaptoken/kilo/03.rawInputAndOutput.html#the-home-and-end-keys
 * 
 * @return Key_Pressed 
 */
static Key_Pressed try_process_escape_sequence()
{
  int c = getchar();
  if (c == '[')
  {
    c = getchar();
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
        if (c == '~') return HOME;
      } break;
      case '3':
      {
        c = getchar();
        if (c == '~') return DELETE;
      } break;
      case '4':
      {
        c = getchar();
        if (c == '~') return END;
      } break;
      case '7':
      {
        c = getchar();
        if (c == '~') return HOME;
      } break;
      case '8':
      {
        c = getchar();
        if (c == '~') return END;
      } break;
    }

    return UNKNOWN;
  }
  else if (c == 'O')
  {
    c = getchar();
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
      if (context->last_typed_commands->index > 0)
      {
        buffer_clear(buffer);
        const char *source = context->last_typed_commands->data[context->last_typed_commands->index - 1];
        buffer_push_all(buffer, source, strlen(source));
        *cursor_position = buffer->index;

        return true;
      }
      else
      {
        emmit_ring_bell();
      }

    }; break;
    case ARROW_DOWN: emmit_ring_bell(); break;
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
    case UNKNOWN:
    {
      assert(false && "Escape sequence desconhecida não tratada.");
    }
    break;
    default: assert(false && "Escape sequence inválida não tratada.");
  }

  return false;
}

// @todo João, muita complexidade e duplicidade nessa rotina, seria interessante simplificar,
// mas antes talvez cobrir com testes. Pra poder testar vou precisar abstrair os comandos "printf"
// pra escrever pra file descriptors previamente setados no processo ao invés de implicitamente
// escrever pro stdout.
char *shell_wait_command_input(Shell_Context_Data *context)
{
  Buffer *buffer = create_buffer(LINE_BUFFER_SIZE, LINE_BUFFER_SIZE);
  char c;
  unsigned cursor_position = 0;

  print_input_mark(context, NULL);
  while (true)
  {
    bool should_update_cursor = false;
    
    // @todo João, essa não é a versão final, mas precisava dar um passo
    // para deixar esse código mais explícito e possível de alterar o stdin
    if (read(STDIN_FILENO, &c, 1) != 1)
    {
      c = EOF;
    }

    if (c == ESC)
    {
      int key = try_process_escape_sequence();
      if (handle_control_key_pressed(context, buffer, key, &cursor_position))
      {
        erase_line();
        print_input_mark(context, buffer_ensure_null_terminated_view(buffer));
      }

      should_update_cursor = true;
    }
    else if (c == EOF || c == '\n')
    {
      write(STDOUT_FILENO, "\n", 1);
      char *result = copy(buffer_ensure_null_terminated_view(buffer));

      if (DEBUG_INFO) printf("linha lida: [%s]\n", result);

      destroy_buffer(buffer);
      return result;
    }
    else if (c == '\t')
    {
      List_Of_Strings *list = create_list_of_strings(64, 64);
      
      get_all_files_for_dir(".", list, true);
      if (list->index > 0)
      {
        write(STDOUT_FILENO, "\n", 1);
        for (unsigned i = 0; i < list->index; i++)
        {
          printf("%s ", list->data[i]);
        }
        write(STDOUT_FILENO, "\n", 1);
        print_input_mark(context, buffer_ensure_null_terminated_view(buffer));
      }
      
      destroy_list_of_strings(list);
    }
    else if (c == FORM_FEED) // Crtl + L
    {
      clear_terminal();
      print_input_mark(context, buffer_ensure_null_terminated_view(buffer));
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
      };
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

      should_update_cursor = true;
    }

    if (should_update_cursor)
    {
      int row = 1, col = 1;
      if (get_cursor_position(&row, &col) > -1)
      {
        set_cursor_position(row, (int) cursor_position + 3);
      }
    }
  }
}

Process_Parameter shell_convert_execute_command_into_process_paramater(Execute_Command_Node *execute_command_node)
{
  assert(execute_command_node->args != NULL);

  Process_Parameter process = STATIC_PROCESS_PARAMETER(execute_command_node->args);
  if (execute_command_node->output_filename)
  {
    int oflags = O_RDWR|O_CREAT;
    if (execute_command_node->append_mode) oflags |= O_APPEND;
    
    int fd = open(execute_command_node->output_filename, oflags, 0600);
    // @todo João, na verdade deveria avisar o usuário e não rodar o comando nesse caso.
    if (fd == -1)
    {
      printf("Internal: Erro abrindo arquivo '%s'", execute_command_node->output_filename);
    }
    process.fd_stdout = fd;
  }

  return process;
}

Execute_Command_Node shell_parse_command(Parse_Context *context)
{
  // @note João, provavelemente seria interessante tokenizar sobre demanda, consumir um token e passar
  // direto para a função de análise sintática/semântica, pra evitar tokenizar tudo e depois falhar no primeiro 
  // token na próxima etapa. Mas tudo isso falta fazer, apenas anotando
  Sequence_Of_Tokens *tokens = tokenize(context);

  if (tokens->index == 0)
  {
    context->error = copy("Nenhum comando encontrado, possivelmente por se tratar de uma linha apenas com espaços.");
    return STATIC_EXECUTE_COMMAND_NODE();
  }

  if (context->error)
  {
    return STATIC_EXECUTE_COMMAND_NODE();
  }

  if (DEBUG_INFO) printf("[[ tokens size: %d ]]\n", tokens->index);

  Execute_Command_Node execute_command_node = parse_execute_command_node(context, tokens);

  destroy_sequence_of_tokens(tokens);

  if (context->error)
  {
    return STATIC_EXECUTE_COMMAND_NODE();
  }
  
  // @note o bloco abaixo é apenas para visualizar o resultado
  if (DEBUG_INFO)
  {
    print_null_terminated_pointer_array(execute_command_node.args, "Argumento extraído");
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

  Builtin_Function builtin_func = has_builtin_for(args[0]);
  if (builtin_func)
  {
    Process_Handles handles = STATIC_PROCESS_HANDLES();
    if (process_parameter.fd_stdout != -1)
    {
      handles.stdout = process_parameter.fd_stdout;
      int result = builtin_func(args, &handles);
      return result;
    }
    return builtin_func(args, &handles);
  }

  return launch_process(process_parameter);
}

void shell_report_parse_error(Parse_Context *context)
{
  // @todo joão, o ideal seria se livrar desses printf, talvez usar o write direto, porém,
  // criando um buffer e fazendo um write apenas seria mais eficiente
  if (context->error_start_index > -1)
  {
    printf("  ");
    for (signed i = 0; i < context->error_start_index+1; i++)
    {
      if (i == context->error_start_index)
      {
        printf("^");
      }
      else
      {
        printf("-");
      }
    }
    printf("\n  Descrição: %s\n", context->error);
  }
  else
  {
    printf("Erro ao executar comando:\n%s\n", context->error);
  }
}

void read_eval_shell_loop(bool colorful)
{
  Shell_Context_Data shell_context = create_shell_context_data();
  shell_context.colorful = colorful;

  while (!exit_requested)
  {
    char *readed_line = shell_wait_command_input(&shell_context);
    Parse_Context context = create_parse_context(readed_line);
    Execute_Command_Node execute_command_node = shell_parse_command(&context);
    Process_Parameter process_parameter = STATIC_PROCESS_PARAMETER(NULL);

    if (context.error == NULL)
    {
      process_parameter = shell_convert_execute_command_into_process_paramater(&execute_command_node);
      shell_execute_command(process_parameter);
    }
    else
    {
      shell_report_parse_error(&context);
      FREE_AND_NULLIFY(context.error);
    }

    // @todo João, por hora vou copiar a string, mas poderia muito bem pegar ela emprestada, pois esse é o ponto aonde ela não é mais necessária
    // @todo João, indiferente de como eu implementar será necessário limpar as strings contidas nessa lista eventualmente
    list_of_strings_push(shell_context.last_typed_commands, copy(readed_line));

    FREE_AND_NULLIFY(readed_line);
    if (process_parameter.args != NULL)
    {
      release_cstring_from_null_terminated_pointer_array(process_parameter.args);
      FREE_AND_NULLIFY(process_parameter.args);
    }
  }
}

#endif // _SHELL_C_
