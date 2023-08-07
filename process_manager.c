#ifndef PROCESS_MANAGER_C
#define PROCESS_MANAGER_C

#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

#include "./list.implementations.h"

/**
 * @brief o argv da família de comandos `exec` recebe um array de ponteiros para char terminado com null
 */
typedef char** Null_Terminated_Pointer_Array;

typedef struct Process_Parameter {
  Null_Terminated_Pointer_Array args;
  int fd_stdout;
} Process_Parameter;

int launch_process(const Process_Parameter process_parameter)
{
  pid_t pid;
  int status;

  pid = fork();

  if (pid == 0)
  {
    if (process_parameter.fd_stdout > 0)
    {
      dup2(process_parameter.fd_stdout, STDOUT_FILENO);
    }

    // processo filho
    if (execvp(process_parameter.args[0], process_parameter.args) == -1)
    {
      printf("Internal: Processo filho não pode executar o programa alvo");
    }
    exit(EXIT_FAILURE);
  }
  else if (pid < 0)
  {
    printf("Internal: Processo filho não iniciou");
  }
  else
  {
    do
    {
      waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));

  }

  close(process_parameter.fd_stdout);

  return 1;
}

bool wait_child_process(pid_t pid)
{
  int status;

  do
  {
    waitpid(pid, &status, WUNTRACED);
  } while (!WIFEXITED(status) && !WIFSIGNALED(status));

  return true;
}

pid_t fork_and_run(void (*func)(void), bool wait)
{
  pid_t pid = fork();

  if (pid == 0)
  {
    // processo filho
    func();

    exit(EXIT_SUCCESS);
  }
  else if (pid < 0)
  {
    printf("Thread não iniciada com sucesso.");
  }
  else if (wait)
  {
    wait_child_process(pid);
  }

  return pid;
}

Null_Terminated_Pointer_Array convert_list_to_argv(const List_Of_Strings *list)
{
  assert(list);
  Null_Terminated_Pointer_Array args = malloc((list->index + 1) * sizeof(char *));

  if (!args)
  {
    fprintf(stderr, "Internal: Erro de alocação");
    exit(EXIT_FAILURE);
  }

  for (unsigned i = 0; i < list->index; i++)
  {
    args[i] = (char *) list->data[i];
  }
  args[list->index] = NULL;

  return args;
}

#endif // PROCESS_MANAGER_C