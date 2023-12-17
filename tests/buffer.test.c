#include <string.h>

#include "../src/types.h"
#include "./test-runner.h"

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

  Assert(buffer->index == 0);
  buffer_pop(buffer);
  Assert(buffer->index == 0);

  buffer_push(buffer, test_name[0]);
  Assert(buffer->index == 1);
  buffer_push(buffer, test_name[1]);
  Assert(buffer->index == 2);

  buffer_push(buffer, test_name[2]);
  buffer_pop(buffer);

  Assert(buffer->index == 2);

  buffer_push(buffer, test_name[2]);

  Assert(buffer->buffer[0] && buffer->buffer[1]);

  Assert(buffer->buffer[0] == test_name[0]);
  Assert(buffer->buffer[1] == test_name[1]);
  Assert(buffer->buffer[2] == test_name[2]);

  Assert(buffer->buffer[3] != '\0');

  Assert(strcmp(test_name, buffer_ensure_null_terminated_view(buffer)) == 0);

  Assert(buffer->buffer[3] == '\0');
}

static void test_buffer_push_at(void)
{
  Buffer *buffer = create_buffer(LINE_BUFFER_SIZE, LINE_BUFFER_SIZE);
  put_garbage_on_buffer(buffer);
  char test_name[] = "Tux";

  Assert(buffer->index == 0);
  buffer_pop(buffer);
  Assert(buffer->index == 0);


  buffer_push_at(buffer, test_name[0], 0);
  Assert(buffer->index == 1);
  buffer_push_at(buffer, test_name[1], 0);
  Assert(buffer->index == 2);

  buffer_push_at(buffer, test_name[2], 0);
  buffer_pop(buffer);

  Assert(buffer->index == 2);

  buffer_push_at(buffer, test_name[2], 0);

  Assert(buffer->buffer[0] && buffer->buffer[1]);

  Assert(buffer->buffer[0] == test_name[2]); // x
  Assert(buffer->buffer[1] == test_name[2]); // x
  Assert(buffer->buffer[2] == test_name[1]); // u

  Assert(buffer->buffer[3] != '\0');
  Assert(buffer->index == 3);

  Assert(strcmp("xxu", buffer_ensure_null_terminated_view(buffer)) == 0);

  Assert(buffer->index == 3);
  Assert(buffer->buffer[3] == '\0');

  buffer_pop(buffer);
  Assert(buffer->index == 2);

  buffer_push_at(buffer, test_name[2], 0);
  Assert(buffer->index == 3);

  buffer_push_at(buffer, 'T', 1);
  Assert(buffer->index == 4);
  Assert(strcmp("xTxx", buffer_ensure_null_terminated_view(buffer)) == 0);
  Assert(buffer->index == 4);
}

static void test_buffer_remove_at(void)
{
  Buffer *buffer = create_buffer(LINE_BUFFER_SIZE, LINE_BUFFER_SIZE);
  put_garbage_on_buffer(buffer);
  char t = 't';
  char u = 'u';
  char x = 'x';
  char texto[] = { t, u, x, '\0' };

  Assert(buffer->index == 0);
  buffer_push(buffer, t);
  Assert(buffer->buffer[0] == t);
  Assert(buffer->index == 1);
  buffer_push(buffer, u);
  Assert(buffer->index == 2);
  Assert(buffer->buffer[1] == u);
  buffer_push(buffer, x);
  Assert(buffer->index == 3);
  Assert(buffer->buffer[2] == x);

  Assert(strcmp(texto, buffer_ensure_null_terminated_view(buffer)) == 0);
  buffer_pop_at(buffer, 1);
  Assert(buffer->index == 2);
  Assert(buffer->buffer[1] == x);
  Assert(strcmp("tx", buffer_ensure_null_terminated_view(buffer)) == 0);


  buffer_push_at(buffer, u, 1);
  Assert(buffer->index == 3);
  Assert(buffer->buffer[1] == u);
  Assert(strcmp(texto, buffer_ensure_null_terminated_view(buffer)) == 0);
}

static void test_buffer_clear(void)
{
  Buffer *buffer = create_buffer(LINE_BUFFER_SIZE, LINE_BUFFER_SIZE);
  put_garbage_on_buffer(buffer);
  char t = 't';
  char u = 'u';
  char x = 'x';
  char texto[] = { t, u, x, '\0' };

  Assert(buffer->index == 0);

  buffer_clear(buffer);
  Assert(buffer->index == 0);

  buffer_push(buffer, t);
  Assert(buffer->index == 1);
  buffer_push(buffer, u);
  Assert(buffer->index == 2);
  buffer_push(buffer, x);
  Assert(buffer->index == 3);

  Assert(strcmp(texto, buffer_ensure_null_terminated_view(buffer)) == 0);

  buffer_clear(buffer);
  Assert(buffer->index == 0);

  Assert(strcmp("", buffer_ensure_null_terminated_view(buffer)) == 0);
}

static void test_buffer_push_all(void)
{
  Buffer *buffer = create_buffer(LINE_BUFFER_SIZE, LINE_BUFFER_SIZE);
  put_garbage_on_buffer(buffer);
  char texto[] = "texto para adicionar";
  unsigned length = strlen(texto);

  Assert(buffer->index == 0);

  buffer_push_all(buffer, (const char *) &texto, length);
  Assert(buffer->index == length);

  Assert(strcmp(texto, buffer_ensure_null_terminated_view(buffer)) == 0);

  buffer_push_all(buffer, (const char *) &texto, length);
  Assert(buffer->index == length * 2);

  buffer_clear(buffer);
  Assert(buffer->index == 0);

  buffer_push_all(buffer, (const char *) &texto, length);
  Assert(buffer->index == length);
  
  Assert(strcmp(texto, buffer_ensure_null_terminated_view(buffer)) == 0);
}

extern void test_suit_buffer()
{
  Register_Test(test_buffer_implementation);
  Register_Test(test_buffer_push_at);
  Register_Test(test_buffer_remove_at);
  Register_Test(test_buffer_clear);
  Register_Test(test_buffer_push_all);
}