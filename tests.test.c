#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <termios.h>
#include <ctype.h>

// Desenvolvimento
#define DEBUG_INFO true

#include "./types.h"
#include "./list.h"
#include "./parser.h"


void test_try_parse_string_01(void)
{
  Parse_Context context = create_parse_context("ls -lha");
  Token token = { 0 };
  bool success = false;
  
  try_parse_string(&context, &token, &success);
  assert(success);
  assert(token.type == STRING);
  assert(token.data.string.cstring);
  assert(strcmp("ls", token.data.string.cstring) == 0);
  assert(context.index == 2);


  try_parse_string(&context, &token, &success);
  assert(context.index == 2);
  assert(success == false);

  skip_whitespace(&context);
  
  try_parse_string(&context, &token, &success);
  assert(success);
  assert(token.type == STRING);
  assert(context.index == 7);
  assert(token.data.string.cstring);
  assert(strcmp("-lha", token.data.string.cstring) == 0);
}

void test_try_parse_string_02(void)
{
  Parse_Context context = create_parse_context("ls ");
  Token token = { 0 };
  bool success = false;
  
  try_parse_string(&context, &token, &success);
  assert(success);
  assert(token.type == STRING);
  assert(token.data.string.cstring);
  assert(strcmp("ls", token.data.string.cstring) == 0);
  assert(context.index == 2);


  try_parse_string(&context, &token, &success);
  assert(context.index == 2);
  assert(success == false);

  skip_whitespace(&context);
  
  try_parse_string(&context, &token, &success);
  assert(success == false);
}

int main(void)
{
  printf("Executando testes\n");

  test_try_parse_string_01();
  test_try_parse_string_02();
  
  printf("Testes executados com sucesso! Nenhum erro detectado.");

  return EXIT_SUCCESS;
}