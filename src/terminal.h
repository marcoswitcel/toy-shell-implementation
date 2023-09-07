#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdbool.h>

int get_cursor_position(int *row, int *column);

bool set_cursor_position(int row, int col);

#endif // TERMINAL_H
