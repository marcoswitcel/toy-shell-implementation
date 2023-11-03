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

void test_lista_de_um_elemento(void)
{
  const char *list[] = { "teste" };

  quick_sort_list(list, 0, 0);

  assert(strcmp(list[0], "teste") == 0);
}

void test_lista_de_dois_elementos()
{
  const char *list[] = { "teste", "abrir" };

  quick_sort_list(list, 0, 1);
  assert(strcmp(list[0], "abrir") == 0);
  assert(strcmp(list[1], "teste") == 0);

  quick_sort_list(list, 0, 1);
  assert(strcmp(list[0], "abrir") == 0);
  assert(strcmp(list[1], "teste") == 0);
}

void test_lista_de_tres_elementos()
{
  const char *list[] = { "teste", "abrir", "casa" };

  quick_sort_list(list, 0, 2);
  assert(strcmp(list[0], "abrir") == 0);
  assert(strcmp(list[1], "casa") == 0);
  assert(strcmp(list[2], "teste") == 0);

  quick_sort_list(list, 0, 2);
  assert(strcmp(list[0], "abrir") == 0);
  assert(strcmp(list[1], "casa") == 0);
  assert(strcmp(list[2], "teste") == 0);
}

extern void test_suit_sorting()
{
  test_list_alfabeto();
  test_lista_de_um_elemento();
  test_lista_de_dois_elementos();
  test_lista_de_tres_elementos();
  test_lista_pequena();
  test_ordena_lista_duas_vezes_em_sequencia();
}