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

  // @todo João, quando houver uma função com suporte a numeros negativos e bases alternativas, adicionar os casos de teste nesse arquivo  
  representation = int_to_cstring(-0);
  assert(strcmp(representation, "0") == 0); // @todo João, validar, mas acho que em integers do C não tem como diferenciar -0 e 0

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

