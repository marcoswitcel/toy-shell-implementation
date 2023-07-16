
typedef int (*Builtin_Function)(char **);

// Definições
int builtin_cd(char **args);
int builtin_help(char **args);
int builtin_exit(char **args);

const char *builtin_cstring[] = {
  "cd",
  "help",
  "exit",
};

const Builtin_Function builtin_func[] = {
  &builtin_cd,
  &builtin_help,
  &builtin_exit,
};

const int number_of_builtins = sizeof(builtin_cstring) / sizeof(char *);

// Implementações
int builtin_cd(char **args)
{
  char *target_folder = args[1];

  if (target_folder == NULL)
  {
    fprintf(stderr, "cd: esperava argumento");
  }
  else
  {
    if (chdir(target_folder) != 0)
    {
      fprintf(stderr, "cd: não consegiu trocar para o diretório \"%s\"", target_folder);
      return 0;
    }
  }

  return 1; //@todo revisar
}

int builtin_help(char **args)
{
  // @note pendente implementação e explicação
  
  printf("executado builtin_help");
  return 1; //@todo revisar
}

int builtin_exit(char **args)
{
  // @note pendente implementação e explicação
  
  printf("executado builtin_exit");
  return 1; //@todo revisar
}

