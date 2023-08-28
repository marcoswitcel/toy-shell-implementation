#ifndef PROCESS_MANAGER_C
#define PROCESS_MANAGER_C

#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

#include "./list.implementations.h"
#include "./utils.c"

typedef struct Process_Parameter {
  Null_Terminated_Pointer_Array args;
  int fd_stdout;
} Process_Parameter;

typedef struct Process_Handles {
  FILE *stdin;
  FILE *stdout;
  FILE *stderr;
} Process_Handles;

#define STATIC_PROCESS_PARAMETER(ARGS) (Process_Parameter) { .args = ARGS, .fd_stdout = -1, }
#define STATIC_PROCESS_HANDLES() (Process_Handles) { .stdin = stdin, .stdout = stdout, .stderr = stderr, }

bool wait_child_process(pid_t pid)
{
  int status;

  do
  {
    waitpid(pid, &status, WUNTRACED);
  } while (!WIFEXITED(status) && !WIFSIGNALED(status));

  return true;
}

int launch_process(const Process_Parameter process_parameter)
{
  pid_t pid;
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
      printf("Internal: Processo filho não pode executar o programa alvo.\n");
    }
    exit(EXIT_FAILURE);
  }
  else if (pid < 0)
  {
    printf("Internal: Processo filho não iniciou.");
  }
  else
  {
    wait_child_process(pid);
  }

  close(process_parameter.fd_stdout);

  return 1;
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

#endif // PROCESS_MANAGER_C
