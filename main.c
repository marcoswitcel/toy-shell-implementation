#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

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

char **shell_split_command_into_args(const char *commands)
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

// @note Não tenho certeza de nomes, nem de estrutura ainda, mas vamos ver como flui.
void read_eval_shell_loop()
{
  char *readed_line;
  char **splitted_by_delimiter;
  // int status;

  while (true)
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
    if (readed_line)
    {
      free(readed_line);
    }
    printf("\n");
    // @note loop infinito por enquanto
  }
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