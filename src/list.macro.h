#ifndef LIST_MACRO_H
#define LIST_MACRO_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

/**
 * @brief Macro que gera a estrutura da lista e o typedef
 * @param STRUCT_NAME Nome da `struct` que será criada
 * @param TYPE Tipo de dados que serão contidos na lista
 */
#define MAKE_LIST_STRUCT_IMPLEMENTATION(STRUCT_NAME, TYPE)                                        \
typedef struct STRUCT_NAME                                                                        \
{                                                                                                 \
  unsigned internal_memory_size;                                                                  \
  unsigned grouth_by;                                                                             \
  unsigned index;                                                                                 \
  TYPE *data;                                                                                     \
} STRUCT_NAME;

/**
 * @brief Macro que gera todo o código necessário para realizar operações na lista
 * @param STRUCT_NAME Nome da `struct` que será criada
 * @param STRUCT_NAME_LOWER_CASE Nome usado como prefixo para as funções da estrutura da lista
 * @param TYPE Tipo de dados que serão contidos na lista
 */
#define MAKE_LIST_FUNCTIONS_IMPLEMENTATION(STRUCT_NAME, STRUCT_NAME_LOWER_CASE, TYPE)             \
STRUCT_NAME *create_##STRUCT_NAME_LOWER_CASE(unsigned internal_memory_size, unsigned grouth_by)   \
{                                                                                                 \
  STRUCT_NAME *list = (STRUCT_NAME *) malloc(sizeof(STRUCT_NAME));                                \
  TYPE *data = (TYPE *) malloc(sizeof(TYPE) * internal_memory_size);                              \
  list->data = data;                                                                              \
  list->internal_memory_size = internal_memory_size;                                              \
  list->grouth_by = grouth_by;                                                                    \
  list->index = 0;                                                                                \
  return list;                                                                                    \
}                                                                                                 \
                                                                                                  \
void destroy_##STRUCT_NAME_LOWER_CASE(STRUCT_NAME *list)                                          \
{                                                                                                 \
  assert(list->data && "Sempre deve ser válido");                                                 \
                                                                                                  \
  free(list->data);                                                                               \
  free(list);                                                                                     \
}                                                                                                 \
                                                                                                  \
static inline bool STRUCT_NAME_LOWER_CASE##_reallocate(STRUCT_NAME *list)                         \
{                                                                                                 \
  TYPE *data = (TYPE *) realloc(list->data, sizeof(TYPE) * list->internal_memory_size);           \
  if (data)                                                                                       \
  {                                                                                               \
    list->data = data;                                                                            \
    return true;                                                                                  \
  }                                                                                               \
  else                                                                                            \
  {                                                                                               \
    printf("Erro alocando memória para buffer\r\n");                                              \
    exit(EXIT_FAILURE);                                                                           \
  }                                                                                               \
  return false;                                                                                   \
}                                                                                                 \
                                                                                                  \
static inline bool STRUCT_NAME_LOWER_CASE##_ensure_enough_space_for_next_n_writes(                \
  STRUCT_NAME *list, unsigned n_writes)                                                           \
{                                                                                                 \
  unsigned index_after_writes = list->index + n_writes;                                           \
                                                                                                  \
  if (index_after_writes > list->internal_memory_size)                                            \
  {                                                                                               \
    while (list->internal_memory_size < index_after_writes)                                       \
    {                                                                                             \
      list->internal_memory_size += list->grouth_by;                                              \
    }                                                                                             \
                                                                                                  \
    return STRUCT_NAME_LOWER_CASE##_reallocate(list);                                             \
  }                                                                                               \
                                                                                                  \
  return false;                                                                                   \
}                                                                                                 \
                                                                                                  \
static inline bool STRUCT_NAME_LOWER_CASE##_ensure_enough_space(STRUCT_NAME *list)                \
{                                                                                                 \
  if (list->index >= list->internal_memory_size)                                                  \
  {                                                                                               \
    list->internal_memory_size += list->grouth_by;                                                \
    TYPE *data = (TYPE *) realloc(list->data, sizeof(TYPE) * list->internal_memory_size);         \
    if (data)                                                                                     \
    {                                                                                             \
      list->data = data;                                                                          \
      return true;                                                                                \
    }                                                                                             \
    else                                                                                          \
    {                                                                                             \
      printf("Erro alocando memória para buffer\n");                                              \
    }                                                                                             \
  }                                                                                               \
                                                                                                  \
  return false;                                                                                   \
}                                                                                                 \
                                                                                                  \
bool STRUCT_NAME_LOWER_CASE##_push(STRUCT_NAME *list, TYPE value)                                 \
{                                                                                                 \
  STRUCT_NAME_LOWER_CASE##_ensure_enough_space(list);                                             \
                                                                                                  \
  list->data[list->index] = value;                                                                \
  list->index++;                                                                                  \
                                                                                                  \
  return true;                                                                                    \
}                                                                                                 \
                                                                                                  \
void STRUCT_NAME_LOWER_CASE##_push_all(STRUCT_NAME *list, const TYPE *source, unsigned length)    \
{                                                                                                 \
  STRUCT_NAME_LOWER_CASE##_ensure_enough_space_for_next_n_writes(list, length);                   \
                                                                                                  \
  for (unsigned i = 0; i < length; i++)                                                           \
  {                                                                                               \
    list->data[list->index] = source[i];                                                          \
    list->index++;                                                                                \
  }                                                                                               \
}                                                                                                 \
                                                                                                  \
bool STRUCT_NAME_LOWER_CASE##_pop(STRUCT_NAME *list)                                              \
{                                                                                                 \
  if (list->index)                                                                                \
  {                                                                                               \
    list->index--;                                                                                \
    return true;                                                                                  \
  }                                                                                               \
  return false;                                                                                   \
}                                                                                                 \
                                                                                                  \
bool STRUCT_NAME_LOWER_CASE##_pop_at(STRUCT_NAME *list, unsigned index)                           \
{                                                                                                 \
  if (list->index && index < list->index)                                                         \
  {                                                                                               \
    if (index < list->index - 1)                                                                  \
    {                                                                                             \
      unsigned number_of_bytes = (list->index - index - 1) * sizeof(TYPE);                        \
      memmove(&list->data[index], &list->data[index + 1], number_of_bytes);                       \
    }                                                                                             \
                                                                                                  \
    list->index--;                                                                                \
    return true;                                                                                  \
  }                                                                                               \
                                                                                                  \
  return false;                                                                                   \
}

/**
 * @brief Macro que gera todo o código necessário para ter uma lista funcional
 * @param STRUCT_NAME Nome da `struct` que será criada
 * @param STRUCT_NAME_LOWER_CASE Nome usado como prefixo para as funções da estrutura da lista
 * @param TYPE Tipo de dados que serão contidos na lista
 */
#define MAKE_LIST_IMPLEMENTATION(STRUCT_NAME, STRUCT_NAME_LOWER_CASE, TYPE)                       \
  MAKE_LIST_STRUCT_IMPLEMENTATION(STRUCT_NAME, TYPE)                                              \
  MAKE_LIST_FUNCTIONS_IMPLEMENTATION(STRUCT_NAME, STRUCT_NAME_LOWER_CASE, TYPE)

#endif // LIST_MACRO_H
