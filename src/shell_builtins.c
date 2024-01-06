#ifndef SHELL_BUILTINS_C
#define SHELL_BUILTINS_C

#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "./shell.h"
#include "./list.implementations.h"
#include "./process_manager.c"
#include "./utils.macro.h"


typedef int (*Builtin_Function)(const Process_Parameter *);

// Definições

int builtin_cd(const Process_Parameter *process_parameter);
int builtin_set(const Process_Parameter *process_parameter);
int builtin_help(const Process_Parameter *process_parameter);
int builtin_exit(const Process_Parameter *process_parameter);
int builtin_clear(const Process_Parameter *process_parameter);
int builtin_history(const Process_Parameter *process_parameter);

const char *builtin_cstring[] = {
  "cd",
  "set",
  "help",
  "exit",
  "clear",
  "history",
};

const Builtin_Function builtin_func[] = {
  &builtin_cd,
  &builtin_set,
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
    write(process_parameter->fd_stderr, EXPAND_STRING_REF_AND_COUNT("cd: esperava argumento\r\n"));
  }
  else if (chdir(target_folder) != 0)
  {
    write(process_parameter->fd_stderr, EXPAND_STRING_REF_AND_COUNT("cd: não foi possível acessar o diretório \""));
    write(process_parameter->fd_stderr, target_folder, strlen(target_folder));
    write(process_parameter->fd_stderr, EXPAND_STRING_REF_AND_COUNT("\"\r\n"));

    return 1;
  }

  return 0;
}

int builtin_set(const Process_Parameter *process_parameter)
{
  // @todo João, ajustar aqui para poder trocar o modo colorful e no_sound, talvez também o input mark
  // @todo João, seria interessante checar do lado do shell aonde o cursor está? ao invés de forçar uma linha
  // nova nos builtins sempre 

  if (process_parameter->args[1] != NULL)
  {
    char *arg1 =  process_parameter->args[1];
    if (strcmp(arg1, "--help") == 0)
    {
      write(process_parameter->fd_stdout, EXPAND_STRING_REF_AND_COUNT("Descrição do comando 'set'\r\nPermite alterar o valor de parâmetros:\r\nset colorful 1|0\n\r")); 
    }
    else if (strcmp(arg1, "colorful") == 0)
    {
      if (process_parameter->args[2] != NULL)
      {
        char *arg2 = process_parameter->args[2];
        if (strcmp(arg2, "1") == 0)
        {
          the_shell_context->colorful = true;
        }
        else if (strcmp(arg2, "0") == 0)
        {
          the_shell_context->colorful = false;
        }
        else
        {
          write(process_parameter->fd_stdout, EXPAND_STRING_REF_AND_COUNT("set: opção invalida para o set\r\n"));
          return 1;
        }
      }
      else
      {
        the_shell_context->colorful = !the_shell_context->colorful;
      }
    }
    else if (strcmp(arg1, "no_sound") == 0)
    {
      if (process_parameter->args[2] != NULL)
      {
        char *arg2 = process_parameter->args[2];
        if (strcmp(arg2, "1") == 0)
        {
          the_shell_context->soundful = false;
        }
        else if (strcmp(arg2, "0") == 0)
        {
          the_shell_context->soundful = true;
        }
        else
        {
          write(process_parameter->fd_stdout, EXPAND_STRING_REF_AND_COUNT("set: opção invalida para o set\r\n"));
          return 1;
        }
      }
      else
      {
        the_shell_context->soundful = !the_shell_context->soundful;
      }
    }
    else
    {
      // @todo João, concatenar o argumento aqui
      write(process_parameter->fd_stdout, EXPAND_STRING_REF_AND_COUNT("argumento não conhecido\r\n"));
      return 1;
    }
  }
  else
  {
    write(process_parameter->fd_stdout, EXPAND_STRING_REF_AND_COUNT("use --help para receber ajuda\r\n"));
    return 1;
  }

  return 0;
}

int builtin_help(const Process_Parameter *process_parameter)
{
  write(process_parameter->fd_stdout, EXPAND_STRING_REF_AND_COUNT("Toy Shell Implementation - João Marcos\r\n"));
  write(process_parameter->fd_stdout, EXPAND_STRING_REF_AND_COUNT("Baseado no tutorial de Stephen Brennan\r\n"));
  write(process_parameter->fd_stdout, EXPAND_STRING_REF_AND_COUNT("Link: https://brennan.io/2015/01/16/write-a-shell-in-c/\r\n\r\n"));

  if (process_parameter->args[1] != NULL)
  {
    write(process_parameter->fd_stdout, EXPAND_STRING_REF_AND_COUNT("Por hora o comando \"help\" não consegue explicar \""));
    write(process_parameter->fd_stdout, process_parameter->args[1], strlen(process_parameter->args[1]));  
    write(process_parameter->fd_stdout, EXPAND_STRING_REF_AND_COUNT("\".\r\n"));
    return 0;
  }

  write(process_parameter->fd_stdout, EXPAND_STRING_REF_AND_COUNT("Comandos builtin básicos:\r\n"));
  for (int i = 0; i < number_of_builtins; i++)
  {
    write(process_parameter->fd_stdout, EXPAND_STRING_REF_AND_COUNT("  "));
    write(process_parameter->fd_stdout, builtin_cstring[i], strlen(builtin_cstring[i]));
    write(process_parameter->fd_stdout, EXPAND_STRING_REF_AND_COUNT("\r\n"));
  }

  write(process_parameter->fd_stdout, EXPAND_STRING_REF_AND_COUNT("\r\nDigite o nome do programa ou builtin, seguido pelos argumentos e aperte enter para executar.\r\n"));
  
  return 0;
}

int builtin_exit(const Process_Parameter *process_parameter)
{
  if (process_parameter->args[1] != NULL)
  {
    write(process_parameter->fd_stdout, EXPAND_STRING_REF_AND_COUNT("Warning: Ignorando argumentos.\r\n"));
  }

  write(process_parameter->fd_stdout, EXPAND_STRING_REF_AND_COUNT("saindo, até mais!!!\r\n"));
  the_shell_context->exit_requested = true;
  
  return 0;
}

int builtin_clear(const Process_Parameter *process_parameter)
{
  clear_terminal();

  if (process_parameter->args && process_parameter->args[1] != NULL)
  {
    write(process_parameter->fd_stdout, EXPAND_STRING_REF_AND_COUNT("Warning: Ignorando argumentos para clear .\r\n"));
  }

  return 0;
}

int builtin_history(const Process_Parameter *process_parameter)
{  
  write(process_parameter->fd_stdout, EXPAND_STRING_REF_AND_COUNT("Resumo:\r\n"));

  List_Of_Strings *last_typed_commands = the_shell_context->last_typed_commands;

  if (last_typed_commands && last_typed_commands->index)
  {
    for (unsigned i = 0; i < last_typed_commands->index; i++)
    {
      const char *command = last_typed_commands->data[i];
      write(process_parameter->fd_stdout, EXPAND_STRING_REF_AND_COUNT("  "));
      write(process_parameter->fd_stdout, command, strlen(command));
      write(process_parameter->fd_stdout, EXPAND_STRING_REF_AND_COUNT("\r\n"));
    }
  }
  else 
  {
    write(process_parameter->fd_stdout, EXPAND_STRING_REF_AND_COUNT("Nenhum comando digitado ainda.\r\n"));
  }

  write(process_parameter->fd_stdout, EXPAND_STRING_REF_AND_COUNT("\r\n"));

  return 0;
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
