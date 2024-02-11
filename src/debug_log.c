#ifndef _DEBUG_LOG_C_
#define _DEBUG_LOG_C_

#include <stdio.h>
#include <fcntl.h>
#include "./utils.macro.h"

static int debug_fd = -1;
static const char *file_name = "debug_file.log";

void init_debug_log(void)
{
  debug_fd = open(file_name, O_CREAT | O_APPEND | O_WRONLY);
}

#define Debug_Log_Static(MSG) write(debug_fd, ESRAC(MSG));

/**
 * @brief Manda um linha para o log.
 */
#define Debug_Log_Line(...) { Debug_Log(__VA_ARGS__); Debug_Log_Static("\r\n") }

/**
 * @brief Escreve mensagem para o log, mas antes formata com os parâmetros caso tenha algum.
 * 
 * @note João, não tenho certeza do suporte dessa função aqui `dprintf`, mas por hora ela parece funcionar
 * bem o suficiente.
 * @link https://stackoverflow.com/questions/4246588/fprintf-like-function-for-file-descriptors-i-e-int-fd-instead-of-file-fp
 */
#define Debug_Log(...) dprintf(debug_fd, __VA_ARGS__);

void close_debug_log(void)
{
  close(debug_fd);
}

#endif // _DEBUG_LOG_C_
