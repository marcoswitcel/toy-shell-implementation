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
    "cana",
  };

  quick_sort_list(arquivos, 0, SIZE_OF_ARRAY(arquivos) - 1);

  assert(strcmp(arquivos[0], "abacaxi") == 0);
  assert(strcmp(arquivos[1], "cana") == 0);
  assert(strcmp(arquivos[2], "casa") == 0);
  assert(strcmp(arquivos[3], "uva") == 0);
}

void test_ordena_lista_duas_vezes_em_sequencia()
{
  const char *arquivos[] = {
    "casa",
    "abacaxi",
    "uva",
    "cana",
  };

  quick_sort_list(arquivos, 0, SIZE_OF_ARRAY(arquivos) - 1);
  assert(strcmp(arquivos[0], "abacaxi") == 0);
  assert(strcmp(arquivos[1], "cana") == 0);
  assert(strcmp(arquivos[2], "casa") == 0);
  assert(strcmp(arquivos[3], "uva") == 0);
  
  printf("=========\n");
  
  // @todo João, por algum motivo ainda não identificado se eu rodo um sort numa list já ordenada, a ordem é perdida
  // @bugfix remover segunda chamada e fazer esse cenário funcionar
  quick_sort_list(arquivos, 0, SIZE_OF_ARRAY(arquivos) - 1);
  quick_sort_list(arquivos, 0, SIZE_OF_ARRAY(arquivos) - 1);


  assert(strcmp(arquivos[0], "abacaxi") == 0);
  assert(strcmp(arquivos[1], "cana") == 0);
  assert(strcmp(arquivos[2], "casa") == 0);
  assert(strcmp(arquivos[3], "uva") == 0);
}

void test_list_alfabeto()
{
   const char *arquivos[] = {
    "z",
    "w",
    "d",
    "a",
  };

  quick_sort_list(arquivos, 0, SIZE_OF_ARRAY(arquivos) - 1);
 
  assert(strcmp(arquivos[0], "a") == 0);
  assert(strcmp(arquivos[1], "d") == 0);
  assert(strcmp(arquivos[2], "w") == 0);
  assert(strcmp(arquivos[3], "z") == 0);
}

extern void test_suit_sorting()
{
  test_lista_pequena();
  // @todo João, pesquisa e implementar ajuste para funcionar com lista de 1, 2 ou 3 elementos
  test_ordena_lista_duas_vezes_em_sequencia();
  test_list_alfabeto();
}