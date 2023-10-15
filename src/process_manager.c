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
  int fd_stdin;
  int fd_stdout;
  int fd_stderr;
  struct Process_Parameter *pipe_through;
} Process_Parameter;

typedef struct Process_Handles {
  int stdin;
  int stdout;
  int stderr;
} Process_Handles;

#define STATIC_PROCESS_PARAMETER(ARGS) (Process_Parameter) { .args = ARGS, .fd_stdin = -1, .fd_stdout = -1, .fd_stderr = -1, .pipe_through = NULL, }
#define STATIC_PROCESS_HANDLES() (Process_Handles) { .stdin = STDIN_FILENO, .stdout = STDOUT_FILENO, .stderr = STDERR_FILENO, }

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
    // @todo João, remover asserts assim que valores começarem a ser usados

    // @todo João, validar se o stdin pode ser setado da mesma forma que os file descriptors de output
    if (process_parameter.fd_stdin > -1)
    {
      assert(false && "não deveria estar sendo usado ainda");
      dup2(process_parameter.fd_stdin, STDIN_FILENO);
    }

    if (process_parameter.fd_stdout > -1)
    {
      dup2(process_parameter.fd_stdout, STDOUT_FILENO);
    }

    if (process_parameter.fd_stderr > -1)
    {
      dup2(process_parameter.fd_stderr, STDERR_FILENO);
    }

    /**
     * @note Aqui entrega o controle do processo atual para o comando que o usuário
     * tentou invocar, caso falhe os trechos abaixo do if cuidarão do encerramento da
     * thread e do print da mensagem de erro para o stdout do processo pai.
     * Por hora o processo pai está esperando a respota desse comando (sincronizado),
     * então o printf será emitido na ordem certa.
     * 
     * @note `execvp` procura pelo comando requisitado no `PATH`.
     * @reference https://man7.org/linux/man-pages/man3/exec.3p.html
     */
    if (execvp(process_parameter.args[0], process_parameter.args) == -1)
    {
      write(STDOUT_FILENO, EXPAND_STRING_REF_AND_COUNT("Internal: Processo filho não pode executar o programa alvo.\n"));
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
    write(STDOUT_FILENO, EXPAND_STRING_REF_AND_COUNT("Internal: Processo filho não iniciou."));
  }
  else
  {
    wait_child_process(pid);
  }

  // @todo João, checar se pode chamar close com -1
  // @todo João, pensar em como isso vai funcionar quando estiver fazendo o tunelamento de output
  // @todo João, quando fecha o stdin?
  close(process_parameter.fd_stdout);
  close(process_parameter.fd_stderr);

  return 1;
}

pid_t fork_and_run(void (*func)(void), bool wait)
{
  pid_t pid = fork();

  if (pid == 0)
  {
    // processo filho
    func();

    _Exit(EXIT_SUCCESS);
  }
  else if (pid < 0)
  {
    write(STDOUT_FILENO, EXPAND_STRING_REF_AND_COUNT("Thread não iniciada com sucesso."));
  }
  else if (wait)
  {
    wait_child_process(pid);
  }

  return pid;
}

#endif // PROCESS_MANAGER_C
