#ifndef _BUFFER_H_
#define _BUFFER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

typedef struct Buffer {
  unsigned buffer_size;
  unsigned grouth_by;
  unsigned index;
  char *buffer;
} Buffer;

Buffer *create_buffer(unsigned buffer_size, unsigned grouth_by);

void destroy_buffer(Buffer *buffer);

bool buffer_push(Buffer *buffer, char value);

bool buffer_push_at(Buffer *buffer, char value, unsigned index);

bool buffer_pop(Buffer *buffer);

bool buffer_pop_at(Buffer *buffer, unsigned index);

const char *buffer_ensure_null_terminated_view(Buffer *buffer);

#endif // _BUFFER_H_