#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdbool.h>

int get_cursor_position(int *row, int *column);

bool set_cursor_position(int row, int col);

void deactivate_raw_mode();

void activate_raw_mode(bool set_cleanup_handler);

#endif // TERMINAL_H
