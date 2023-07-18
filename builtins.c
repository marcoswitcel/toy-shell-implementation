
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
      return 0; // @note por hora uma falha no cd encerra o terminal... mudar
    }
  }

  return 1;
}

int builtin_help(char **args)
{
  printf("Toy Shell Implementation - João Marcos\n");
  printf("Baseado no tutorial de Stephen Brennan\n");
  printf("Link: https://brennan.io/2015/01/16/write-a-shell-in-c/\n\n");

  if (args[1] != NULL)
  {
    printf("Por hora o comando \"help\" não consegue explicar \"%s\".\n", args[1]);  
    return 1;
  }

  printf("Comandos builtin básicos:\n");
  for (int i = 0; i < number_of_builtins; i++)
  {
    printf("  %s\n", builtin_cstring[i]);
  }

  printf("\nDigite o nome do programa ou builtin, seguido pelos argumentos e aperte enter para executar.");
  
  return 1;
}

int builtin_exit(char **args)
{
  if (args[1] != NULL)
  {
    printf("Warning: Ignorando argumentos.\n");  
  }

  printf("saindo, até mais!!!");
  return 0;
}

