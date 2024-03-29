#ifndef NODES_H
#define NODES_H

#include <stdbool.h>

typedef struct Execute_Command_Node {
  char **args;
  const char *stdout_redirect_filename;
  const char *stderr_redirect_filename;
  /**
   * @brief esse campo está sendo usado para armazenar o índice do token
   * que representa o nome do último argumento que indica um arquivo.
   */
  signed token_index_start;
  bool append_mode_stdout;
  bool append_mode_stderr;
  struct Execute_Command_Node *pipe;
  struct Execute_Command_Node *next_command;
} Execute_Command_Node;

#define STATIC_EXECUTE_COMMAND_NODE() (Execute_Command_Node) { .args = NULL, .stdout_redirect_filename = NULL, .stderr_redirect_filename = NULL, .token_index_start = -1, .append_mode_stdout = false, .append_mode_stderr = false, .pipe = NULL, .next_command = NULL, }

/**
 * @brief Função que libera a memória da cstrings individuais referenciadas pelo pointer_array
 *
 * @param pointer_array 
 */
void release_cstring_from_null_terminated_pointer_array_skipping_symbols(Null_Terminated_Pointer_Array pointer_array)
{
  assert(pointer_array);

  while (*pointer_array != NULL)
  {
    // @note Idealmente esse 'if' não precisaria existir aqui, poderia tratar os símbolos fixos de outra forma,
    // mas por hora o conceito de argumentos que precisam ser substituídos antes da execução existe e os símbolos
    // são strings constantes que preciso considerar antes de fazer o release.
    if (*pointer_array == static_query_last_status_code_symbol || *pointer_array == static_globbing_symbol) 
    {
      pointer_array++;
      continue;
    }

    FREE_AND_NULLIFY(*pointer_array);
    pointer_array++;
  }
}

void release_execute_command_nodes(Execute_Command_Node *execute_command_node, bool clear_root_node)
{
  Execute_Command_Node *current_command = execute_command_node;
  Execute_Command_Node *next_command = NULL;

  while (current_command)
  {
    next_command = current_command->next_command;

    if (current_command->pipe) release_execute_command_nodes(current_command->pipe, true);

    if (current_command->args) release_cstring_from_null_terminated_pointer_array_skipping_symbols(current_command->args);
    free(current_command->args);

    if (current_command != execute_command_node || clear_root_node) free(current_command);

    current_command = next_command;
  }
}

#endif // NODES_H
