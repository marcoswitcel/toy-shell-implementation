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
#include "./dev-utils.c"
#include "./utils.c"
#include "./sorting.c"

#define HISTORY_MAX_ELEMENTS 20

typedef struct Shell_Context_Data
{
  bool colorful;
  List_Of_Strings *last_typed_commands;
  int next_typed_command_to_show;
  int last_status_code;
} Shell_Context_Data;

Shell_Context_Data create_shell_context_data()
{
  return (Shell_Context_Data) {
    .colorful = false,
    .last_typed_commands = create_list_of_strings(64, 64),
    .next_typed_command_to_show = -1,
    .last_status_code = 0,
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
        emmit_ring_bell();
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
        emmit_ring_bell();
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
  context->next_typed_command_to_show = -1;
  char c;
  unsigned cursor_position = 0;

  print_input_mark(context, NULL);
  while (true)
  {
    bool should_update_cursor = false;
    
    // @todo João, essa não é a versão final, mas precisava dar um passo
    // para deixar esse código mais explícito e possível de alterar o stdin
    // @note Fiz a configuração acima para setar um timeout na função de leitura, coloquei um monte de asserts para garantir
    // que minha compreensão do fluxo estava correta, não entendia bem como essa função deveria se comportar
    int readed = 0;
    do
    {
      assert(readed == 0);
      readed = read(STDIN_FILENO, &c, 1);
      if (readed == -1) assert(false && "ainda não tratei os possíveis erros");
    }
    while (readed != 1);
    assert(readed == 1);

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
    else if (c == EOF || c == NEW_LINE || c == CARRIAGE_RETURN)
    {
      write(STDOUT_FILENO, "\r\n", 2);
      // @note decidi mover a lógica de cópia, release de memória e retorno pra fora do loop, parece mais legível
      break;
    }
    else if (c == '\t')
    {
      List_Of_Strings *list = create_list_of_strings(64, 64);
      
      get_all_files_for_dir(".", list, true);
      
      quick_sort_list(list->data, 0, list->index - 1);

      if (list->index > 0)
      {
        write(STDOUT_FILENO, "\r\n", 2);
        for (unsigned i = 0; i < list->index; i++)
        {
          printf("%s ", list->data[i]);
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
          exit_requested = true;
          write(STDOUT_FILENO, "\r\n", 2);
          break;
        }
        else
        {
          emmit_ring_bell();
        }
      }
      else if (c == CTRL_KEY('c'))
      {
        // @todo João, isso acaba sobreescrevendo alguns caracteres quando é dado ctrl-c no meio do texto,
        // mas não causa nenhum bug, é só visual, por hora fica assim.
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

  char *result = copy(buffer_ensure_null_terminated_view(buffer));
  destroy_buffer(buffer);

  if (DEBUG_INFO) printf("linha lida: [%s]\r\n", result);

  return result;
}

/**
 * @brief Função que percorre os argumentos e verifica se os argumentos contém algum dos símbolos de query e realiza
 * a substituição com a informação provida.
 * 
 * Símbolo de query atualmente suportados: $?
 * 
 * @param execute_command_node 
 * @param last_status_code 
 */
void replace_static_symbols_with_query_info(Execute_Command_Node *execute_command_node, int last_status_code)
{
  assert(execute_command_node->args != NULL);

  char **pointer_array = execute_command_node->args;

  while (*pointer_array != NULL)
  {
    if (*pointer_array == static_query_last_status_code_symbol) 
    {
      *pointer_array = int_to_cstring(last_status_code);
    }

    pointer_array++;
  }
}

Process_Parameter shell_convert_execute_command_into_process_paramater(Execute_Command_Node *execute_command_node, bool *tried_opening_file_and_failed)
{
  assert(execute_command_node->args != NULL);

  Process_Parameter process = STATIC_PROCESS_PARAMETER(execute_command_node->args);
  if (execute_command_node->output_filename)
  {
    int oflags = O_RDWR|O_CREAT;
    if (execute_command_node->append_mode) oflags |= O_APPEND;
    
    int fd = open(execute_command_node->output_filename, oflags, 0600);
    
    if (fd == -1)
    {
      *tried_opening_file_and_failed = true;
    }
    else
    {
      if (execute_command_node->fd == 2)
      {
        process.fd_stderr = fd;
      }
      else
      {
        process.fd_stdout = fd;
      }
    }
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
  }

  if (context->error)
  {
    destroy_sequence_of_tokens(tokens);
    return STATIC_EXECUTE_COMMAND_NODE();
  }

  if (DEBUG_INFO) printf("[[ tokens size: %d ]]\r\n", tokens->index);

  Execute_Command_Node execute_command_node = parse_execute_command_node(context, 0, tokens);

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

  // @todo João, investigar se não alguma forma de distorção do output nesse caso
  // já que não estou reativando a flag opost aqui. Me parece que por emitirmos os
  // caracteres manualmente tudo já está funcionando corretamente.
  Builtin_Function builtin_func = has_builtin_for(args[0]);
  if (builtin_func)
  {    
    return builtin_func(&process_parameter);
  }

  // @todo João, não necessariamente aqui, mas seria importante validar se o comando será encontrado
  // antes de tentar executá-lo, seria mais rápido do que fazer o fork para falhar e seria mais fácil
  // apresentar mensagens úteis.
  // @note Não tenho mais certeza se seria de fato melhor checar, considerando que aparentemente não há
  // formar padrão de fazer isso. @reference https://stackoverflow.com/questions/890894/portable-way-to-find-out-if-a-command-exists-c-c
  // Por hora seria melhor apenas melhorar a mensagem de retorno (parcialmente melhorado)
  return launch_process(process_parameter, true);
}

void shell_report_error(const char*error, signed error_start_index)
{
  // RECORD_TIME(report_error);
  Buffer *buffer = create_buffer(LINE_BUFFER_SIZE, LINE_BUFFER_SIZE);

  if (error_start_index > -1)
  {
    buffer_push_all(buffer, EXPAND_STRING_REF_AND_COUNT("  "));
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

  buffer_push_all(buffer, EXPAND_STRING_REF_AND_COUNT("  Problema: "));
  buffer_push_all(buffer, error, strlen(error));
  buffer_push_all(buffer, "\r\n", 2);

  // @todo João, passar o contexto aqui, pra pegar o stdout configurado
  write(1, buffer->buffer, buffer->index);
  
  destroy_buffer(buffer);
  // MEASURE_TIME(report_error, "teste");
}

void shell_report_parse_error(Parse_Context *context)
{
  shell_report_error(context->error, context->error_start_index);
}

void read_eval_shell_loop(bool colorful)
{
  Shell_Context_Data shell_context = create_shell_context_data();
  shell_context.colorful = colorful;

  last_typed_commands = shell_context.last_typed_commands; // @todo João, assim não é a forma mais elegante, porém, temporariamente fica assim

  while (!exit_requested)
  {
    char *readed_line = shell_wait_command_input(&shell_context);
    Parse_Context context = create_parse_context(readed_line);
    /**
     * @todo João, deveria chamar um método free nessa estrutura, pois ela pode conter 
     * referências para outras estruturas alocadas no heap. Mas para isso é preciso criar esse método
     * e talvez mudar a função shell_parse_command para retornar uma referência.
     * @leak @fixme
     */
    Execute_Command_Node execute_command_node = shell_parse_command(&context);

    if (context.error == NULL)
    {
      // @todo João, necessário fazer o free dos comandos executados
      Execute_Command_Node *current_command = &execute_command_node;
      unsigned command_counter = 0;
      bool should_interrupt = false;
      while (current_command && !should_interrupt)
      {
        bool tried_opening_file_and_failed = false;

        // @Note Aqui antes de executar o comando eu faço a substituiçãodo $? pelo status, porque depende
        // do resultado da execução do comando anterior
        replace_static_symbols_with_query_info(current_command, shell_context.last_status_code);
        Process_Parameter process_parameter = shell_convert_execute_command_into_process_paramater(current_command, &tried_opening_file_and_failed);
        if (tried_opening_file_and_failed)
        {
          if (current_command->next_command) context.error = "O arquivo não pôde ser aberto. Parando execução em sequência.";
          else                               context.error = "O arquivo não pôde ser aberto.";

          context.error_start_index = current_command->token_index_start;
          
          // @todo João, me parece estranho esse código parar aqui, mas é necessário para fazer o report alinhado ao input 
          write(STDOUT_FILENO, "\r\n", 2);
          print_input_mark(&shell_context, readed_line);
          write(STDOUT_FILENO, "\r\n", 2);

          shell_report_parse_error(&context);
          should_interrupt = true;
        }
        else
        {
          // @todo João, é necessário checar se o comando executou corretamente, acredito que o método que inicia o processo filho
          // não está retornando o status do mesmo. É necessário fazer o ajuste na função `launch_process`
          int result = shell_execute_command(process_parameter);
          shell_context.last_status_code = result;

          // @todo João, agora que desativei a conversão de \n pra \r\n com o OPOST os processos filhos não estão
          // printando corretamente. A linha abaixo corrige a falta de carriage return, mas não corrige se não houve enter,
          // neste caso inclusive causa falhas na apresentação do output. Talvez resetar o ouput processing do processo filho
          write(STDOUT_FILENO, "\r", 1);

          if (result)
          {
            should_interrupt = true;
          }
          else
          {
            command_counter++;
          }
        }

        current_command = current_command->next_command;
      }
    }
    else if (exit_requested)
    {
      write(STDOUT_FILENO, EXPAND_STRING_REF_AND_COUNT("saindo, até mais!!!"));
      FREE_AND_NULLIFY(context.error);
    }
    else // @note se exit_requested for false, no momento ele reporta o erro pro tentar parsear input vazio
    {
      shell_report_parse_error(&context);
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
