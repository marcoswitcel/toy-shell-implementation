#ifndef _SHELL_H_
#define _SHELL_H_

#include "./list.implementations.h"

typedef struct Shell_Context_Data
{
  bool colorful;
  bool soundful;
  /**
   * @brief Armazena o histórico de comandos digitados
  */
  List_Of_Strings *last_typed_commands;
  int next_typed_command_to_show;
  int last_status_code;
  /**
   * @brief Usada como controle para o loop de interpretação do shell
   */
  bool exit_requested;
  const char *input_mark;
  unsigned input_mark_length;
} Shell_Context_Data;

/**
 * @brief Status global do shell
 * Usado principalmente para compartilhar dados com os builtins
 */
extern Shell_Context_Data *the_shell_context;

#endif // _SHELL_H_
