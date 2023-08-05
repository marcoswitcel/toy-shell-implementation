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
#define DEBUG_INFO false

#include "./types.h"
#include "./tokens.h"
#include "./parser.h"
#include "./list.implementations.h"

void test_list_char_prt_implementation(void)
{
  List_Of_Strings *list = create_list_of_strings(1024, 1024);
  char test_name[] = "Tux";

  assert(list->index == 0);
  list_of_strings_pop(list);
  assert(list->index == 0);

  list_of_strings_push(list, test_name);
  assert(list->index == 1);
  list_of_strings_push(list, test_name);
  assert(list->index == 2);

  list_of_strings_push(list, test_name);
  list_of_strings_pop(list);

  assert(list->index == 2);

  assert(list->data[0] && list->data[1]);

  assert(list->data[0] == list->data[1]);

  assert(list->data[0] == test_name);
}

void test_list_of_floats_implementation(void)
{
  List_Of_Floats *list = create_list_of_floats(1024, 1024);
  float float_value_a = 0.5;
  float float_value_b = 0.3;

  assert(list->index == 0);
  list_of_floats_pop(list);
  assert(list->index == 0);

  list_of_floats_push(list, float_value_a);
  assert(list->data[0] == float_value_a);
  assert(list->index == 1);
  list_of_floats_push(list, float_value_a);
  assert(list->data[1] == float_value_a);
  assert(list->index == 2);

  list_of_floats_push(list, float_value_a);
  list_of_floats_pop(list);
  assert(list->index == 2);

  list_of_floats_push(list, float_value_b);
  assert(list->index == 3);
  assert(list->data[2] == float_value_b);
  list_of_floats_ensure_enough_space(list);
}


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


void test_try_parse_string_03(void)
{
  Parse_Context context = create_parse_context("echo \"ads");
  Token token = { 0 };
  bool success = false;
  
  try_parse_string(&context, &token, &success);
  assert(success);
  assert(token.type == STRING);
  assert(token.data.string.cstring);
  assert(strcmp("echo", token.data.string.cstring) == 0);
  assert(context.index == 4);

  skip_whitespace(&context);
  assert(context.index == 5);
  try_parse_string(&context, &token, &success);
  assert(context.index == 5);
  assert(success == false);
}

void test_parse_01(void)
{
  const char parse_input_sample[] = "echo teste teste2 * > arquivo.txt";

  Parse_Context context = create_parse_context(parse_input_sample);
  assert(context.error == NULL);
  assert(context.index == 0);
  assert(context.length == strlen(parse_input_sample));

  Sequence_Of_Tokens *tokens = parse(&context);

  assert(tokens->index == 6);

  assert(context.error == NULL);
  assert(context.index == context.length);
  assert(context.length == strlen(parse_input_sample));

  
  assert(tokens->data[0].type == STRING);
  assert(tokens->data[0].data.string.cstring && strcmp(tokens->data[0].data.string.cstring, "echo") == 0);

  assert(tokens->data[1].type == STRING);
  assert(tokens->data[1].data.string.cstring && strcmp(tokens->data[1].data.string.cstring, "teste") == 0);

  assert(tokens->data[2].type == STRING);
  assert(tokens->data[2].data.string.cstring && strcmp(tokens->data[2].data.string.cstring, "teste2") == 0);

  assert(tokens->data[3].type == GLOBBING);
  assert(tokens->data[3].data.globbing.cstring && strcmp(tokens->data[3].data.globbing.cstring, "*") == 0);

  assert(tokens->data[4].type == REDIRECT);
  assert(tokens->data[4].data.redirect.cstring && strcmp(tokens->data[4].data.redirect.cstring, ">") == 0);

  assert(tokens->data[5].type == STRING);
  assert(tokens->data[5].data.string.cstring && strcmp(tokens->data[5].data.string.cstring, "arquivo.txt") == 0);
}

void test_parse_02(void)
{
  const char parse_input_sample[] = "echo \"teste' teste2\" * > arquivo.txt";

  Parse_Context context = create_parse_context(parse_input_sample);
  assert(context.error == NULL);
  assert(context.index == 0);
  assert(context.length == strlen(parse_input_sample));

  Sequence_Of_Tokens *tokens = parse(&context);

  assert(tokens->index == 5);

  assert(context.error == NULL);
  assert(context.index == context.length);
  assert(context.length == strlen(parse_input_sample));

  
  assert(tokens->data[0].type == STRING);
  assert(tokens->data[0].data.string.cstring && strcmp(tokens->data[0].data.string.cstring, "echo") == 0);

  assert(tokens->data[1].type == STRING);
  assert(tokens->data[1].data.string.cstring && strcmp(tokens->data[1].data.string.cstring, "teste' teste2") == 0);

  assert(tokens->data[2].type == GLOBBING);
  assert(tokens->data[2].data.globbing.cstring && strcmp(tokens->data[2].data.globbing.cstring, "*") == 0);

  assert(tokens->data[3].type == REDIRECT);
  assert(tokens->data[3].data.redirect.cstring && strcmp(tokens->data[3].data.redirect.cstring, ">") == 0);

  assert(tokens->data[4].type == STRING);
  assert(tokens->data[4].data.string.cstring && strcmp(tokens->data[4].data.string.cstring, "arquivo.txt") == 0);
}

int main(void)
{
  printf("Executando testes\n");

  test_try_parse_string_01();
  test_try_parse_string_02();
  test_try_parse_string_03();
  test_list_char_prt_implementation();
  test_list_of_floats_implementation();
  test_parse_01();
  test_parse_02();
  
  printf("Testes executados com sucesso! Nenhum erro detectado.");

  return EXIT_SUCCESS;
}
