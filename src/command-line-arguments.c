#ifndef _COMMAND_LINE_ARGUMENTS_C_
#define _COMMAND_LINE_ARGUMENTS_C_

#include <stdbool.h>
#include <string.h>

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
} Command_Line_Arguments;

/**
 * @brief aqui estão definidos os valores padrões para os parâmetros
 */
#define ARGUMENTS_DEFAULTS() { .colorful = false, .no_sound = false }

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
  // @note Talvez adicionar um argumento para controlar a emissão de som?
}

#endif // _COMMAND_LINE_ARGUMENTS_C_
