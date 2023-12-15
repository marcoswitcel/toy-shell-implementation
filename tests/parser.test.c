#include "../src/compilation_definitions.h"

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

#include "../src/types.h"
#include "../src/tokens.h"
#include "../src/parser.h"
#include "../src/utils.macro.h"
#include "../src/shell.c"

void test_try_parse_string_01(void)
{
  Parse_Context context = create_parse_context("ls -lha");
  Token token = STATIC_TOKEN(UNINITIALIZED);
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
  Token token = STATIC_TOKEN(UNINITIALIZED);
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
  Token token = STATIC_TOKEN(UNINITIALIZED);
  bool success = false;
  
  try_parse_string(&context, &token, &success);
  assert(success);
  assert(token.type == STRING);
  assert(token.data.string.cstring);
  assert(token.token_index_start == 0);
  assert(strcmp("echo", token.data.string.cstring) == 0);
  assert(context.index == 4);
  assert(context.error_start_index == -1);

  skip_whitespace(&context);
  assert(context.index == 5);
  try_parse_string(&context, &token, &success);
  assert(context.index == 5);
  assert(success == false);
  assert(context.error_start_index == 9);
}

void test_try_parse_string_04(void)
{
  Parse_Context context = create_parse_context(STRINGIFY("asd"));
  Token token = STATIC_TOKEN(UNINITIALIZED);
  bool success = false;
  
  try_parse_string(&context, &token, &success);
  assert(success);
  assert(token.type == STRING);
  assert(token.data.string.cstring);
  assert(token.token_index_start == 0);
  assert(strcmp("asd", token.data.string.cstring) == 0);
  assert(context.index == 5);
  assert(context.error_start_index == -1);
}

void test_try_parse_string_05(void)
{
  Parse_Context context = create_parse_context("asd");
  Token token = STATIC_TOKEN(UNINITIALIZED);
  bool success = false;
  
  try_parse_string(&context, &token, &success);
  assert(success);
  assert(token.type == STRING);
  assert(token.data.string.cstring);
  assert(token.token_index_start == 0);
  assert(strcmp("asd", token.data.string.cstring) == 0);
  assert(context.index == 3);
  assert(context.error_start_index == -1);
}

void test_try_parse_string_06(void)
{
  Parse_Context context = create_parse_context("\"asd");
  Token token = STATIC_TOKEN(UNINITIALIZED);
  bool success = false;
  
  try_parse_string(&context, &token, &success);
  assert(!success);
  assert(token.token_index_start == -1);
  assert(context.index == 0);
  assert(context.error_start_index == 4);
}

void test_try_parse_string_07(void)
{
  Parse_Context context = create_parse_context("asd\\");
  Token token = STATIC_TOKEN(UNINITIALIZED);
  bool success = false;
  
  try_parse_string(&context, &token, &success);
  assert(!success);
  assert(token.token_index_start == -1);
  assert(context.index == 0);
  assert(context.error_start_index == 3);
}

void test_try_parse_redirect_01(void)
{
  Parse_Context context = create_parse_context(">");
  Token token = STATIC_TOKEN(UNINITIALIZED);
  bool success = false;

  assert(token.type == UNINITIALIZED);
  assert(token.token_index_start == -1);
  
  try_parse_redirect(&context, &token, &success);
  assert(success);
  assert(token.type == REDIRECT);
  assert(token.token_index_start == 0);
  assert(context.index == 1);
  assert(context.error_start_index == -1);
}

void test_try_parse_redirect_02(void)
{
  Parse_Context context = create_parse_context("> texto");
  Token token = STATIC_TOKEN(UNINITIALIZED);
  bool success = false;

  assert(token.token_index_start == -1);
  
  try_parse_redirect(&context, &token, &success);
  assert(success);
  assert(token.token_index_start == 0);
  assert(context.index == 1);
  assert(context.error_start_index == -1);
}

void test_try_parse_redirect_03(void)
{
  Parse_Context context = create_parse_context(">texto");
  Token token = STATIC_TOKEN(UNINITIALIZED);
  bool success = false;

  assert(token.token_index_start == -1);
  assert(context.error_start_index == -1);
  
  try_parse_redirect(&context, &token, &success);
  assert(!success);
  assert(token.token_index_start == -1);
  assert(context.index == 0);
  assert(context.error_start_index == 1);
}

void test_try_parse_redirect_04(void)
{
  Parse_Context context = create_parse_context(">>");
  Token token = STATIC_TOKEN(UNINITIALIZED);
  bool success = false;

  assert(token.token_index_start == -1);
  
  try_parse_redirect(&context, &token, &success);
  assert(success);
  assert(token.token_index_start == 0);
  assert(context.index == 2);
  assert(context.error_start_index == -1);
}

void test_try_parse_redirect_05(void)
{
  Parse_Context context = create_parse_context(">> texto");
  Token token = STATIC_TOKEN(UNINITIALIZED);
  bool success = false;

  assert(token.token_index_start == -1);
  
  try_parse_redirect(&context, &token, &success);
  assert(success);
  assert(token.token_index_start == 0);
  assert(context.index == 2);
  assert(context.error_start_index == -1);
}

void test_try_parse_redirect_06(void)
{
  Parse_Context context = create_parse_context(">>texto");
  Token token = STATIC_TOKEN(UNINITIALIZED);
  bool success = false;

  assert(token.token_index_start == -1);
  assert(context.error_start_index == -1);
  
  try_parse_redirect(&context, &token, &success);
  assert(!success);
  assert(token.token_index_start == -1);
  assert(context.index == 0);
  assert(context.error_start_index == 2);
}

void test_try_parse_redirect_07(void)
{
  Parse_Context context = create_parse_context("1> arquivo.txt");
  Token token = STATIC_TOKEN(UNINITIALIZED);
  bool success = false;

  assert(token.token_index_start == -1);
  assert(context.error_start_index == -1);
  
  try_parse_redirect(&context, &token, &success);
  assert(success);
  assert(token.token_index_start == 0);
  assert(context.index == 2);
  assert(context.error_start_index == -1);
}

void test_try_parse_redirect_08(void)
{
  Parse_Context context = create_parse_context("2> arquivo.txt");
  Token token = STATIC_TOKEN(UNINITIALIZED);
  bool success = false;

  assert(token.token_index_start == -1);
  assert(context.error_start_index == -1);
  
  try_parse_redirect(&context, &token, &success);
  assert(success);
  assert(token.token_index_start == 0);
  assert(context.index == 2);
  assert(context.error_start_index == -1);
}

void test_try_parse_redirect_09(void)
{
  Parse_Context context = create_parse_context("1>> arquivo.txt");
  Token token = STATIC_TOKEN(UNINITIALIZED);
  bool success = false;

  assert(token.token_index_start == -1);
  assert(context.error_start_index == -1);
  
  try_parse_redirect(&context, &token, &success);
  assert(success);
  assert(token.token_index_start == 0);
  assert(context.index == 3);
  assert(context.error_start_index == -1);
}

void test_try_parse_redirect_10(void)
{
  Parse_Context context = create_parse_context("2>> arquivo.txt");
  Token token = STATIC_TOKEN(UNINITIALIZED);
  bool success = false;

  assert(token.token_index_start == -1);
  assert(context.error_start_index == -1);
  
  try_parse_redirect(&context, &token, &success);
  assert(success);
  assert(token.token_index_start == 0);
  assert(context.index == 3);
  assert(context.error_start_index == -1);
}

void test_try_parse_redirect_11(void)
{
  Parse_Context context = create_parse_context("> arquivo.txt");
  Token token = STATIC_TOKEN(UNINITIALIZED);
  bool success = false;

  assert(token.token_index_start == -1);
  assert(context.error_start_index == -1);
  
  try_parse_redirect(&context, &token, &success);
  assert(success);
  assert(!token.data.redirect.appending);
  assert(token.token_index_start == 0);
  assert(context.index == 1);
  assert(context.error_start_index == -1);
}

void test_try_parse_redirect_12(void)
{
  Parse_Context context = create_parse_context(">> arquivo.txt");
  Token token = STATIC_TOKEN(UNINITIALIZED);
  bool success = false;

  assert(token.token_index_start == -1);
  assert(context.error_start_index == -1);
  
  try_parse_redirect(&context, &token, &success);
  assert(success);
  assert(token.data.redirect.appending);
  assert(token.token_index_start == 0);
  assert(context.index == 2);
  assert(context.error_start_index == -1);
}

void test_try_parse_pipe01()
{
  Parse_Context context = create_parse_context("| grep");
  Token token = STATIC_TOKEN(UNINITIALIZED);
  bool success = false;

  assert(token.token_index_start == -1);
  assert(context.error_start_index == -1);
  
  try_parse_pipe(&context, &token, &success);
  assert(success);
  assert(token.type == PIPE);
  assert(token.data.pipe.cstring);
  assert(token.token_index_start == 0);
  assert(context.index == 1);
  assert(context.error_start_index == -1);
}

void test_try_parse_pipe02()
{
  Parse_Context context = create_parse_context("|grep");
  Token token = STATIC_TOKEN(UNINITIALIZED);
  bool success = false;

  assert(token.token_index_start == -1);
  assert(context.error_start_index == -1);
  
  try_parse_pipe(&context, &token, &success);
  assert(!success);
  assert(context.error);
  assert(context.index == 0);
  assert(context.error_start_index == 1);
}

void test_try_parse_and01()
{
  Parse_Context context = create_parse_context("&& grep");
  Token token = STATIC_TOKEN(UNINITIALIZED);
  bool success = false;

  assert(token.token_index_start == -1);
  assert(context.error_start_index == -1);
  
  try_parse_and(&context, &token, &success);
  assert(success);
  assert(token.type == AND);
  assert(token.data.and.cstring);
  assert(token.token_index_start == 0);
  assert(context.index == 2);
  assert(context.error_start_index == -1);
}

void test_try_parse_and02()
{
  Parse_Context context = create_parse_context("&&grep");
  Token token = STATIC_TOKEN(UNINITIALIZED);
  bool success = false;

  assert(token.token_index_start == -1);
  assert(context.error_start_index == -1);
  
  try_parse_and(&context, &token, &success);
  assert(!success);
  assert(context.error);
  assert(context.index == 0);
  assert(context.error_start_index == 2);
}

void test_try_parse_and03()
{
  Parse_Context context = create_parse_context("& grep");
  Token token = STATIC_TOKEN(UNINITIALIZED);
  bool success = false;

  assert(token.token_index_start == -1);
  assert(context.error_start_index == -1);
  
  try_parse_and(&context, &token, &success);
  assert(!success);
  assert(context.error);
  assert(context.index == 0);
  assert(context.error_start_index == 1);
}

void test_try_parse_and04()
{
  Parse_Context context = create_parse_context("&grep");
  Token token = STATIC_TOKEN(UNINITIALIZED);
  bool success = false;

  assert(token.token_index_start == -1);
  assert(context.error_start_index == -1);
  
  try_parse_and(&context, &token, &success);
  assert(!success);
  assert(context.error);
  assert(context.index == 0);
  assert(context.error_start_index == 1);
}

void test_try_parse_and05()
{
  Parse_Context context = create_parse_context("&&");
  Token token = STATIC_TOKEN(UNINITIALIZED);
  bool success = false;

  assert(token.token_index_start == -1);
  assert(context.error_start_index == -1);
  
  try_parse_and(&context, &token, &success);
  assert(!success);
  assert(context.error);
  assert(context.index == 0);
  assert(context.error_start_index == 2);
}

// @todo João, replicar esse teste para os outros 'parsers'
// a ideia por trás desse teste é que o parser tem o conceito de 
// fracasso que não gera aviso e fracaso de parsing que gera aviso.
void test_try_parse_and06()
{
  Parse_Context context = create_parse_context("asd &&");
  Token token = STATIC_TOKEN(UNINITIALIZED);
  bool success = false;

  assert(token.token_index_start == -1);
  assert(context.error_start_index == -1);
  
  try_parse_and(&context, &token, &success);
  assert(!success);
  assert(context.error == NULL);
  assert(context.index == 0);
  assert(context.error_start_index == -1);
}

void test_try_parse_globbing_01(void)
{
  Parse_Context context = create_parse_context("*");
  Token token = STATIC_TOKEN(UNINITIALIZED);
  bool success = false;

  assert(token.token_index_start == -1);
  
  try_parse_globbing(&context, &token, &success);
  assert(success);
  assert(token.type == GLOBBING);
  assert(token.token_index_start == 0);
  assert(context.index == 1);
  assert(context.error_start_index == -1);
}

void test_try_parse_globbing_02(void)
{
  Parse_Context context = create_parse_context("* texto");
  Token token = STATIC_TOKEN(UNINITIALIZED);
  bool success = false;

  assert(token.token_index_start == -1);
  
  try_parse_globbing(&context, &token, &success);
  assert(success);
  assert(token.type == GLOBBING);
  assert(token.token_index_start == 0);
  assert(context.index == 1);
  assert(context.error_start_index == -1);
}

void test_try_parse_globbing_03(void)
{
  Parse_Context context = create_parse_context("*texto");
  Token token = STATIC_TOKEN(UNINITIALIZED);
  bool success = false;

  assert(token.token_index_start == -1);
  assert(context.error_start_index == -1);
  
  try_parse_globbing(&context, &token, &success);
  assert(!success);
  assert(token.token_index_start == -1);
  assert(context.index == 0);
  assert(context.error_start_index == 1);
}

void test_try_query_status_01()
{
  Parse_Context context = create_parse_context("$?");
  Token token = STATIC_TOKEN(UNINITIALIZED);
  bool success = false;

  assert(token.token_index_start == -1);
  assert(context.error_start_index == -1);
  
  try_parse_query_last_status(&context, &token, &success);
  assert(success);
  assert(token.type == QUERY_LAST_STATUS);
  assert(token.data.and.cstring);
  assert(token.token_index_start == 0);
  assert(context.index == 2);
  assert(context.error_start_index == -1);
}

void test_try_query_status_02()
{
  Parse_Context context = create_parse_context("$? ");
  Token token = STATIC_TOKEN(UNINITIALIZED);
  bool success = false;

  assert(token.token_index_start == -1);
  assert(context.error_start_index == -1);
  
  try_parse_query_last_status(&context, &token, &success);
  assert(success);
  assert(token.type == QUERY_LAST_STATUS);
  assert(token.data.and.cstring);
  assert(token.token_index_start == 0);
  assert(context.index == 2);
  assert(context.error_start_index == -1);
}

void test_try_query_status_03()
{
  Parse_Context context = create_parse_context("$?a");
  Token token = STATIC_TOKEN(UNINITIALIZED);
  bool success = false;

  assert(token.token_index_start == -1);
  assert(context.error_start_index == -1);
  
  try_parse_query_last_status(&context, &token, &success);
  assert(!success);
  assert(token.type == UNINITIALIZED);
  assert(token.token_index_start == -1);
  assert(context.error);
  assert(context.index == 0);
  assert(context.error_start_index == 2);
}

void test_try_query_status_04()
{
  Parse_Context context = create_parse_context("$");
  Token token = STATIC_TOKEN(UNINITIALIZED);
  bool success = false;

  assert(token.token_index_start == -1);
  assert(context.error_start_index == -1);
  
  try_parse_query_last_status(&context, &token, &success);
  assert(!success);
  assert(token.type == UNINITIALIZED);
  assert(token.token_index_start == -1);
  assert(context.error);
  assert(context.index == 0);
  assert(context.error_start_index == 1);
}

void test_tokenize_01(void)
{
  const char parse_input_sample[] = "echo teste teste2 * > arquivo.txt";

  Parse_Context context = create_parse_context(parse_input_sample);
  assert(context.error == NULL);
  assert(context.index == 0);
  assert(context.length == strlen(parse_input_sample));

  Sequence_Of_Tokens *tokens = tokenize(&context);

  assert(tokens->index == 6);

  assert(context.error == NULL);
  assert(context.index == context.length);
  assert(context.length == strlen(parse_input_sample));

  
  assert(tokens->data[0].type == STRING);
  assert(tokens->data[0].data.string.cstring && strcmp(tokens->data[0].data.string.cstring, "echo") == 0);
  assert(tokens->data[0].token_index_start == 0);

  assert(tokens->data[1].type == STRING);
  assert(tokens->data[1].data.string.cstring && strcmp(tokens->data[1].data.string.cstring, "teste") == 0);
  assert(tokens->data[1].token_index_start == 5);

  assert(tokens->data[2].type == STRING);
  assert(tokens->data[2].data.string.cstring && strcmp(tokens->data[2].data.string.cstring, "teste2") == 0);
  assert(tokens->data[2].token_index_start == 11);

  assert(tokens->data[3].type == GLOBBING);
  assert(tokens->data[3].data.globbing.cstring && strcmp(tokens->data[3].data.globbing.cstring, "*") == 0);
  assert(tokens->data[3].token_index_start == 18);

  assert(tokens->data[4].type == REDIRECT);
  assert(tokens->data[4].data.redirect.cstring && strcmp(tokens->data[4].data.redirect.cstring, ">") == 0);
  assert(tokens->data[4].token_index_start == 20);

  assert(tokens->data[5].type == STRING);
  assert(tokens->data[5].data.string.cstring && strcmp(tokens->data[5].data.string.cstring, "arquivo.txt") == 0);
  assert(tokens->data[5].token_index_start == 22);
}

void test_tokenize_02(void)
{
  const char parse_input_sample[] = "echo \"teste' teste2\" * > arquivo.txt";

  Parse_Context context = create_parse_context(parse_input_sample);
  assert(context.error == NULL);
  assert(context.index == 0);
  assert(context.length == strlen(parse_input_sample));

  Sequence_Of_Tokens *tokens = tokenize(&context);

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

extern void test_suit_parser(void)
{
  test_try_parse_string_01();
  test_try_parse_string_02();
  test_try_parse_string_03();
  test_try_parse_string_04();
  test_try_parse_string_05();
  test_try_parse_string_06();
  test_try_parse_string_07();
  test_try_parse_redirect_01();
  test_try_parse_redirect_02();
  test_try_parse_redirect_03();
  test_try_parse_redirect_04();
  test_try_parse_redirect_05();
  test_try_parse_redirect_06();
  test_try_parse_redirect_07();
  test_try_parse_redirect_08();
  test_try_parse_redirect_09();
  test_try_parse_redirect_10();
  test_try_parse_redirect_11();
  test_try_parse_redirect_12();
  test_try_parse_globbing_01();
  test_try_parse_globbing_02();
  test_try_parse_globbing_03();
  test_try_parse_pipe01();
  test_try_parse_pipe02();
  test_try_parse_and01();
  test_try_parse_and02();
  test_try_parse_and03();
  test_try_parse_and04();
  test_try_parse_and05();
  test_try_parse_and06();
  test_try_query_status_01();
  test_try_query_status_02();
  test_try_query_status_03();
  test_try_query_status_04();
  test_tokenize_01();
  test_tokenize_02();
}