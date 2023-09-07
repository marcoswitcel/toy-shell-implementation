#include "./compilation_definitions.h"

#include <stdio.h>
#include <stdlib.h>
#include <termios.h>

#include "./shell.c"


static struct termios original_config;

void deactivate_raw_mode()
{
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

  // @note Aceitar argumentos pela linha de comando? que argumentos?
  // @note talvez ler um arquivo de configuração? mas que configurações aceitar?

  // loop principal, toda lógica roda aqui
  read_eval_shell_loop();

  // @note Cleanup se tiver algum para fazer

  return EXIT_SUCCESS;
}
