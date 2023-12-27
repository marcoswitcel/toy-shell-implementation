#ifndef _COMMAND_LINE_ARGUMENTS_C_
#define _COMMAND_LINE_ARGUMENTS_C_

#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#include "./utils.macro.h"

/**
 * @brief Define e documenta os parâmetros aceitos pela linha de comando
 */
typedef struct Command_Line_Arguments {
  /**
   * @brief define se pode usar "escape sequences" para colorir e destacar aspectos da interface de texto
   * Padrão: false
   */
  bool colorful;
  /**
   * @brief define se o interpretador pode emitir caracteres que causem som no terminal 
   * Padrão: false
   */
  bool no_sound;
  /**
   * @brief define se o usuário está requisitando auxílio com os parâmetros
   */
  bool help;
} Command_Line_Arguments;

/**
 * @brief aqui estão definidos os valores padrões para os parâmetros
 */
#define ARGUMENTS_DEFAULTS() { .colorful = false, .no_sound = false, .help = false }

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
  arguments->no_sound = is_string_present_in_argv("--no_sound", argc, argv);
  arguments->help = is_string_present_in_argv("--help", argc, argv);
}

void handle_help_request(void)
{
  write(STDOUT_FILENO, EXPAND_STRING_REF_AND_COUNT("Toy Shell Implementation - João Marcos\r\n"));
  write(STDOUT_FILENO, EXPAND_STRING_REF_AND_COUNT("Baseado no tutorial de Stephen Brennan\r\n"));
  write(STDOUT_FILENO, EXPAND_STRING_REF_AND_COUNT("Link: https://brennan.io/2015/01/16/write-a-shell-in-c/"));
}

#endif // _COMMAND_LINE_ARGUMENTS_C_
