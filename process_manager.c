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


#endif // PROCESS_MANAGER_C