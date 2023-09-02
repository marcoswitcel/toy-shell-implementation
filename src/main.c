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
#include "./utils.macro.h"
#include "./types.h"
#include "./tokens.h"
#include "./parser.h"
#include "./shell.c"


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
