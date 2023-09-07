#ifndef TERMINAL_C
#define TERMINAL_C

#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "./types.h"
#include "./terminal.h"

int get_cursor_position(int *row, int *column)
{
  // @reference https://vt100.net/docs/vt100-ug/chapter3.html#DSR
  const char *device_status_report = "\x1b[6n";
  const int length = 4;

  if (write(STDOUT_FILENO, device_status_report, length) != length) return -1;

  // @reference https://vt100.net/docs/vt100-ug/chapter3.html#CPR
  char buffer[32];
  unsigned i = 0;

  while (i < sizeof(buffer) - 1)
  {
    buffer[i] = getchar();
    if (buffer[i] == 'R') break;

    i++;
  }

  buffer[i] = '\0';

  if (buffer[0] != ESC || buffer[1] != '[') return -1;
  if (sscanf(&buffer[2], "%d;%d", row, column) != 2) return -1;

  return 0;
}

bool set_cursor_position(int row, int col)
{
  char buffer[64];
  sprintf(&buffer[0], "\x1b[%d;%dH", row, col);
  int length = strlen(&buffer[0]);

  return write(STDOUT_FILENO, &buffer, length) == length;
}

#endif // TERMINAL_C
