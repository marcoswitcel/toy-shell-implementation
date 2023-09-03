#include <assert.h>

#include "../src/types.h"

#define LINE_BUFFER_SIZE 1024

void put_garbage_on_buffer(Buffer *buffer)
{
  for (unsigned i = 0; i < buffer->buffer_size; i++)
  {
    buffer->buffer[i] = (char) 0xCC;
  }
}

static void test_buffer_implementation(void)
{
  Buffer *buffer = create_buffer(LINE_BUFFER_SIZE, LINE_BUFFER_SIZE);
  put_garbage_on_buffer(buffer);
  char test_name[] = "Tux";

  assert(buffer->index == 0);
  buffer_pop(buffer);
  assert(buffer->index == 0);


  buffer_push(buffer, test_name[0]);
  assert(buffer->index == 1);
  buffer_push(buffer, test_name[1]);
  assert(buffer->index == 2);

  buffer_push(buffer, test_name[2]);
  buffer_pop(buffer);

  assert(buffer->index == 2);

  buffer_push(buffer, test_name[2]);

  assert(buffer->buffer[0] && buffer->buffer[1]);

  assert(buffer->buffer[0] == test_name[0]);
  assert(buffer->buffer[1] == test_name[1]);
  assert(buffer->buffer[2] == test_name[2]);

  assert(buffer->buffer[3] != '\0');

  assert(strcmp(test_name, buffer_ensure_null_terminated_view(buffer)) == 0);

  assert(buffer->buffer[3] == '\0');
}


extern void test_suit_buffer()
{
  test_buffer_implementation();
}