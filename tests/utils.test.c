#include <assert.h>
#include <string.h>

#include "../src/utils.c"

void test_int_to_cstring(void)
{
  const char *representation;
  
  representation = int_to_cstring(0);
  assert(strcmp(representation, "0") == 0);

  representation = int_to_cstring(1);
  assert(strcmp(representation, "1") == 0);

  representation = int_to_cstring(2);
  assert(strcmp(representation, "2") == 0);

  representation = int_to_cstring(3);
  assert(strcmp(representation, "3") == 0);

  representation = int_to_cstring(4);
  assert(strcmp(representation, "4") == 0);

  representation = int_to_cstring(5);
  assert(strcmp(representation, "5") == 0);

  representation = int_to_cstring(6);
  assert(strcmp(representation, "6") == 0);

  representation = int_to_cstring(7);
  assert(strcmp(representation, "7") == 0);

  representation = int_to_cstring(8);
  assert(strcmp(representation, "8") == 0);

  representation = int_to_cstring(9);
  assert(strcmp(representation, "9") == 0);

  representation = int_to_cstring(10);
  assert(strcmp(representation, "10") == 0);

  representation = int_to_cstring(12);
  assert(strcmp(representation, "12") == 0);

  representation = int_to_cstring(23);
  assert(strcmp(representation, "23") == 0);

  representation = int_to_cstring(154);
  assert(strcmp(representation, "154") == 0);

  representation = int_to_cstring(20056);
  assert(strcmp(representation, "20056") == 0);

  // @note Em C não existe um zero negativo inteiro, porém o operador '-' pode ser especificado a frente de um zero
  // e não produzira efeito nenhum. Esse teste abaixo fica mais como uma curiosidade do que como um teste prático.
  representation = int_to_cstring(-0); 
  assert(strcmp(representation, "0") == 0);

  representation = int_to_cstring(-1);
  assert(strcmp(representation, "-1") == 0);

  representation = int_to_cstring(-2);
  assert(strcmp(representation, "-2") == 0);

  representation = int_to_cstring(-3);
  assert(strcmp(representation, "-3") == 0);

  representation = int_to_cstring(-4);
  assert(strcmp(representation, "-4") == 0);

  representation = int_to_cstring(-5);
  assert(strcmp(representation, "-5") == 0);

  representation = int_to_cstring(-6);
  assert(strcmp(representation, "-6") == 0);

  representation = int_to_cstring(-7);
  assert(strcmp(representation, "-7") == 0);

  representation = int_to_cstring(-8);
  assert(strcmp(representation, "-8") == 0);

  representation = int_to_cstring(-9);
  assert(strcmp(representation, "-9") == 0);

  representation = int_to_cstring(-10);
  assert(strcmp(representation, "-10") == 0);

  representation = int_to_cstring(-12);
  assert(strcmp(representation, "-12") == 0);

  representation = int_to_cstring(-23);
  assert(strcmp(representation, "-23") == 0);

  representation = int_to_cstring(-154);
  assert(strcmp(representation, "-154") == 0);

  representation = int_to_cstring(-20056);
  assert(strcmp(representation, "-20056") == 0);

  representation = int_to_cstring(-20057);
  assert(strcmp(representation, "-20057") == 0);

  representation = int_to_cstring(-210056);
  assert(strcmp(representation, "-210056") == 0);
}

void test_suit_utils(void)
{
  test_int_to_cstring();
}

