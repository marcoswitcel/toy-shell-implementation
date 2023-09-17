#ifndef _BUFFER_C_
#define _BUFFER_C_

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "./buffer.h"
#include "./utils.macro.h"

Buffer *create_buffer(unsigned buffer_size, unsigned grouth_by)
{
  Buffer *new_buffer = (Buffer *) malloc(sizeof(Buffer));
  char *buffer = (char *) malloc(sizeof(char) * buffer_size);
  
  new_buffer->buffer = buffer;
  new_buffer->buffer_size = buffer_size;
  new_buffer->grouth_by = grouth_by;
  new_buffer->index = 0;

  return new_buffer;
}

void destroy_buffer(Buffer *buffer)
{
  assert(buffer->buffer && "Sempre deve ser válido");

  free(buffer->buffer);
  free(buffer);
}

static inline bool buffer_ensure_enough_space_for_next_write(Buffer *buffer)
{
  if (buffer->index >= buffer->buffer_size)
  {
    buffer->buffer_size += buffer->grouth_by;
    char *new_buffer = (char *) realloc(buffer->buffer, sizeof(char) * buffer->buffer_size);
    if (new_buffer)
    {
      buffer->buffer = new_buffer;
      return true;
    }
    else
    {
      write(STDERR_FILENO, EXPAND_STRING_REF_AND_COUNT("Erro alocando memória para buffer\n"));
      exit(EXIT_FAILURE);
    }
  }

  return false;
}

bool buffer_push(Buffer *buffer, char value)
{
  buffer_ensure_enough_space_for_next_write(buffer);

  buffer->buffer[buffer->index] = value;
  buffer->index++;

  return true;
}


bool buffer_push_at(Buffer *buffer, char value, unsigned index)
{
  buffer_ensure_enough_space_for_next_write(buffer);

  memmove(&buffer->buffer[index+1], &buffer->buffer[index], buffer->index - index);

  buffer->buffer[index] = value;
  buffer->index++;

  return true;
}

bool buffer_pop(Buffer *buffer)
{
  if (buffer->index)
  {
    buffer->index--;
    return true;
  }
  return false;
}

bool buffer_pop_at(Buffer *buffer, unsigned index)
{
  if (buffer->index && index < buffer->index)
  {
    if (index < buffer->index - 1)
    {
      memmove(&buffer->buffer[index], &buffer->buffer[index + 1], buffer->index - index - 1);
    }

    buffer->index--;
    return true;
  }
  
  return false;
}

void buffer_clear(Buffer *buffer)
{
  buffer->index = 0;
}

void buffer_push_all(Buffer *buffer, const char *source, unsigned length)
{
  // @todo João, implementar isso aqui de uma forma eficiente
  for (unsigned i = 0; i < length; i++)
  {
    buffer_push(buffer, source[i]);
  }
}

/**
 * @brief adiciona um null ao final do buffer, mas não incrementa o index
 * 
 * @param buffer 
 * @return const char* 
 */
const char *buffer_ensure_null_terminated_view(Buffer *buffer)
{
  buffer_ensure_enough_space_for_next_write(buffer);

  buffer->buffer[buffer->index] = '\0';

  return &buffer->buffer[0];
}

#endif // _BUFFER_C_
