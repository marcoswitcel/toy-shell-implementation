#ifndef UTILS_C
#define UTILS_C

#ifndef _DEFAULT_SOURCE
  #error "_DEFAULT_SOURCE deve estar setado para a correta compilação de algumas funções abaixo"
#endif

#include <assert.h>
#include <stdbool.h>
#include <dirent.h>

#include "./list.implementations.h"

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

#endif // UTILS_C
