#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// @note Não tenho certeza de nomes, nem de estrutura ainda, mas vamos ver como flui.
void read_eval_shell_loop()
{
  char *read_line;
  char **splitted_by_delimiter;
  int status;

  while (true) {
    printf("|>");
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