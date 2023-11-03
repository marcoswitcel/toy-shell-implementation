#ifndef __SORTING__
#define __SORTING__

#include "./utils.macro.h"
#include "string.h"

static signed partition(const char **list, signed begin, signed end)
{
  const char* pivot = list[end];
  signed i = begin - 1;

  for (signed j = begin; j <= end - 1; j++)
  {
    int diff = strcmp(list[j], pivot);
    // printf("[%s] [%s] r: %d\r\n", list[j], pivot, diff );

    if (diff <= 0)
    {
      i++;
      SWAP(list[i], list[j]);
    }
  }

  i++;
  SWAP(list[i], list[end]);

  return i;
}

/**
 * @brief Função que faz a ordenação de listas de strings de forma lexicográfica
 * 
 * @todo João, desenvolver testes para essa função, corrigir possíveis bugs
 * @todo João, bug aonde chamando a função de quick_sort duas vezes seguida causa leitura de endereço inválido
 * 
 * @note ordem dos arquivos: a.txt aa.txt buffer.o help.txt help1.txt jaininha.txt main temp.txt aja.txt teste testeteeeee.txt tests teste.txt
 * 
 * @link https://en.wikipedia.org/wiki/Quicksort
 * @link https://www.baeldung.com/java-quicksort
 * @link https://www.programiz.com/c-programming/library-function/string.h/strcmp
 * 
 * @param list 
 * @param begin 
 * @param end 
 */
void quick_sort_list(const char **list, signed begin, signed end)
{
  if (begin < end && begin >= 0)
  {
    // signed diff = (end - begin) + 1;

    // if (diff == 1)
    // {
    //   return;
    // }
    // else if (diff == 2)
    // {
    //   if (strcmp(list[begin], list[end]) > 0) SWAP(list[begin], list[end]);
      
    //   return;
    // }
    // else if (diff == 3)
    // {
    //   if (strcmp(list[begin], list[begin + 1]) > 0) SWAP(list[begin], list[begin + 1]);
    //   if (strcmp(list[begin + 1], list[end]) > 0) SWAP(list[begin + 1], list[end]);
    //   if (strcmp(list[begin], list[begin + 1]) > 0) SWAP(list[begin], list[begin + 1]);
    //   return;
    // }

    signed partitionIndex = partition(list, begin, end);

    quick_sort_list(list, begin, partitionIndex - 1);
    quick_sort_list(list, partitionIndex + 1, end);
  }
}

#endif // __SORTING__
