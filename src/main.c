#include "./compilation_definitions.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "./shell.c"
#include "./terminal.c"

typedef struct Command_Line_Arguments {
  bool colorful;
} Command_Line_Arguments;

bool is_string_present_in_argv(const char *switch_name, int argc, const char *argv[])
{
  for (int i = 0; i < argc; i++)
  {
    if (!strcmp(argv[i], switch_name))
    {
      return true;
    }
  }
  
  return false;
}

void command_line_arguments_apply_argv(Command_Line_Arguments *arguments, int argc, const char *argv[])
{
  arguments->colorful = is_string_present_in_argv("--colorful", argc, argv);
}

int main(int argc, const char *argv[])
{
  // @note Aceitar argumentos pela linha de comando? que argumentos?
  Command_Line_Arguments arguments = { 0 };
  command_line_arguments_apply_argv(&arguments, argc, argv);

  activate_raw_mode(true);

  // @note talvez ler um arquivo de configuração? mas que configurações aceitar?

  // loop principal, toda lógica roda aqui
  read_eval_shell_loop(arguments.colorful);

  // @note Cleanup se tiver algum para fazer

  return EXIT_SUCCESS;
}
