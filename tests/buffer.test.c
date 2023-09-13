#include <assert.h>
#include <string.h>

#include "../src/types.h"

#define LINE_BUFFER_SIZE 1024

static void put_garbage_on_buffer(Buffer *buffer)
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

static void test_buffer_push_at(void)
{
  Buffer *buffer = create_buffer(LINE_BUFFER_SIZE, LINE_BUFFER_SIZE);
  put_garbage_on_buffer(buffer);
  char test_name[] = "Tux";

  assert(buffer->index == 0);
  buffer_pop(buffer);
  assert(buffer->index == 0);


  buffer_push_at(buffer, test_name[0], 0);
  assert(buffer->index == 1);
  buffer_push_at(buffer, test_name[1], 0);
  assert(buffer->index == 2);

  buffer_push_at(buffer, test_name[2], 0);
  buffer_pop(buffer);

  assert(buffer->index == 2);

  buffer_push_at(buffer, test_name[2], 0);

  assert(buffer->buffer[0] && buffer->buffer[1]);

  assert(buffer->buffer[0] == test_name[2]); // x
  assert(buffer->buffer[1] == test_name[2]); // x
  assert(buffer->buffer[2] == test_name[1]); // u

  assert(buffer->buffer[3] != '\0');
  assert(buffer->index == 3);

  assert(strcmp("xxu", buffer_ensure_null_terminated_view(buffer)) == 0);

  assert(buffer->index == 3);
  assert(buffer->buffer[3] == '\0');

  buffer_pop(buffer);
  assert(buffer->index == 2);

  buffer_push_at(buffer, test_name[2], 0);
  assert(buffer->index == 3);

  buffer_push_at(buffer, 'T', 1);
  assert(buffer->index == 4);
  assert(strcmp("xTxx", buffer_ensure_null_terminated_view(buffer)) == 0);
  assert(buffer->index == 4);
}

static void test_buffer_remove_at(void)
{
  Buffer *buffer = create_buffer(LINE_BUFFER_SIZE, LINE_BUFFER_SIZE);
  put_garbage_on_buffer(buffer);
  char t = 't';
  char u = 'u';
  char x = 'x';
  char texto[] = { t, u, x, '\0' };

  assert(buffer->index == 0);
  buffer_push(buffer, t);
  assert(buffer->buffer[0] == t);
  assert(buffer->index == 1);
  buffer_push(buffer, u);
  assert(buffer->index == 2);
  assert(buffer->buffer[1] == u);
  buffer_push(buffer, x);
  assert(buffer->index == 3);
  assert(buffer->buffer[2] == x);

  assert(strcmp(texto, buffer_ensure_null_terminated_view(buffer)) == 0);
  buffer_pop_at(buffer, 1);
  assert(buffer->index == 2);
  assert(buffer->buffer[1] == x);
  assert(strcmp("tx", buffer_ensure_null_terminated_view(buffer)) == 0);


  buffer_push_at(buffer, u, 1);
  assert(buffer->index == 3);
  assert(buffer->buffer[1] == u);
  assert(strcmp(texto, buffer_ensure_null_terminated_view(buffer)) == 0);
}

static void test_buffer_clear(void)
{
  Buffer *buffer = create_buffer(LINE_BUFFER_SIZE, LINE_BUFFER_SIZE);
  put_garbage_on_buffer(buffer);
  char t = 't';
  char u = 'u';
  char x = 'x';
  char texto[] = { t, u, x, '\0' };

  assert(buffer->index == 0);

  buffer_clear(buffer);
  assert(buffer->index == 0);

  buffer_push(buffer, t);
  assert(buffer->index == 1);
  buffer_push(buffer, u);
  assert(buffer->index == 2);
  buffer_push(buffer, x);
  assert(buffer->index == 3);

  assert(strcmp(texto, buffer_ensure_null_terminated_view(buffer)) == 0);

  buffer_clear(buffer);
  assert(buffer->index == 0);

  assert(strcmp("", buffer_ensure_null_terminated_view(buffer)) == 0);
}

extern void test_suit_buffer()
{
  test_buffer_implementation();
  test_buffer_push_at();
  test_buffer_remove_at();
  test_buffer_clear();
}