#include <string.h>

#include "../src/utils.c"
#include "./test-runner.h"

void test_int_to_cstring(void)
{
  const char *representation;
  
  representation = int_to_cstring(0);
  Assert(strcmp(representation, "0") == 0);

  representation = int_to_cstring(1);
  Assert(strcmp(representation, "1") == 0);

  representation = int_to_cstring(2);
  Assert(strcmp(representation, "2") == 0);

  representation = int_to_cstring(3);
  Assert(strcmp(representation, "3") == 0);

  representation = int_to_cstring(4);
  Assert(strcmp(representation, "4") == 0);

  representation = int_to_cstring(5);
  Assert(strcmp(representation, "5") == 0);

  representation = int_to_cstring(6);
  Assert(strcmp(representation, "6") == 0);

  representation = int_to_cstring(7);
  Assert(strcmp(representation, "7") == 0);

  representation = int_to_cstring(8);
  Assert(strcmp(representation, "8") == 0);

  representation = int_to_cstring(9);
  Assert(strcmp(representation, "9") == 0);

  representation = int_to_cstring(10);
  Assert(strcmp(representation, "10") == 0);

  representation = int_to_cstring(12);
  Assert(strcmp(representation, "12") == 0);

  representation = int_to_cstring(23);
  Assert(strcmp(representation, "23") == 0);

  representation = int_to_cstring(154);
  Assert(strcmp(representation, "154") == 0);

  representation = int_to_cstring(20056);
  Assert(strcmp(representation, "20056") == 0);

  // @note Em C não existe um zero negativo inteiro, porém o operador '-' pode ser especificado a frente de um zero
  // e não produzira efeito nenhum. Esse teste abaixo fica mais como uma curiosidade do que como um teste prático.
  representation = int_to_cstring(-0); 
  Assert(strcmp(representation, "0") == 0);

  representation = int_to_cstring(-1);
  Assert(strcmp(representation, "-1") == 0);

  representation = int_to_cstring(-2);
  Assert(strcmp(representation, "-2") == 0);

  representation = int_to_cstring(-3);
  Assert(strcmp(representation, "-3") == 0);

  representation = int_to_cstring(-4);
  Assert(strcmp(representation, "-4") == 0);

  representation = int_to_cstring(-5);
  Assert(strcmp(representation, "-5") == 0);

  representation = int_to_cstring(-6);
  Assert(strcmp(representation, "-6") == 0);

  representation = int_to_cstring(-7);
  Assert(strcmp(representation, "-7") == 0);

  representation = int_to_cstring(-8);
  Assert(strcmp(representation, "-8") == 0);

  representation = int_to_cstring(-9);
  Assert(strcmp(representation, "-9") == 0);

  representation = int_to_cstring(-10);
  Assert(strcmp(representation, "-10") == 0);

  representation = int_to_cstring(-12);
  Assert(strcmp(representation, "-12") == 0);

  representation = int_to_cstring(-23);
  Assert(strcmp(representation, "-23") == 0);

  representation = int_to_cstring(-154);
  Assert(strcmp(representation, "-154") == 0);

  representation = int_to_cstring(-20056);
  Assert(strcmp(representation, "-20056") == 0);

  representation = int_to_cstring(-20057);
  Assert(strcmp(representation, "-20057") == 0);

  representation = int_to_cstring(-210056);
  Assert(strcmp(representation, "-210056") == 0);
}

extern void test_suit_utils(void)
{
  Register_Test(test_int_to_cstring);
}

