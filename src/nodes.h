#ifndef NODES_H
#define NODES_H

#include <stdbool.h>

typedef struct Execute_Command_Node {
  char **args;
  const char *output_filename;
  int fd; // @todo João, refatorar isso para um lugar mais apropriado
  signed token_index_start;
  bool append_mode;
  struct Execute_Command_Node *pipe;
  struct Execute_Command_Node *next_command;
} Execute_Command_Node;

#define STATIC_EXECUTE_COMMAND_NODE() (Execute_Command_Node) { .args = NULL, .output_filename = NULL, .fd = -1, .token_index_start = -1, .append_mode = false, .pipe = NULL, .next_command = NULL, }

void release_execute_command_nodes(Execute_Command_Node *execute_command_node, bool clear_root_node)
{
  Execute_Command_Node *current_command = execute_command_node;
  Execute_Command_Node *next_command = NULL;

  while (current_command)
  {
    next_command = current_command->next_command;

    if (current_command->pipe) release_execute_command_nodes(current_command->pipe, true);

    release_cstring_from_null_terminated_pointer_array(current_command->args);
    free(current_command->args);

    if (current_command != execute_command_node || clear_root_node) free(current_command);

    current_command = next_command;
  }
}

#endif // NODES_H
