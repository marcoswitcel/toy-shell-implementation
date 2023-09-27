#ifndef SHELL_BUILTINS_C
#define SHELL_BUILTINS_C

#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "./list.implementations.h"
#include "./process_manager.c"
#include "./utils.macro.h"

bool exit_requested = false;
List_Of_Strings *last_typed_commands = NULL;

typedef int (*Builtin_Function)(char **, Process_Handles *);

// Definições

int builtin_cd(char **args, Process_Handles *handles);
int builtin_help(char **args, Process_Handles *handles);
int builtin_exit(char **args, Process_Handles *handles);
int builtin_clear(char **args, Process_Handles *handles);
int builtin_history(char **args, Process_Handles *handles);

const char *builtin_cstring[] = {
  "cd",
  "help",
  "exit",
  "clear",
  "history",
};

const Builtin_Function builtin_func[] = {
  &builtin_cd,
  &builtin_help,
  &builtin_exit,
  &builtin_clear,
  &builtin_history,
};

const int number_of_builtins = sizeof(builtin_cstring) / sizeof(char *);

// Implementações

int builtin_cd(char **args, Process_Handles *handles)
{
  char *target_folder = args[1];

  if (target_folder == NULL)
  {
    write(handles->stderr, EXPAND_STRING_REF_AND_COUNT("cd: esperava argumento\n"));
  }
  else if (chdir(target_folder) != 0)
  {
    write(handles->stderr, EXPAND_STRING_REF_AND_COUNT("cd: não foi possível acessar o diretório \""));
    write(handles->stderr, target_folder, strlen(target_folder));
    write(handles->stderr, EXPAND_STRING_REF_AND_COUNT("\"\n"));
  }

  return 1;
}

int builtin_help(char **args, Process_Handles *handles)
{
  write(handles->stdout, EXPAND_STRING_REF_AND_COUNT("Toy Shell Implementation - João Marcos\n"));
  write(handles->stdout, EXPAND_STRING_REF_AND_COUNT("Baseado no tutorial de Stephen Brennan\n"));
  write(handles->stdout, EXPAND_STRING_REF_AND_COUNT("Link: https://brennan.io/2015/01/16/write-a-shell-in-c/\n\n"));

  if (args[1] != NULL)
  {
    write(handles->stdout, EXPAND_STRING_REF_AND_COUNT("Por hora o comando \"help\" não consegue explicar \""));
    write(handles->stdout, args[1], strlen(args[1]));  
    write(handles->stdout, EXPAND_STRING_REF_AND_COUNT("\".\n"));
    return 1;
  }

  write(handles->stdout, EXPAND_STRING_REF_AND_COUNT("Comandos builtin básicos:\n"));
  for (int i = 0; i < number_of_builtins; i++)
  {
    write(handles->stdout, EXPAND_STRING_REF_AND_COUNT("  "));
    write(handles->stdout, builtin_cstring[i], strlen(builtin_cstring[i]));
    write(handles->stdout, EXPAND_STRING_REF_AND_COUNT("\n"));
  }

  write(handles->stdout, EXPAND_STRING_REF_AND_COUNT("\nDigite o nome do programa ou builtin, seguido pelos argumentos e aperte enter para executar.\n"));
  
  return 1;
}

int builtin_exit(char **args, Process_Handles *handles)
{
  if (args[1] != NULL)
  {
    write(handles->stdout, EXPAND_STRING_REF_AND_COUNT("Warning: Ignorando argumentos.\n"));
  }

  write(handles->stdout, EXPAND_STRING_REF_AND_COUNT("saindo, até mais!!!\n"));
  exit_requested = true;
  
  return 0;
}

int builtin_clear(char **args, Process_Handles *handles)
{
  clear_terminal();

  if (args && args[1] != NULL)
  {
    write(handles->stdout, EXPAND_STRING_REF_AND_COUNT("Warning: Ignorando argumentos para clear .\n"));
  }

  return 1;
}

int builtin_history(char **args, Process_Handles *handles)
{
  // Apenas para se livrar do aviso de variável não usada,
  // ela faz parte da assinatura dessa função
  (void) args; 
  
  write(handles->stdout, EXPAND_STRING_REF_AND_COUNT("History:\n"));

  if (last_typed_commands)
  {
    for (unsigned i = 0; i < last_typed_commands->index; i++)
    {
      const char *command = last_typed_commands->data[i];
      write(handles->stdout, EXPAND_STRING_REF_AND_COUNT("  "));
      write(handles->stdout, command, strlen(command));
      write(handles->stdout, EXPAND_STRING_REF_AND_COUNT("\n"));
    }
  }

  write(handles->stdout, EXPAND_STRING_REF_AND_COUNT("\n"));

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
