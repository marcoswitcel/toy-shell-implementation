#ifndef PROCESS_MANAGER_C
#define PROCESS_MANAGER_C

#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>

#include "./list.implementations.h"
#include "./utils.c"
#include "./terminal.c"

typedef struct Process_Parameter {
  Null_Terminated_Pointer_Array args;
  int fd_stdin;
  int fd_stdout;
  int fd_stderr;
  struct Process_Parameter *pipe_through;
} Process_Parameter;

#define STATIC_PROCESS_PARAMETER(ARGS) (Process_Parameter) { .args = ARGS, .fd_stdin = STDIN_FILENO, .fd_stdout = STDOUT_FILENO, .fd_stderr = STDERR_FILENO, .pipe_through = NULL, }

/**
 * @brief espera o processo terminar e retornar o stauts
 * @todo João, ainda não testado.......
 * 
 * @reference https://stackoverflow.com/questions/27306764/capturing-exit-status-code-of-child-process
 * 
 * @param pid 
 * @return int 
 */
int wait_child_process(pid_t pid)
{
  int status;

  do
  {
    waitpid(pid, &status, WUNTRACED);
  } while (!WIFEXITED(status) && !WIFSIGNALED(status));

  if (WIFEXITED(status)) return WEXITSTATUS(status);

  return -1;
}

/**
 * @brief função que faz o fork do processo pai e configura todos os "file descriptors" no processo novo.
 * 
 * @param process_parameter descrever os "file descriptors", pipe through e args
 * @param activate_opost_before_fork ativa o processamento de output, especificamente a conversão de enters em carriage return seguido de enter.
 * @return int 
 */
int launch_process(const Process_Parameter process_parameter, bool activate_opost_before_fork)
{
  if (activate_opost_before_fork) enable_oflag_opost();
  
  pid_t pid;
  pid = fork();
  int status = -1;

  if (pid == 0)
  {
    // @todo João, remover asserts assim que valores começarem a ser usados

    // @todo João, validar se o stdin pode ser setado da mesma forma que os file descriptors de output
    // @todo João, talvez incluir um cheque para não fazer o dup2, caso os atributos fd_* contiverem o valor padrão para o file descriptor 
    if (process_parameter.fd_stdin > -1 && process_parameter.fd_stdin != STDIN_FILENO)
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
      /**
       * @note como diferenciar e printar erros
       * @todo João, otimizar isso aqui
       * @reference https://stackoverflow.com/questions/503878/how-to-know-what-the-errno-means
       */
      if (errno == EACCES)
      {
        write(STDOUT_FILENO, EXPAND_STRING_REF_AND_COUNT("  Problema de permissão ao tentar executar: "));
        write(STDOUT_FILENO, process_parameter.args[0], strlen(process_parameter.args[0]));
        write(STDOUT_FILENO, EXPAND_STRING_REF_AND_COUNT("\r\n"));
      }
      else if (errno == ENOENT)
      {
        write(STDOUT_FILENO, EXPAND_STRING_REF_AND_COUNT("  Programa não encontrado: "));
        write(STDOUT_FILENO, process_parameter.args[0], strlen(process_parameter.args[0]));
        write(STDOUT_FILENO, EXPAND_STRING_REF_AND_COUNT("\r\n"));
      }
      else
      {
        write(STDOUT_FILENO, EXPAND_STRING_REF_AND_COUNT("  Internal: Processo filho não pode executar o programa alvo.\r\nMotivo: "));
        const char *error_description = strerror(errno);
        write(STDOUT_FILENO, error_description, strlen(error_description));
        write(STDOUT_FILENO, EXPAND_STRING_REF_AND_COUNT("\r\n"));
      }
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
    status = wait_child_process(pid);
  }

  // @todo João, pensar em como isso vai funcionar quando estiver fazendo o tunelamento de output
  // @todo João, quando fecha o stdin?
  // @todo João, validar se ficou tudo certo removendo a estrutura Process_Handles
  if (process_parameter.fd_stdout != STDOUT_FILENO) close(process_parameter.fd_stdout);
  if (process_parameter.fd_stderr != STDERR_FILENO) close(process_parameter.fd_stderr);

  if (activate_opost_before_fork) disable_oflag_opost();

  return status;
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
