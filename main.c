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

#include "./types.h"
#include "./builtins.c"

// Desenvolvimento
#define DEBUG_INFO false

#define LINE_BUFFER_SIZE 1024

int shell_launch_process(char **args);

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

#define SHELL_SPLIT_COMMAND_BUFFER_SIZE 64
#define SHELL_SPLIT_COMMAND_TOKEN_DELIMITER " \t\r\n\a"

char **shell_split_command_into_args(char *commands)
{
  int buffer_size = SHELL_SPLIT_COMMAND_BUFFER_SIZE;
  int position = 0;
  char **tokens = malloc(buffer_size * sizeof(char *));
  char *token;

  if (!tokens)
  {
    fprintf(stderr, "Internal: Erro de alocação");
    exit(EXIT_FAILURE);
  }

  /**
   * @note strtok retorna ponteiros para dentro da string provida, ele altera a string colocando '\0' aonde encontrar o delimitador provido.
   */
  token = strtok(commands, SHELL_SPLIT_COMMAND_TOKEN_DELIMITER);
  while (token != NULL)
  {
    tokens[position] = token;
    position++;

    if (position >= buffer_size)
    {
      buffer_size += SHELL_SPLIT_COMMAND_BUFFER_SIZE;
      tokens = realloc(tokens, buffer_size * sizeof(char *));
      if (!tokens)
      {
        fprintf(stderr, "Internal: Erro de alocação");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, SHELL_SPLIT_COMMAND_TOKEN_DELIMITER);
  }

  tokens[position] = NULL;
  // @note o bloco abaixo é apenas para visualizar o resultado
  if (DEBUG_INFO)
  {
    char **token = tokens;
    while (*token != NULL)
    {
      printf("argumento extraído: [%s]\n", *token);
      token++;
    }
  }
  return tokens;
}

int shell_launch_process(char **args)
{
  pid_t pid;
  int status;

  pid = fork();

  if (pid == 0)
  {
    // processo filho
    if (execvp(args[0], args) == -1)
    {
      perror("Internal: Processo filho não pode executar o programa alvo");
    }
    exit(EXIT_FAILURE);
  }
  else if (pid < 0)
  {
    perror("Internal: Processo filho não iniciou");
  }
  else
  {
    do
    {
      waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));

  }

  return 1;
}

Builtin_Function has_builtin_for(const char *cstring)
{
  for (int i = 0; i < number_of_builtins; i++)
  {
    if (strcmp(cstring, builtin_cstring[i]) == 0)
    {
      return builtin_func[i];
    }
  }

  return NULL;
}

int shell_execute_command(char **args)
{
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

  return shell_launch_process(args);
}

// @note Não tenho certeza de nomes, nem de estrutura ainda, mas vamos ver como flui.
void read_eval_shell_loop()
{
  char *readed_line;
  char **splitted_by_delimiter;
  int status = 0;

  do 
  {
    readed_line = shell_wait_command_input();
    splitted_by_delimiter = shell_split_command_into_args(readed_line);
    // @note já consegue iniciar processos, por hora precisam ser com o caminho absoluto "/usr/bin/ls"
    // @note só precisava mudar para `execvp` para ele aceitar ls sem o caminho completo
    status = shell_execute_command(splitted_by_delimiter);

    if (readed_line)
    {
      free(readed_line);
    }
    if (splitted_by_delimiter)
    {
      free(splitted_by_delimiter);
    }
    printf("\n");
    // @note loop infinito por enquanto
  } while (status);
}

static struct termios original_config;

void deactivate_raw_mode()
{
  if (DEBUG_INFO) printf("[[ deactivate_raw_mode ]] :: restaurando configurações de terminal.\n");
  tcsetattr(STDERR_FILENO, TCSAFLUSH, &original_config);
}

void activate_raw_mode()
{
  // @todo João, precisa restaurar antes de sair?
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