#include <assert.h>
#include <string.h>

#include "../src/sorting.c"
#include "../src/utils.macro.h"
#include "string.h"

void test_lista_pequena()
{
  const char *arquivos[] = {
    "casa",
    "abacaxi",
    "uva",
  };

  assert(strcmp(arquivos[0], "casa") == 0);
  quick_sort_list(arquivos, 0, SIZE_OF_ARRAY(arquivos) - 1);
  assert(strcmp(arquivos[0], "abacaxi") == 0);
}

extern void test_suit_sorting()
{
  // @todo João, implementar

  // @todo João, não passando
  // test_lista_pequena();
}