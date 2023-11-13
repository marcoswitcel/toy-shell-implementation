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
}

void test_suit_utils(void)
{
  test_int_to_cstring();
}
