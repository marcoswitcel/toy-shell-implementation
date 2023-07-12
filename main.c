#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

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

// @note Não tenho certeza de nomes, nem de estrutura ainda, mas vamos ver como flui.
void read_eval_shell_loop()
{
  char *readed_line;
  // char **splitted_by_delimiter;
  // int status;

  while (true)
  {
    printf("|>");
    readed_line = shell_read_command();
    printf("[%s]", readed_line);
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