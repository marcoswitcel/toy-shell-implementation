#ifndef UTILS_C
#define UTILS_C

#ifndef _DEFAULT_SOURCE
  #error "_DEFAULT_SOURCE deve estar setado para a correta compilação de algumas funções abaixo"
#endif

#include <assert.h>
#include <stdbool.h>
#include <dirent.h>

#include "./list.implementations.h"


/**
 * @brief o argv da família de comandos `exec` recebe um array de ponteiros para char terminado com null
 */
typedef char** Null_Terminated_Pointer_Array;


void print_null_terminated_pointer_array(Null_Terminated_Pointer_Array pointer_array, const char* label)
{
  char **token = pointer_array;
  while (*token != NULL)
  {
    printf("%s: [%s]\n", label, *token);
    token++;
  }
}

char* copy(const char *source)
{
  size_t size = strlen(source);
  char * copy = (char *) malloc(size + 1);
  memcpy(copy, source, size + 1);
  return copy;
}

/**
 * @brief Retorna o nome dos arquivos contidos no diretório provido.
 * 
 * @param path caminho do diretório
 * @param list_or_null lista aonde os nomes devem ser inserido caso já possua uma
 * @param include_hidden flag boleana para controlar se deve ou não inserir nome de arquivos ocultos
 * @return A referência da lista preenchida, caso o calle tenha provido a lista, a mesma será retornada aqui. List_Of_Strings* 
 */
static inline List_Of_Strings *get_all_files_for_dir(const char *path, List_Of_Strings *list_or_null, bool include_hidden)
{
  List_Of_Strings *list = list_or_null;
  if (list_or_null == NULL)
  {
    list = create_list_of_strings(128, 128);
  }

  DIR *current_dir = opendir(path);
  struct dirent *dir_entry;
  if (current_dir)
  {
    while((dir_entry = readdir(current_dir)))
    {
      if (dir_entry->d_type == DT_REG || dir_entry->d_type == DT_DIR)
      {
        if (include_hidden || dir_entry->d_name[0] != '.')
        {
          list_of_strings_push(list, copy((const char *) &dir_entry->d_name));
        }
      }
    }
    closedir(current_dir);
  }

  return list;
}

Null_Terminated_Pointer_Array convert_list_to_argv(const List_Of_Strings *list)
{
  assert(list);
  Null_Terminated_Pointer_Array args = malloc((list->index + 1) * sizeof(char *));

  if (!args)
  {
    fprintf(stderr, "Internal: Erro de alocação");
    exit(EXIT_FAILURE);
  }

  for (unsigned i = 0; i < list->index; i++)
  {
    args[i] = (char *) list->data[i];
  }
  args[list->index] = NULL;

  return args;
}

#endif // UTILS_C
