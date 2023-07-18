#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "./builtins.c"

#define LINE_BUFFER_SIZE 1024

char *shell_read_command(void)
{
  unsigned buffer_size = LINE_BUFFER_SIZE;
  unsigned position = 0;
  // @note Essa é a memória que será retornada
  char *buffer = malloc(sizeof(char) * buffer_size);
  int c;

  // @note Acho que poderia ter um buffer separado para gerenciar essa memória
  if (!buffer_size)
  {
    fprintf(stderr, "Internal: Erro de alocação");
    exit(EXIT_FAILURE);
  }

  while (true)
  {
    c = getchar();

    if (c == EOF || c == '\n')
    {
      buffer[position] = '\0';
      return buffer;
    }
    else
    {
      buffer[position] = c;
    }
    position++;

    if (position >= buffer_size)
    {
      buffer_size += LINE_BUFFER_SIZE;
      char *new_buffer = realloc(buffer, buffer_size);
      if (!new_buffer)
      {
        fprintf(stderr, "Internal: Erro de alocação");
        free(buffer); // @note Embora vai ser encerrado o processo, vou deixar o comando de `free` aqui pra não esquecer
        // @note Muito prematura esse encerramento, mas por hora fica assim.
        exit(EXIT_FAILURE);
      }
      else
      {
        buffer = new_buffer;
      }
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
    printf("|>");
    readed_line = shell_read_command();
    printf("linha lida: [%s]\n", readed_line); // @note apenas para debugar
    splitted_by_delimiter = shell_split_command_into_args(readed_line);
    // @note o bloco abaixo é apenas para visualizar o resultado
    {
      char **token = splitted_by_delimiter;
      while (*token != NULL)
      {
        printf("argumento extraído: [%s]\n", *token);
        token++;
      }
    }
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

int main(void)
{
  // @note Aceitar argumentos pela linha de comando? que argumentos

  // @note talvez ler um arquivo de configuração? mas que configurações aceitar?

  // Read Eval Loop
  read_eval_shell_loop();

  // @note Cleanup se tiver algum para fazer

  return EXIT_SUCCESS;
}