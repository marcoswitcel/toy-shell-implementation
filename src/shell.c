#ifndef _SHELL_C_
#define _SHELL_C_

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "./compilation_definitions.h"
#include "./parser.h"
#include "./process_manager.c"
#include "./types.h"
#include "./shell_builtins.c"

#define LINE_BUFFER_SIZE 1024

static inline void print_input_mark(const char *cstring)
{
  if (cstring)
  {
    printf("|>%s", cstring); // @note organizar reimpressão da marcação inicial
    return;
  }

  printf("|>");
}

char *shell_wait_command_input(void)
{
  Buffer *buffer = create_buffer(LINE_BUFFER_SIZE, LINE_BUFFER_SIZE); // @note aqui para testar
  int c;

  print_input_mark(NULL);
  while (true)
  {
    c = getchar();

    if (c == EOF || c == '\n')
    {
      printf("\n");// @note deveria ser bufferizado do meu lado? por hora não está em "raw_mode" ainda, mas vai ficar.
      char *result = copy(buffer_ensure_null_terminated_view(buffer));

      if (DEBUG_INFO) printf("linha lida: [%s]\n", result); // @note apenas para debugar

      destroy_buffer(buffer);
      return result;
    }
    else if (c == '\t')
    {
      List_Of_Strings *list = create_list_of_strings(64, 64);
      
      get_all_files_for_dir(".", list, true);
      if (list->index > 0)
      {
        printf("\n");
        for (unsigned i = 0; i < list->index; i++)
        {
          printf("%s ", list->data[i]);
        }
        printf("\n");
        print_input_mark(buffer_ensure_null_terminated_view(buffer)); // @note organizar reimpressão da marcação inicial
      }
      
      destroy_list_of_strings(list);
    }
    else if (c == FORM_FEED) // @note Crtl + L
    {
      clear_terminal();
      print_input_mark(buffer_ensure_null_terminated_view(buffer)); // @note organizar reimpressão da marcação inicial
    }
    else if (iscntrl(c))
    {
      if (c == BACKSPACE)
      {
        if (buffer_pop(buffer))
        {
          clear_terminal();
          print_input_mark(buffer_ensure_null_terminated_view(buffer)); // @note organizar reimpressão da marcação inicial
        }
      };
    }
    else
    {
      printf("%c", c);// @note deveria ser bufferizado do meu lado? por hora não está em "raw_mode" ainda, mas vai ficar.
      buffer_push(buffer, c); // @todo João, testar
    }
  }
}

Process_Parameter shell_parse_command(Parse_Context *context)
{
  // @note João, provavelemente seria interessante tokenizar sobre demanda, consumir um token e passar
  // direto para a função de análise sintática/semântica, pra evitar tokenizar tudo e depois falhar no primeiro 
  // token na próxima etapa. Mas tudo isso falta fazer, apenas anotando
  Sequence_Of_Tokens *tokens = tokenize(context);

  if (context->error)
  {
    return STATIC_PROCESS_PARAMETER(NULL);
  }

  if (DEBUG_INFO) printf("[[ tokens size: %d ]]\n", tokens->index);

  Execute_Command_Node execute_command_node = parse_execute_command_node(context, tokens);

  destroy_sequence_of_tokens(tokens);

  if (context->error)
  {
    return STATIC_PROCESS_PARAMETER(NULL);
  }
  
  // @note o bloco abaixo é apenas para visualizar o resultado
  if (DEBUG_INFO)
  {
    print_null_terminated_pointer_array(execute_command_node.args, "Argumento extraído");
  }

  Process_Parameter process = STATIC_PROCESS_PARAMETER(execute_command_node.args);
  if (execute_command_node.output_filename)
  {
    int oflags = O_RDWR|O_CREAT;
    if (execute_command_node.append_mode) oflags |= O_APPEND;
    
    int fd = open(execute_command_node.output_filename, oflags, 0600);
    if (fd == -1)
    {
      printf("Internal: Erro abrindo arquivo '%s'", execute_command_node.output_filename);
    }
    process.fd_stdout = fd;
  }
  return process;
}

int shell_execute_command(const Process_Parameter process_parameter)
{
  char **args = process_parameter.args;

  if (args[0] == NULL)
  {
    // @note De fato acontece?
    return 1; // comando vazio
  }

  Builtin_Function builtin_func = has_builtin_for(args[0]);
  if (builtin_func)
  {
    Process_Handles handles = STATIC_PROCESS_HANDLES();
    if (process_parameter.fd_stdout != -1)
    {
      handles.stdout = fdopen(process_parameter.fd_stdout, "w");
      int result = builtin_func(args, &handles);
      fclose(handles.stdout);
      return result;
    }
    return builtin_func(args, &handles);
  }

  return launch_process(process_parameter);
}

void shell_report_parse_error(Parse_Context *context)
{
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

// @note Não tenho certeza de nomes, nem de estrutura ainda, mas vamos ver como flui.
void read_eval_shell_loop()
{
  while (!exit_requested)
  {
    char *readed_line = shell_wait_command_input();
    Parse_Context context = create_parse_context(readed_line);
    Process_Parameter process_parameter = shell_parse_command(&context);
    // @note já consegue iniciar processos, por hora precisam ser com o caminho absoluto "/usr/bin/ls"
    // @note só precisava mudar para `execvp` para ele aceitar ls sem o caminho completo
    if (context.error == NULL)
    {
      shell_execute_command(process_parameter);
    }
    else
    {
      shell_report_parse_error(&context);
      FREE_AND_NULLIFY(context.error);
    }

    FREE_AND_NULLIFY(readed_line);
    if (process_parameter.args != NULL)
    {
      release_cstring_from_null_terminated_pointer_array(process_parameter.args);
      FREE_AND_NULLIFY(process_parameter.args);
    }
  }
}

#endif // _SHELL_C_
