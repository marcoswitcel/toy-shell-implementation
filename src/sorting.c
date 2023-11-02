#ifndef __SORTING__
#define __SORTING__

#include "./list.implementations.h"
#include "./utils.macro.h"
#include "string.h"

static unsigned partition(List_Of_Strings *list, unsigned begin, unsigned end)
{
  const char* pivot = list->data[end];
  unsigned i = begin - 1;

  for (unsigned j = begin; j < end - 1; j++)
  {
    int diff = strcmp(list->data[j], pivot);
    // printf("[%s] [%s] r: %d\r\n", list->data[j], pivot, diff );

    if (diff <= 0)
    {
      i++;
      SWAP(list->data[i], list->data[j]);
    }
  }

  i++;
  SWAP(list->data[i], list->data[end]);

  return i;
}

/**
 * @brief Função que faz a ordenação de listas de strings de forma lexicográfica
 * 
 * @todo João, desenvolver testes para essa função, corrigir possíveis bugs
 * 
 * @link https://en.wikipedia.org/wiki/Quicksort
 * @link https://www.baeldung.com/java-quicksort
 * @link https://www.programiz.com/c-programming/library-function/string.h/strcmp
 * 
 * @param list 
 * @param begin 
 * @param end 
 */
void quick_sort_list(List_Of_Strings *list, unsigned begin, unsigned end)
{
  if (begin < end)
  {
    unsigned partitionIndex = partition(list, begin, end);

    quick_sort_list(list, begin, partitionIndex - 1);
    quick_sort_list(list, partitionIndex + 1, end);
  }
}

#endif // __SORTING__
