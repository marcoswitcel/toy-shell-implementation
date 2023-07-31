#define MAKE_LIST_STRUCT_IMPLEMENTATION(STRUCT_NAME, TYPE) \
typedef struct STRUCT_NAME                                 \
{                                                          \
  unsigned internal_memory_size; \
  unsigned grouth_by;            \
  unsigned index;                \
  TYPE *data;                    \
} STRUCT_NAME;

#define MAKE_LIST_FUNCTIONS_IMPLEMENTATION(STRUCT_NAME, STRUCT_NAME_LOWER_CASE, TYPE)             \
STRUCT_NAME *create_ ## STRUCT_NAME_LOWER_CASE(unsigned internal_memory_size, unsigned grouth_by) \
{                                                                                                 \
  STRUCT_NAME *list = (STRUCT_NAME *) malloc(sizeof(STRUCT_NAME));                                \
  TYPE *data = (TYPE *) malloc(sizeof(TYPE) * internal_memory_size);                              \
  list->data = data;                                                                              \
  list->internal_memory_size = internal_memory_size;                                              \
  list->grouth_by = grouth_by;                                                                    \
  list->index = 0;                                                                                \
  return list;                                                                                    \
}                                                                                                 \
void destroy_ ## NAME(STRUCT_NAME *list)                                                          \
{                                                                                                 \
  assert(list->data && "Sempre deve ser válido"); \
                                                                                                  \
  free(list->data);\
  free(list);\
}\
\
static inline bool list_ensure_enough_space(STRUCT_NAME *list)\
{\
  if (list->index >= list->internal_memory_size)\
  {\
    list->internal_memory_size += list->grouth_by;\
    TYPE *data = (TYPE *) realloc(list->data, sizeof(TYPE) * list->internal_memory_size);\
    if (data)\
    {\
      list->data = data;\
      return true;\
    }\
    else\
    {\
      printf("Erro alocando memória para buffer\n");\
    }\
  }\
\
  return false;\
}\
\
bool STRUCT_NAME_LOWER_CASE ## _push(STRUCT_NAME *list, TYPE value)\
{\
  list_ensure_enough_space(list);\
\
  list->data[list->index] = value;\
  list->index++;\
\
  return true;\
}\
\
bool STRUCT_NAME_LOWER_CASE ## pop(STRUCT_NAME *list)\
{\
  if (list->index)\
  {\
    list->index--;\
    return true;\
  }\
  return false;\
}

#define MAKE_LIST_IMPLEMENTATION(STRUCT_NAME, STRUCT_NAME_LOWER_CASE, TYPE) \
  MAKE_LIST_STRUCT_IMPLEMENTATION(STRUCT_NAME, TYPE) \
  MAKE_LIST_FUNCTIONS_IMPLEMENTATION(STRUCT_NAME, STRUCT_NAME_LOWER_CASE, TYPE)
