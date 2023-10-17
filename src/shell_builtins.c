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

typedef int (*Builtin_Function)(const Process_Parameter *);

// Definições

int builtin_cd(const Process_Parameter *process_parameter);
int builtin_help(const Process_Parameter *process_parameter);
int builtin_exit(const Process_Parameter *process_parameter);
int builtin_clear(const Process_Parameter *process_parameter);
int builtin_history(const Process_Parameter *process_parameter);

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

int builtin_cd(const Process_Parameter *process_parameter)
{
  char *target_folder = process_parameter->args[1];

  if (target_folder == NULL)
  {
    write(process_parameter->fd_stderr, EXPAND_STRING_REF_AND_COUNT("cd: esperava argumento\n"));
  }
  else if (chdir(target_folder) != 0)
  {
    write(process_parameter->fd_stderr, EXPAND_STRING_REF_AND_COUNT("cd: não foi possível acessar o diretório \""));
    write(process_parameter->fd_stderr, target_folder, strlen(target_folder));
    write(process_parameter->fd_stderr, EXPAND_STRING_REF_AND_COUNT("\"\n"));
  }

  return 1;
}

int builtin_help(const Process_Parameter *process_parameter)
{
  write(process_parameter->fd_stdout, EXPAND_STRING_REF_AND_COUNT("Toy Shell Implementation - João Marcos\n"));
  write(process_parameter->fd_stdout, EXPAND_STRING_REF_AND_COUNT("Baseado no tutorial de Stephen Brennan\n"));
  write(process_parameter->fd_stdout, EXPAND_STRING_REF_AND_COUNT("Link: https://brennan.io/2015/01/16/write-a-shell-in-c/\n\n"));

  if (process_parameter->args[1] != NULL)
  {
    write(process_parameter->fd_stdout, EXPAND_STRING_REF_AND_COUNT("Por hora o comando \"help\" não consegue explicar \""));
    write(process_parameter->fd_stdout, process_parameter->args[1], strlen(process_parameter->args[1]));  
    write(process_parameter->fd_stdout, EXPAND_STRING_REF_AND_COUNT("\".\n"));
    return 1;
  }

  write(process_parameter->fd_stdout, EXPAND_STRING_REF_AND_COUNT("Comandos builtin básicos:\n"));
  for (int i = 0; i < number_of_builtins; i++)
  {
    write(process_parameter->fd_stdout, EXPAND_STRING_REF_AND_COUNT("  "));
    write(process_parameter->fd_stdout, builtin_cstring[i], strlen(builtin_cstring[i]));
    write(process_parameter->fd_stdout, EXPAND_STRING_REF_AND_COUNT("\n"));
  }

  write(process_parameter->fd_stdout, EXPAND_STRING_REF_AND_COUNT("\nDigite o nome do programa ou builtin, seguido pelos argumentos e aperte enter para executar.\n"));
  
  return 1;
}

int builtin_exit(const Process_Parameter *process_parameter)
{
  if (process_parameter->args[1] != NULL)
  {
    write(process_parameter->fd_stdout, EXPAND_STRING_REF_AND_COUNT("Warning: Ignorando argumentos.\n"));
  }

  write(process_parameter->fd_stdout, EXPAND_STRING_REF_AND_COUNT("saindo, até mais!!!\n"));
  exit_requested = true;
  
  return 0;
}

int builtin_clear(const Process_Parameter *process_parameter)
{
  clear_terminal();

  if (process_parameter->args && process_parameter->args[1] != NULL)
  {
    write(process_parameter->fd_stdout, EXPAND_STRING_REF_AND_COUNT("Warning: Ignorando argumentos para clear .\n"));
  }

  return 1;
}

int builtin_history(const Process_Parameter *process_parameter)
{  
  write(process_parameter->fd_stdout, EXPAND_STRING_REF_AND_COUNT("Resumo:\n"));

  if (last_typed_commands && last_typed_commands->index)
  {
    for (unsigned i = 0; i < last_typed_commands->index; i++)
    {
      const char *command = last_typed_commands->data[i];
      write(process_parameter->fd_stdout, EXPAND_STRING_REF_AND_COUNT("  "));
      write(process_parameter->fd_stdout, command, strlen(command));
      write(process_parameter->fd_stdout, EXPAND_STRING_REF_AND_COUNT("\n"));
    }
  }
  else 
  {
    write(process_parameter->fd_stdout, EXPAND_STRING_REF_AND_COUNT("Nenhum comando digitado ainda.\n"));
  }

  write(process_parameter->fd_stdout, EXPAND_STRING_REF_AND_COUNT("\n"));

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
