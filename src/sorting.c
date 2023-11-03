#ifndef __SORTING__
#define __SORTING__

#include "./utils.macro.h"
#include "string.h"

static signed partition(const char **list, signed begin, signed end)
{
  // Pra garatir que os cálculos sempre resultam em índices válidos
  assert(begin > -1);
  assert(end > -1);

  const char* pivot = list[end];
  signed i = begin - 1;

  for (signed j = begin; j <= end - 1; j++)
  {
    int diff = strcmp(list[j], pivot);

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
 * @param beginIndex 
 * @param endIndex 
 */
void quick_sort_list(const char **list, signed beginIndex, signed endIndex)
{
  if (beginIndex < endIndex && beginIndex >= 0)
  {
    signed partitionIndex = partition(list, beginIndex, endIndex);

    quick_sort_list(list, beginIndex, partitionIndex - 1);
    quick_sort_list(list, partitionIndex + 1, endIndex);
  }
}

#endif // __SORTING__
