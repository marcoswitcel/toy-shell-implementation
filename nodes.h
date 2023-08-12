#ifndef NODES_H
#define NODES_H

typedef struct Execute_Command_Node {
  char **args;
  const char *output_filename;
  bool append_mode;
} Execute_Command_Node;


#endif // NODES_H
