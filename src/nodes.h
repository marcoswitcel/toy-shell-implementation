#ifndef NODES_H
#define NODES_H

#include <stdbool.h>

typedef struct Execute_Command_Node {
  char **args;
  const char *output_filename;
  bool append_mode;
} Execute_Command_Node;

#define STATIC_EXECUTE_COMMAND_NODE() (Execute_Command_Node) { .args = NULL, .output_filename = NULL, .append_mode = false, }


#endif // NODES_H
