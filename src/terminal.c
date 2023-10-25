#ifndef TERMINAL_C
#define TERMINAL_C

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>

#include "./compilation_definitions.h"
#include "./types.h"
#include "./terminal.h"

static struct termios original_config;

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


void deactivate_raw_mode()
{
  if (DEBUG_INFO) printf("[[ deactivate_raw_mode ]] :: restaurando configurações de terminal.\r\n");
  tcsetattr(STDERR_FILENO, TCSAFLUSH, &original_config);
}

void activate_raw_mode(bool set_cleanup_handler)
{
  tcgetattr(STDIN_FILENO, &original_config);

  if (set_cleanup_handler) atexit(deactivate_raw_mode);

  struct termios new_config = original_config;
  new_config.c_iflag &= ~(IXON | ICRNL); // crtl + s / crtl + q e crtl + M
  new_config.c_oflag &= ~(OPOST); // desabilitando processamento de output 
  new_config.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN); // sem echo, buffer de saída, ctrl + c / crtl + z e crtl + v
  // @note Optei por não trazer as outras flags citadas no tutorial do editor de texto
  // @reference https://viewsourcecode.org/snaptoken/kilo/02.enteringRawMode.html#miscellaneous-flags
  tcsetattr(STDERR_FILENO, TCSAFLUSH, &new_config);
}

#endif // TERMINAL_C
