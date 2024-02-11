#ifndef _DEBUG_LOG_C_
#define _DEBUG_LOG_C_

#include <fcntl.h>
#include "./utils.macro.h"

static int debug_fd = -1;

void init_debug_log(void)
{
  debug_fd = open("debug_file.log", O_CREAT | O_APPEND | O_WRONLY);
}

#define Debug_Log_Static(MSG) write(debug_fd, ESRAC(MSG));
#define Debug_Log(MSG) write(debug_fd, (MSG), strlen(MSG));

void close_debug_log(void)
{
  close(debug_fd);
}

#endif // _DEBUG_LOG_C_
