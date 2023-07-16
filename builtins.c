
typedef int (*Builtin_Function)(char **);

// Definições
int builtin_cd(char **args);
int builtin_help(char **args);
int builtin_exit(char **args);

char *builtin_cstring[] = {
  "cd",
  "help",
  "exit",
};

Builtin_Function builtin_func[] = {
  &builtin_cd,
  &builtin_help,
  &builtin_exit,
};

const int number_of_builtins = sizeof(builtin_cstring) / sizeof(char *);

// Implementações
int builtin_cd(char **args)
{
  printf("executado builtin_cd");
  return 1; //@todo revisar
}

int builtin_help(char **args)
{
  printf("executado builtin_help");
  return 1; //@todo revisar
}

int builtin_exit(char **args)
{
  printf("executado builtin_exit");
  return 1; //@todo revisar
}

