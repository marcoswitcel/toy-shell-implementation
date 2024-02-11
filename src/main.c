#include "./compilation_definitions.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>

#include "./shell.c"
#include "./terminal.c"
#include "./debug_log.c"
#include "./command-line-arguments.c"

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
  (void) signal; // suppress unused variable
  /**
   * Estou restaurando o modo "normal" do terminal enquanto o processo filho executa.
   * Por isso os handlers são acionados em caso de algum sinal ser disparado.
   * Cadastrei esse handler para lidar com o SIGINT, porém, em teoria podem e virão
   * mais tipos de sinais. Por hora vou deixar só esse, mas devo considerar os outros casos.
   */
}

int main(int argc, const char *argv[])
{
  Command_Line_Arguments arguments = ARGUMENTS_DEFAULTS();
  command_line_arguments_apply_argv(&arguments, argc, argv);

  activate_raw_mode(true);
  signal(SIGINT, handle_sigint);
  init_debug_log();

  // @note talvez ler um arquivo de configuração? aceitar a configuração do modo colorful e do modo sem som?

  if (arguments.help)
  {
    handle_help_request();
  }
  else
  {
    // loop principal, toda lógica roda aqui
    read_eval_shell_loop(arguments.colorful, arguments.no_sound);
  }

  // considerações finais do programa
  close_debug_log();

  return EXIT_SUCCESS;
}
