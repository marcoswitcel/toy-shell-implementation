#ifndef UTILS_C
#define UTILS_C

#ifndef _DEFAULT_SOURCE
  #error "_DEFAULT_SOURCE deve estar setado para a correta compilação de algumas funções abaixo"
#endif

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>

#include "./list.implementations.h"
#include "./utils.macro.h"
#include "./tokens.h"

/**
 * @brief o argv da família de comandos `exec` recebe um array de ponteiros para char terminado com null
 */
typedef char** Null_Terminated_Pointer_Array;


void print_null_terminated_pointer_array(Null_Terminated_Pointer_Array pointer_array, const char* label)
{
  char **token = pointer_array;
  while (*token != NULL)
  {
    printf("%s: [%s]\r\n", label, *token);
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

void release_cstring_from_null_terminated_pointer_array(Null_Terminated_Pointer_Array pointer_array)
{
  assert(pointer_array);

  while (*pointer_array != NULL)
  {
    /**
     * @note Em teoria eu troco todos os símbolos de query antes de chamar essa função, então ela nunca deveria 
     * tentar fazer o release da memória de um símbolo, porém, por precaução eu deixarei o 'if' de guarda abaixo 
     * para proteger as versões de release. Em dev observarei atentamente se cometo muitos erros e esse assert
     * costuma disparar conforme faço refatorações.
     */
    assert(*pointer_array != static_query_last_status_code_symbol);

    // @note Idealmente esse 'if' não precisaria existir aqui, poderia tratar os símbolos fixos de outra forma,
    // mas por hora o conceito de argumentos que precisam ser substituídos antes da execução existe e os símbolos
    // são strings constantes que preciso considerar antes de fazer o release.
    if (*pointer_array == static_query_last_status_code_symbol) 
    {
      pointer_array++;
      continue;
    }

    FREE_AND_NULLIFY(*pointer_array);
    pointer_array++;
  }
}

// @reference sugestão vinda do stackoverflow, conforme for evoluindo esse código revisar esse VT escape code
void erase_line()
{
  write(STDOUT_FILENO, EXPAND_STRING_REF_AND_COUNT("\33[2K\r"));
}

void clear_terminal()
{
  // @reference http://vt100.net/docs/vt100-ug/chapter3.html#ED
  write(STDOUT_FILENO, EXPAND_STRING_REF_AND_COUNT("\x1b[2J"));
  // @reference http://vt100.net/docs/vt100-ug/chapter3.html#CUP
  write(STDOUT_FILENO, EXPAND_STRING_REF_AND_COUNT("\x1b[H"));
}

void emmit_ring_bell()
{
  // o char BEL é o valor decimal 7 e hex 07
  write(STDOUT_FILENO, "\x7", 1);
}

// @todo João, implementar o algoritmo para criar a string
// @todo João, testar e validar contra implementações de outras pessoas
char * int_to_cstring(int number)
{
  int n_chars = 1;
  int copy = number;

  while ((copy /= 10) > 0) n_chars++;

  char *number_cstring = (char *) malloc(n_chars + 1);

  // @todo João, estudar e trocar a implementação por um algoritmo customizado
  sprintf(number_cstring, "%d", number);

  number_cstring[n_chars] = '\0';

  return number_cstring;
}

#endif // UTILS_C
