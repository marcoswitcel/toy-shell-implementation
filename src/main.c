#include "./compilation_definitions.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>

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

/**
 * @brief Handler vazio do sigint
 * 
 * 
 * @link https://stackoverflow.com/questions/17766550/ctrl-c-interrupt-event-handling-in-linux
 * @link https://stackoverflow.com/questions/14205200/prevent-c-program-from-getting-killed-with-ctrl-c
 * @link https://stackoverflow.com/questions/41909348/set-a-custom-function-as-handler-to-all-signals
 * 
 * @param signal 
 */
void handle_sigint(int signal)
{
  (void) signal;
  /**
   * Estou restaurando o modo "normal" do terminal enquanto o processo filho executa.
   * Por isso os handlers são acionados em caso de algum sinal ser disparado.
   * Cadastrei esse handler para lidar com o SIGINT, porém, em teoria podem e virão
   * mais tipos de sinais. Por hora vou deixar só esse, mas devo considerar os outros casos.
   */
}

int main(int argc, const char *argv[])
{
  // @note Aceitar argumentos pela linha de comando? que argumentos?
  Command_Line_Arguments arguments = { 0 };
  command_line_arguments_apply_argv(&arguments, argc, argv);

  activate_raw_mode(true);
  signal(SIGINT, handle_sigint);

  // @note talvez ler um arquivo de configuração? mas que configurações aceitar?

  // loop principal, toda lógica roda aqui
  read_eval_shell_loop(arguments.colorful);

  // @note Cleanup se tiver algum para fazer

  return EXIT_SUCCESS;
}
