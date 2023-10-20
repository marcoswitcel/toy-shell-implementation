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


#endif // NODES_H
