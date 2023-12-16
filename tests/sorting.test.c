#include <string.h>

#include "../src/sorting.c"
#include "../src/utils.macro.h"
#include "./test-runner.c"

void test_lista_pequena()
{
  const char *arquivos[] = {
    "casa",
    "abacaxi",
    "uva",
    "cana",
  };

  quick_sort_list(arquivos, 0, SIZE_OF_ARRAY(arquivos) - 1);

  Assert(strcmp(arquivos[0], "abacaxi") == 0);
  Assert(strcmp(arquivos[1], "cana") == 0);
  Assert(strcmp(arquivos[2], "casa") == 0);
  Assert(strcmp(arquivos[3], "uva") == 0);
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
  Assert(strcmp(arquivos[0], "abacaxi") == 0);
  Assert(strcmp(arquivos[1], "cana") == 0);
  Assert(strcmp(arquivos[2], "casa") == 0);
  Assert(strcmp(arquivos[3], "uva") == 0);
  
  quick_sort_list(arquivos, 0, SIZE_OF_ARRAY(arquivos) - 1);
  Assert(strcmp(arquivos[0], "abacaxi") == 0);
  Assert(strcmp(arquivos[1], "cana") == 0);
  Assert(strcmp(arquivos[2], "casa") == 0);
  Assert(strcmp(arquivos[3], "uva") == 0);
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
 
  Assert(strcmp(arquivos[0], "a") == 0);
  Assert(strcmp(arquivos[1], "d") == 0);
  Assert(strcmp(arquivos[2], "w") == 0);
  Assert(strcmp(arquivos[3], "z") == 0);
}

void test_lista_de_um_elemento(void)
{
  const char *list[] = { "teste" };

  quick_sort_list(list, 0, 0);

  Assert(strcmp(list[0], "teste") == 0);
}

void test_lista_de_dois_elementos()
{
  const char *list[] = { "teste", "abrir" };

  quick_sort_list(list, 0, 1);
  Assert(strcmp(list[0], "abrir") == 0);
  Assert(strcmp(list[1], "teste") == 0);

  quick_sort_list(list, 0, 1);
  Assert(strcmp(list[0], "abrir") == 0);
  Assert(strcmp(list[1], "teste") == 0);
}

void test_lista_de_tres_elementos()
{
  const char *list[] = { "teste", "abrir", "casa" };

  quick_sort_list(list, 0, 2);
  Assert(strcmp(list[0], "abrir") == 0);
  Assert(strcmp(list[1], "casa") == 0);
  Assert(strcmp(list[2], "teste") == 0);

  quick_sort_list(list, 0, 2);
  Assert(strcmp(list[0], "abrir") == 0);
  Assert(strcmp(list[1], "casa") == 0);
  Assert(strcmp(list[2], "teste") == 0);
}

extern void test_suit_sorting()
{
  Register_Test(test_list_alfabeto);
  Register_Test(test_lista_de_um_elemento);
  Register_Test(test_lista_de_dois_elementos);
  Register_Test(test_lista_de_tres_elementos);
  Register_Test(test_lista_pequena);
  Register_Test(test_ordena_lista_duas_vezes_em_sequencia);
}