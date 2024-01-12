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

/**
 * @brief Utilitário geral para facilitar a cópia de strings
 * 
 * @note Acredito para otimizar as alocações e facilitar a liberação da memória,
 * talvez seria interessante fazer alguma versão que usa um `Bump Alocator` e 
 * libera a memória toda de uma vez ao final do siclo de processamento.
 * 
 * @param source String a ser copiada
 * @return char* 
 */
char *copy(const char *source)
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

/**
 * @brief Função que libera a memória da cstrings individuais referenciadas pelo pointer_array
 * 
 * @param pointer_array 
 */
void release_cstring_from_null_terminated_pointer_array(Null_Terminated_Pointer_Array pointer_array)
{
  assert(pointer_array);

  while (*pointer_array != NULL)
  {
    /**
     * @note Essa função não deveria mais receber símbolos, porém fica o assert para eventuais erros de uso
     */
    assert(*pointer_array != static_query_last_status_code_symbol);
    assert(*pointer_array != static_globbing_symbol);

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

/**
 * @brief Gera um string com a representação textual do inteiro
 * 
 * @note João, não seria complexo adicionar uma versão que suporte bases
 * alternativas como base: 2, 16 e etc... Pode-se usar o código e os links de
 * referência abaixo. 
 * 
 * Referências
 * @link https://daveparillo.github.io/cisc187-reader/recursion/to_string.html
 * @link https://panda.ime.usp.br/panda/static/pythonds_pt/04-Recursao/04-int2str.html
 * 
 * @param number 
 * @return char* 
 */
char *int_to_cstring(int number)
{
  const int base = 10;
  bool is_negative_number = number < 0;
  int n_chars = 1 + is_negative_number;
  int copy = number;
  // @note a inversão do sinal poderia ser feita com uma negação e uma adição "copy = ~copy + 1;"
  if (is_negative_number) copy *= -1;
  
  while ((copy /= 10) > 0) n_chars++;

  char *number_cstring = (char *) malloc(n_chars + 1);

  int currentNumber = number;
  if (is_negative_number) currentNumber *= -1;
  
  for (int i = n_chars; i--;)
  {
    if (currentNumber < base)
    {
      number_cstring[i] = '0' + currentNumber;
      break;
    }

    int remainder = currentNumber % base;
    number_cstring[i] = '0' + remainder;
    currentNumber /= base;
  }
  
  if (is_negative_number)
  {
    number_cstring[0] = '-';
  }
  number_cstring[n_chars] = '\0';

  return number_cstring;
}

#endif // UTILS_C
