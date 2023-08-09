#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <termios.h>
#include <ctype.h>
#include <dirent.h>

// Desenvolvimento
#define DEBUG_INFO false

#include "./list.implementations.h"
#include "./process_manager.c"
#include "./utils.c"
#include "./types.h"
#include "./tokens.h"
#include "./parser.h"
#include "./builtins.c"


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
      builtin_clear(NULL);
      print_input_mark(buffer_ensure_null_terminated_view(buffer)); // @note organizar reimpressão da marcação inicial
    }
    else if (iscntrl(c))
    {
      if (c == BACKSPACE)
      {
        if (buffer_pop(buffer))
        {
          builtin_clear(NULL);
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
    char **token = execute_command_node.args;
    while (*token != NULL)
    {
      printf("argumento extraído: [%s]\n", *token);
      token++;
    }
  }

  Process_Parameter process = STATIC_PROCESS_PARAMETER(execute_command_node.args);
  if (execute_command_node.output_filename)
  {
    int fd = open(execute_command_node.output_filename, O_RDWR|O_CREAT, 0600);
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
    return builtin_func(args);
  }

  return launch_process(process_parameter);
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
      if (context.error_start_index > -1)
      {
        printf("  ");
        for (signed i = 0; i < context.error_start_index+1; i++)
        {
          if (i == context.error_start_index)
          {
            printf("^");
          }
          else
          {
            printf("-");
          }
        }
        printf("\n  Descrição: %s\n", context.error);
      }
      else
      {
        printf("Erro ao executar comando:\n%s\n", context.error);
      }
      free((void *) context.error);
    }

    if (readed_line)
    {
      free(readed_line);
    }
    if (process_parameter.args)
    {
      free(process_parameter.args);
      process_parameter.args = NULL;
    }
  }
}

static struct termios original_config;

void deactivate_raw_mode()
{
  // @todo João, analisar aqui, está disparando mais de uma vez aparentemente, mas acho que é nos processos filhos
  // verificar se isso pode causar problemas.
  if (DEBUG_INFO) printf("[[ deactivate_raw_mode ]] :: restaurando configurações de terminal.\n");
  tcsetattr(STDERR_FILENO, TCSAFLUSH, &original_config);
}

void activate_raw_mode()
{
  tcgetattr(STDIN_FILENO, &original_config);
  atexit(deactivate_raw_mode);

  struct termios new_config = original_config;

  new_config.c_lflag &= ~(ECHO | ICANON); // sem echo e buffer de saída
  tcsetattr(STDERR_FILENO, TCSAFLUSH, &new_config);
}

int main(void)
{
  activate_raw_mode();
  // @note Aceitar argumentos pela linha de comando? que argumentos

  // @note talvez ler um arquivo de configuração? mas que configurações aceitar?

  // Read Eval Loop
  read_eval_shell_loop();

  // @note Cleanup se tiver algum para fazer

  return EXIT_SUCCESS;
}
