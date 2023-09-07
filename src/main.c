#include "./compilation_definitions.h"

#include <stdio.h>
#include <stdlib.h>

#include "./shell.c"
#include "./terminal.c"

int main(void)
{
  activate_raw_mode(true);

  // @note Aceitar argumentos pela linha de comando? que argumentos?
  // @note talvez ler um arquivo de configuração? mas que configurações aceitar?

  // loop principal, toda lógica roda aqui
  read_eval_shell_loop();

  // @note Cleanup se tiver algum para fazer

  return EXIT_SUCCESS;
}
