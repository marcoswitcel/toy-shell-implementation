#ifndef SHELL_BUILTINS_C
#define SHELL_BUILTINS_C

#include <stdbool.h>
#include <stdio.h>

#include "./process_manager.c"

bool exit_requested = false;

typedef int (*Builtin_Function)(char **, Process_Handles *);

// Definições
int builtin_cd(char **args, Process_Handles *handles);
int builtin_help(char **args, Process_Handles *handles);
int builtin_exit(char **args, Process_Handles *handles);
int builtin_clear(char **args, Process_Handles *handles);

const char *builtin_cstring[] = {
  "cd",
  "help",
  "exit",
  "clear",
};

const Builtin_Function builtin_func[] = {
  &builtin_cd,
  &builtin_help,
  &builtin_exit,
  &builtin_clear,
};

const int number_of_builtins = sizeof(builtin_cstring) / sizeof(char *);

// Implementações
int builtin_cd(char **args, Process_Handles *handles)
{
  char *target_folder = args[1];

  if (target_folder == NULL)
  {
    fprintf(handles->stderr, "cd: esperava argumento");
  }
  else
  {
    if (chdir(target_folder) != 0)
    {
      fprintf(handles->stderr, "cd: não foi possível acessar o diretório \"%s\"\n", target_folder);
      return 0; // @note por hora uma falha no cd encerra o terminal... mudar
    }
  }

  return 1;
}

int builtin_help(char **args, Process_Handles *handles)
{
  fprintf(handles->stdout, "Toy Shell Implementation - João Marcos\n");
  fprintf(handles->stdout, "Baseado no tutorial de Stephen Brennan\n");
  fprintf(handles->stdout, "Link: https://brennan.io/2015/01/16/write-a-shell-in-c/\n\n");

  if (args[1] != NULL)
  {
    fprintf(handles->stdout, "Por hora o comando \"help\" não consegue explicar \"%s\".\n", args[1]);  
    return 1;
  }

  fprintf(handles->stdout, "Comandos builtin básicos:\n");
  for (int i = 0; i < number_of_builtins; i++)
  {
    fprintf(handles->stdout, "  %s\n", builtin_cstring[i]);
  }

  fprintf(handles->stdout, "\nDigite o nome do programa ou builtin, seguido pelos argumentos e aperte enter para executar.\n");
  
  return 1;
}

int builtin_exit(char **args, Process_Handles *handles)
{
  if (args[1] != NULL)
  {
    fprintf(handles->stdout, "Warning: Ignorando argumentos.\n");  
  }

  fprintf(handles->stdout, "saindo, até mais!!!\n");
  exit_requested = true;
  
  return 0;
}

int builtin_clear(char **args, Process_Handles *handles)
{
  clear_terminal();

  if (args && args[1] != NULL)
  {
    fprintf(handles->stdout, "Warning: Ignorando argumentos para clear .\n");  //@note apenas para evitar avisos do compilador sobre variáveis não usadas
  }

  return 1;
}

Builtin_Function has_builtin_for(const char *cstring)
{
  for (int i = 0; i < number_of_builtins; i++)
  {
    if (strcmp(cstring, builtin_cstring[i]) == 0)
    {
      return builtin_func[i];
    }
  }

  return NULL;
}

#endif // SHELL_BUILTINS_C
