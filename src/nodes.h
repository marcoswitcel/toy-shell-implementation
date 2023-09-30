#ifndef NODES_H
#define NODES_H

#include <stdbool.h>

typedef struct Execute_Command_Node {
  char **args;
  const char *output_filename;
  signed token_index_start;
  bool append_mode;
  struct Execute_Command_Node *pipe;
} Execute_Command_Node;

#define STATIC_EXECUTE_COMMAND_NODE() (Execute_Command_Node) { .args = NULL, .output_filename = NULL, .token_index_start = -1, .append_mode = false, }


#endif // NODES_H
