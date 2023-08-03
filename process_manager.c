#ifndef PROCESS_MANAGER_C
#define PROCESS_MANAGER_C

#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

typedef struct Process_Parameter {
    char **args;
    int fd_stdout;
} Process_Parameter;

int launch_process(char **args, int fd_stdout)
{
  pid_t pid;
  int status;

  pid = fork();

  if (pid == 0)
  {
    if (fd_stdout > 0)
    {
      dup2(fd_stdout, STDOUT_FILENO);
    }
    // processo filho
    if (execvp(args[0], args) == -1)
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

  close(fd_stdout);

  return 1;
}


#endif // PROCESS_MANAGER_C