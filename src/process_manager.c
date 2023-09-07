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

    /**
     * @note Aqui entrega o controle do processo atual para o comando que o usuário
     * tentou invocar, caso falhe os trechos abaixo do if cuidarão do encerramento da
     * thread e do print da mensagem de erro para o stdout do processo pai.
     * Por hora o processo pai está esperando a respota desse comando (sincronizado),
     * então o printf será emitido na ordem certa.
     * 
     */
    if (execvp(process_parameter.args[0], process_parameter.args) == -1)
    {
      printf("Internal: Processo filho não pode executar o programa alvo.\n");
    }

    /**
     * @note João, o ideial seria nem fazer o fork caso o programar não fosse encontrado,
     * porém, não analisei se tem outro possível motivo para cair aqui, portanto não vou
     * remover essa lógica aqui.
     * Deixei o comando marcado como sucesso porquê o erro será tratado na thread principal
     * então não tem porquê deixar esse processo terminar com código de erro.
     * Outra coisa, substituí a chamada da função `exit` pela função `_Exit` porque ela não
     * invoca os handlers registrados com a função `atexit`.
     */
    _Exit(EXIT_SUCCESS);
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