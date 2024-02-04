#include <string.h>

#include "../src/utils.c"
#include "./test-runner.h"

void test_int_to_cstring(void)
{
  const char *representation;
  
  representation = int_to_cstring(0);
  Assert_String_Equals(representation, "0");

  representation = int_to_cstring(1);
  Assert_String_Equals(representation, "1");

  representation = int_to_cstring(2);
  Assert_String_Equals(representation, "2");

  representation = int_to_cstring(3);
  Assert_String_Equals(representation, "3");

  representation = int_to_cstring(4);
  Assert_String_Equals(representation, "4");

  representation = int_to_cstring(5);
  Assert_String_Equals(representation, "5");

  representation = int_to_cstring(6);
  Assert_String_Equals(representation, "6");

  representation = int_to_cstring(7);
  Assert_String_Equals(representation, "7");

  representation = int_to_cstring(8);
  Assert_String_Equals(representation, "8");

  representation = int_to_cstring(9);
  Assert_String_Equals(representation, "9");

  representation = int_to_cstring(10);
  Assert_String_Equals(representation, "10");

  representation = int_to_cstring(12);
  Assert_String_Equals(representation, "12");

  representation = int_to_cstring(23);
  Assert_String_Equals(representation, "23");

  representation = int_to_cstring(154);
  Assert_String_Equals(representation, "154");

  representation = int_to_cstring(20056);
  Assert_String_Equals(representation, "20056");

  // @note Em C não existe um zero negativo inteiro, porém o operador '-' pode ser especificado a frente de um zero
  // e não produzira efeito nenhum. Esse teste abaixo fica mais como uma curiosidade do que como um teste prático.
  representation = int_to_cstring(-0); 
  Assert_String_Equals(representation, "0");

  representation = int_to_cstring(-1);
  Assert_String_Equals(representation, "-1");

  representation = int_to_cstring(-2);
  Assert_String_Equals(representation, "-2");

  representation = int_to_cstring(-3);
  Assert_String_Equals(representation, "-3");

  representation = int_to_cstring(-4);
  Assert_String_Equals(representation, "-4");

  representation = int_to_cstring(-5);
  Assert_String_Equals(representation, "-5");

  representation = int_to_cstring(-6);
  Assert_String_Equals(representation, "-6");

  representation = int_to_cstring(-7);
  Assert_String_Equals(representation, "-7");

  representation = int_to_cstring(-8);
  Assert_String_Equals(representation, "-8");

  representation = int_to_cstring(-9);
  Assert_String_Equals(representation, "-9");

  representation = int_to_cstring(-10);
  Assert_String_Equals(representation, "-10");

  representation = int_to_cstring(-12);
  Assert_String_Equals(representation, "-12");

  representation = int_to_cstring(-23);
  Assert_String_Equals(representation, "-23");

  representation = int_to_cstring(-154);
  Assert_String_Equals(representation, "-154");

  representation = int_to_cstring(-20056);
  Assert_String_Equals(representation, "-20056");

  representation = int_to_cstring(-20057);
  Assert_String_Equals(representation, "-20057");

  representation = int_to_cstring(-210056);
  Assert_String_Equals(representation, "-210056");
}

void test_is_only_spaces_or_empty_01(void)
{
  Assert_Is_True(is_only_spaces_or_empty(" "));
}

void test_is_only_spaces_or_empty_02(void)
{
  Assert_Is_True(is_only_spaces_or_empty(""));
}

void test_is_only_spaces_or_empty_03(void)
{
  Assert_Is_True(is_only_spaces_or_empty("    "));
}

void test_is_only_spaces_or_empty_04(void)
{
  Assert_Is_False(is_only_spaces_or_empty("a"));
  Assert_Is_False(is_only_spaces_or_empty(" b"));
  Assert_Is_False(is_only_spaces_or_empty(" c "));
  Assert_Is_False(is_only_spaces_or_empty("d "));
  Assert_Is_False(is_only_spaces_or_empty("fg "));
}

extern void test_suit_utils(void)
{
  Register_Test(test_int_to_cstring);
  Register_Test(test_is_only_spaces_or_empty_01);
  Register_Test(test_is_only_spaces_or_empty_02);
  Register_Test(test_is_only_spaces_or_empty_03);
  Register_Test(test_is_only_spaces_or_empty_04);
}
