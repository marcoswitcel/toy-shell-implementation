#ifndef TERMINAL_C
#define TERMINAL_C

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>

#include "./compilation_definitions.h"
#include "./types.h"
#include "./terminal.h"
#include "./debug_log.c"

/**
 * @brief mascara a letra com o padrão binário 00011111 (decimal 31 e hexadecimal 0x1f)
 * esse é o padrão que o terminal aplica a combinação ctrl + key, poderia ser por exemplo:
 * ctrl+q ficaria 17, 'q' mascarado por 0x1f é igual à 17.
 * 
 * @reference https://viewsourcecode.org/snaptoken/kilo/03.rawInputAndOutput.html#press-ctrl-q-to-quit
 * @reference https://stackoverflow.com/questions/76901856/getting-the-status-of-the-ctrl-key-when-reading-console-input-in-java-on-unix
 */
#define CTRL_KEY(LETTER) ((LETTER) & 0x1f)


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
  if (DEBUG_INFO) Debug_Log_Line("restaurando configurações de terminal.");
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
  new_config.c_cc[VMIN] = 0;
  new_config.c_cc[VTIME] = 1;

  tcsetattr(STDERR_FILENO, TCSAFLUSH, &new_config);
}

// @note João, considerei usar a configuração orignal para evitar o "tcget" mas seria menos flexível a função
// então como não tenho certeza vou deixar assim por hora
void enable_oflag_opost()
{
  struct termios current_config;
  tcgetattr(STDIN_FILENO, &current_config);

  current_config.c_oflag |= OPOST; // habilita processamento de output 

  tcsetattr(STDERR_FILENO, TCSAFLUSH, &current_config);
}

// @note João, considerei usar a configuração orignal para evitar o "tcget" mas seria menos flexível a função
// então como não tenho certeza vou deixar assim por hora
void disable_oflag_opost()
{
  struct termios current_config;
  tcgetattr(STDIN_FILENO, &current_config);

  current_config.c_oflag &= ~OPOST; // desabilita processamento de output 

  tcsetattr(STDERR_FILENO, TCSAFLUSH, &current_config);
}

#endif // TERMINAL_C
