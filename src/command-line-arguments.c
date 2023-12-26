#ifndef _COMMAND_LINE_ARGUMENTS_C_
#define _COMMAND_LINE_ARGUMENTS_C_

#include <stdbool.h>
#include <string.h>

typedef struct Command_Line_Arguments {
  bool colorful;
  bool no_sound;
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
  arguments->no_sound = is_string_present_in_argv("--no_sound", argc, argv);
  // @note Talvez adicionar um argumento para controlar a emissão de som?
}

#endif // _COMMAND_LINE_ARGUMENTS_C_
