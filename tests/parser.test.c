#include "../src/compilation_definitions.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <termios.h>
#include <ctype.h>

#include "../src/types.h"
#include "../src/tokens.h"
#include "../src/parser.c"
#include "../src/utils.macro.h"
#include "../src/shell.c"
#include "./test-runner.h"

void test_try_parse_string_01(void)
{
  Parse_Context context = create_parse_context("ls -lha");
  Token token = STATIC_TOKEN(UNINITIALIZED);
  bool success = false;
  
  try_parse_string(&context, &token, &success);
  Assert(success);
  Assert(token.type == STRING);
  Assert(token.data.string.cstring);
  Assert(strcmp("ls", token.data.string.cstring) == 0);
  Assert(context.index == 2);


  try_parse_string(&context, &token, &success);
  Assert(context.index == 2);
  Assert(success == false);

  skip_whitespace(&context);
  
  try_parse_string(&context, &token, &success);
  Assert(success);
  Assert(token.type == STRING);
  Assert(context.index == 7);
  Assert(token.data.string.cstring);
  Assert(strcmp("-lha", token.data.string.cstring) == 0);
}

void test_try_parse_string_02(void)
{
  Parse_Context context = create_parse_context("ls ");
  Token token = STATIC_TOKEN(UNINITIALIZED);
  bool success = false;
  
  try_parse_string(&context, &token, &success);
  Assert(success);
  Assert(token.type == STRING);
  Assert(token.data.string.cstring);
  Assert(strcmp("ls", token.data.string.cstring) == 0);
  Assert(context.index == 2);


  try_parse_string(&context, &token, &success);
  Assert(context.index == 2);
  Assert(success == false);

  skip_whitespace(&context);
  
  try_parse_string(&context, &token, &success);
  Assert(success == false);
}


void test_try_parse_string_03(void)
{
  Parse_Context context = create_parse_context("echo \"ads");
  Token token = STATIC_TOKEN(UNINITIALIZED);
  bool success = false;
  
  try_parse_string(&context, &token, &success);
  Assert(success);
  Assert(token.type == STRING);
  Assert(token.data.string.cstring);
  Assert(token.token_index_start == 0);
  Assert(strcmp("echo", token.data.string.cstring) == 0);
  Assert(context.index == 4);
  Assert(context.error_start_index == -1);

  skip_whitespace(&context);
  Assert(context.index == 5);
  try_parse_string(&context, &token, &success);
  Assert(context.index == 5);
  Assert(success == false);
  Assert(context.error_start_index == 9);
}

void test_try_parse_string_04(void)
{
  Parse_Context context = create_parse_context(STRINGIFY("asd"));
  Token token = STATIC_TOKEN(UNINITIALIZED);
  bool success = false;
  
  try_parse_string(&context, &token, &success);
  Assert(success);
  Assert(token.type == STRING);
  Assert(token.data.string.cstring);
  Assert(token.token_index_start == 0);
  Assert(strcmp("asd", token.data.string.cstring) == 0);
  Assert(context.index == 5);
  Assert(context.error_start_index == -1);
}

void test_try_parse_string_05(void)
{
  Parse_Context context = create_parse_context("asd");
  Token token = STATIC_TOKEN(UNINITIALIZED);
  bool success = false;
  
  try_parse_string(&context, &token, &success);
  Assert(success);
  Assert(token.type == STRING);
  Assert(token.data.string.cstring);
  Assert(token.token_index_start == 0);
  Assert(strcmp("asd", token.data.string.cstring) == 0);
  Assert(context.index == 3);
  Assert(context.error_start_index == -1);
}

void test_try_parse_string_06(void)
{
  Parse_Context context = create_parse_context("\"asd");
  Token token = STATIC_TOKEN(UNINITIALIZED);
  bool success = false;
  
  try_parse_string(&context, &token, &success);
  Assert(!success);
  Assert(token.token_index_start == -1);
  Assert(context.index == 0);
  Assert(context.error_start_index == 4);
}

void test_try_parse_string_07(void)
{
  Parse_Context context = create_parse_context("asd\\");
  Token token = STATIC_TOKEN(UNINITIALIZED);
  bool success = false;
  
  try_parse_string(&context, &token, &success);
  Assert(!success);
  Assert(token.token_index_start == -1);
  Assert(context.index == 0);
  Assert(context.error_start_index == 3);
}

void test_try_parse_redirect_01(void)
{
  Parse_Context context = create_parse_context(">");
  Token token = STATIC_TOKEN(UNINITIALIZED);
  bool success = false;

  Assert(token.type == UNINITIALIZED);
  Assert(token.token_index_start == -1);
  
  try_parse_redirect(&context, &token, &success);
  Assert(success);
  Assert(token.type == REDIRECT);
  Assert(token.token_index_start == 0);
  Assert(context.index == 1);
  Assert(context.error_start_index == -1);
}

void test_try_parse_redirect_02(void)
{
  Parse_Context context = create_parse_context("> texto");
  Token token = STATIC_TOKEN(UNINITIALIZED);
  bool success = false;

  Assert(token.token_index_start == -1);
  
  try_parse_redirect(&context, &token, &success);
  Assert(success);
  Assert(token.token_index_start == 0);
  Assert(context.index == 1);
  Assert(context.error_start_index == -1);
}

void test_try_parse_redirect_03(void)
{
  Parse_Context context = create_parse_context(">texto");
  Token token = STATIC_TOKEN(UNINITIALIZED);
  bool success = false;

  Assert(token.token_index_start == -1);
  Assert(context.error_start_index == -1);
  
  try_parse_redirect(&context, &token, &success);
  Assert(!success);
  Assert(token.token_index_start == -1);
  Assert(context.index == 0);
  Assert(context.error_start_index == 1);
}

void test_try_parse_redirect_04(void)
{
  Parse_Context context = create_parse_context(">>");
  Token token = STATIC_TOKEN(UNINITIALIZED);
  bool success = false;

  Assert(token.token_index_start == -1);
  
  try_parse_redirect(&context, &token, &success);
  Assert(success);
  Assert(token.token_index_start == 0);
  Assert(context.index == 2);
  Assert(context.error_start_index == -1);
}

void test_try_parse_redirect_05(void)
{
  Parse_Context context = create_parse_context(">> texto");
  Token token = STATIC_TOKEN(UNINITIALIZED);
  bool success = false;

  Assert(token.token_index_start == -1);
  
  try_parse_redirect(&context, &token, &success);
  Assert(success);
  Assert(token.token_index_start == 0);
  Assert(context.index == 2);
  Assert(context.error_start_index == -1);
}

void test_try_parse_redirect_06(void)
{
  Parse_Context context = create_parse_context(">>texto");
  Token token = STATIC_TOKEN(UNINITIALIZED);
  bool success = false;

  Assert(token.token_index_start == -1);
  Assert(context.error_start_index == -1);
  
  try_parse_redirect(&context, &token, &success);
  Assert(!success);
  Assert(token.token_index_start == -1);
  Assert(context.index == 0);
  Assert(context.error_start_index == 2);
}

void test_try_parse_redirect_07(void)
{
  Parse_Context context = create_parse_context("1> arquivo.txt");
  Token token = STATIC_TOKEN(UNINITIALIZED);
  bool success = false;

  Assert(token.token_index_start == -1);
  Assert(context.error_start_index == -1);
  
  try_parse_redirect(&context, &token, &success);
  Assert(success);
  Assert(token.token_index_start == 0);
  Assert(context.index == 2);
  Assert(context.error_start_index == -1);
}

void test_try_parse_redirect_08(void)
{
  Parse_Context context = create_parse_context("2> arquivo.txt");
  Token token = STATIC_TOKEN(UNINITIALIZED);
  bool success = false;

  Assert(token.token_index_start == -1);
  Assert(context.error_start_index == -1);
  
  try_parse_redirect(&context, &token, &success);
  Assert(success);
  Assert(token.token_index_start == 0);
  Assert(context.index == 2);
  Assert(context.error_start_index == -1);
}

void test_try_parse_redirect_09(void)
{
  Parse_Context context = create_parse_context("1>> arquivo.txt");
  Token token = STATIC_TOKEN(UNINITIALIZED);
  bool success = false;

  Assert(token.token_index_start == -1);
  Assert(context.error_start_index == -1);
  
  try_parse_redirect(&context, &token, &success);
  Assert(success);
  Assert(token.token_index_start == 0);
  Assert(context.index == 3);
  Assert(context.error_start_index == -1);
}

void test_try_parse_redirect_10(void)
{
  Parse_Context context = create_parse_context("2>> arquivo.txt");
  Token token = STATIC_TOKEN(UNINITIALIZED);
  bool success = false;

  Assert(token.token_index_start == -1);
  Assert(context.error_start_index == -1);
  
  try_parse_redirect(&context, &token, &success);
  Assert(success);
  Assert(token.token_index_start == 0);
  Assert(context.index == 3);
  Assert(context.error_start_index == -1);
}

void test_try_parse_redirect_11(void)
{
  Parse_Context context = create_parse_context("> arquivo.txt");
  Token token = STATIC_TOKEN(UNINITIALIZED);
  bool success = false;

  Assert(token.token_index_start == -1);
  Assert(context.error_start_index == -1);
  
  try_parse_redirect(&context, &token, &success);
  Assert(success);
  Assert(!token.data.redirect.appending);
  Assert(token.token_index_start == 0);
  Assert(context.index == 1);
  Assert(context.error_start_index == -1);
}

void test_try_parse_redirect_12(void)
{
  Parse_Context context = create_parse_context(">> arquivo.txt");
  Token token = STATIC_TOKEN(UNINITIALIZED);
  bool success = false;

  Assert(token.token_index_start == -1);
  Assert(context.error_start_index == -1);
  
  try_parse_redirect(&context, &token, &success);
  Assert(success);
  Assert(token.data.redirect.appending);
  Assert(token.token_index_start == 0);
  Assert(context.index == 2);
  Assert(context.error_start_index == -1);
}

void test_try_parse_pipe01()
{
  Parse_Context context = create_parse_context("| grep");
  Token token = STATIC_TOKEN(UNINITIALIZED);
  bool success = false;

  Assert(token.token_index_start == -1);
  Assert(context.error_start_index == -1);
  
  try_parse_pipe(&context, &token, &success);
  Assert(success);
  Assert(token.type == PIPE);
  Assert(token.data.pipe.cstring);
  Assert(token.token_index_start == 0);
  Assert(context.index == 1);
  Assert(context.error_start_index == -1);
}

void test_try_parse_pipe02()
{
  Parse_Context context = create_parse_context("|grep");
  Token token = STATIC_TOKEN(UNINITIALIZED);
  bool success = false;

  Assert(token.token_index_start == -1);
  Assert(context.error_start_index == -1);
  
  try_parse_pipe(&context, &token, &success);
  Assert(!success);
  Assert(context.error);
  Assert(context.index == 0);
  Assert(context.error_start_index == 1);
}

void test_try_parse_and01()
{
  Parse_Context context = create_parse_context("&& grep");
  Token token = STATIC_TOKEN(UNINITIALIZED);
  bool success = false;

  Assert(token.token_index_start == -1);
  Assert(context.error_start_index == -1);
  
  try_parse_and(&context, &token, &success);
  Assert(success);
  Assert(token.type == AND);
  Assert(token.data.and.cstring);
  Assert(token.token_index_start == 0);
  Assert(context.index == 2);
  Assert(context.error_start_index == -1);
}

void test_try_parse_and02()
{
  Parse_Context context = create_parse_context("&&grep");
  Token token = STATIC_TOKEN(UNINITIALIZED);
  bool success = false;

  Assert(token.token_index_start == -1);
  Assert(context.error_start_index == -1);
  
  try_parse_and(&context, &token, &success);
  Assert(!success);
  Assert(context.error);
  Assert(context.index == 0);
  Assert(context.error_start_index == 2);
}

void test_try_parse_and03()
{
  Parse_Context context = create_parse_context("& grep");
  Token token = STATIC_TOKEN(UNINITIALIZED);
  bool success = false;

  Assert(token.token_index_start == -1);
  Assert(context.error_start_index == -1);
  
  try_parse_and(&context, &token, &success);
  Assert(!success);
  Assert(context.error);
  Assert(context.index == 0);
  Assert(context.error_start_index == 1);
}

void test_try_parse_and04()
{
  Parse_Context context = create_parse_context("&grep");
  Token token = STATIC_TOKEN(UNINITIALIZED);
  bool success = false;

  Assert(token.token_index_start == -1);
  Assert(context.error_start_index == -1);
  
  try_parse_and(&context, &token, &success);
  Assert(!success);
  Assert(context.error);
  Assert(context.index == 0);
  Assert(context.error_start_index == 1);
}

void test_try_parse_and05()
{
  Parse_Context context = create_parse_context("&&");
  Token token = STATIC_TOKEN(UNINITIALIZED);
  bool success = false;

  Assert(token.token_index_start == -1);
  Assert(context.error_start_index == -1);
  
  try_parse_and(&context, &token, &success);
  Assert(!success);
  Assert(context.error);
  Assert(context.index == 0);
  Assert(context.error_start_index == 2);
}

// @todo João, replicar esse teste para os outros 'parsers'
// a ideia por trás desse teste é que o parser tem o conceito de 
// fracasso que não gera aviso e fracaso de parsing que gera aviso.
void test_try_parse_and06()
{
  Parse_Context context = create_parse_context("asd &&");
  Token token = STATIC_TOKEN(UNINITIALIZED);
  bool success = false;

  Assert(token.token_index_start == -1);
  Assert(context.error_start_index == -1);
  
  try_parse_and(&context, &token, &success);
  Assert(!success);
  Assert(context.error == NULL);
  Assert(context.index == 0);
  Assert(context.error_start_index == -1);
}

void test_try_parse_globbing_01(void)
{
  Parse_Context context = create_parse_context("*");
  Token token = STATIC_TOKEN(UNINITIALIZED);
  bool success = false;

  Assert(token.token_index_start == -1);
  
  try_parse_globbing(&context, &token, &success);
  Assert(success);
  Assert(token.type == GLOBBING);
  Assert(token.token_index_start == 0);
  Assert(context.index == 1);
  Assert(context.error_start_index == -1);
}

void test_try_parse_globbing_02(void)
{
  Parse_Context context = create_parse_context("* texto");
  Token token = STATIC_TOKEN(UNINITIALIZED);
  bool success = false;

  Assert(token.token_index_start == -1);
  
  try_parse_globbing(&context, &token, &success);
  Assert(success);
  Assert(token.type == GLOBBING);
  Assert(token.token_index_start == 0);
  Assert(context.index == 1);
  Assert(context.error_start_index == -1);
}

void test_try_parse_globbing_03(void)
{
  Parse_Context context = create_parse_context("*texto");
  Token token = STATIC_TOKEN(UNINITIALIZED);
  bool success = false;

  Assert(token.token_index_start == -1);
  Assert(context.error_start_index == -1);
  
  try_parse_globbing(&context, &token, &success);
  Assert(!success);
  Assert(token.token_index_start == -1);
  Assert(context.index == 0);
  Assert(context.error_start_index == 1);
}

void test_try_query_status_01()
{
  Parse_Context context = create_parse_context("$?");
  Token token = STATIC_TOKEN(UNINITIALIZED);
  bool success = false;

  Assert(token.token_index_start == -1);
  Assert(context.error_start_index == -1);
  
  try_parse_query_last_status(&context, &token, &success);
  Assert(success);
  Assert(token.type == QUERY_LAST_STATUS);
  Assert(token.data.and.cstring);
  Assert(token.token_index_start == 0);
  Assert(context.index == 2);
  Assert(context.error_start_index == -1);
}

void test_try_query_status_02()
{
  Parse_Context context = create_parse_context("$? ");
  Token token = STATIC_TOKEN(UNINITIALIZED);
  bool success = false;

  Assert(token.token_index_start == -1);
  Assert(context.error_start_index == -1);
  
  try_parse_query_last_status(&context, &token, &success);
  Assert(success);
  Assert(token.type == QUERY_LAST_STATUS);
  Assert(token.data.and.cstring);
  Assert(token.token_index_start == 0);
  Assert(context.index == 2);
  Assert(context.error_start_index == -1);
}

void test_try_query_status_03()
{
  Parse_Context context = create_parse_context("$?a");
  Token token = STATIC_TOKEN(UNINITIALIZED);
  bool success = false;

  Assert(token.token_index_start == -1);
  Assert(context.error_start_index == -1);
  
  try_parse_query_last_status(&context, &token, &success);
  Assert(!success);
  Assert(token.type == UNINITIALIZED);
  Assert(token.token_index_start == -1);
  Assert(context.error);
  Assert(context.index == 0);
  Assert(context.error_start_index == 2);
}

void test_try_query_status_04()
{
  Parse_Context context = create_parse_context("$");
  Token token = STATIC_TOKEN(UNINITIALIZED);
  bool success = false;

  Assert(token.token_index_start == -1);
  Assert(context.error_start_index == -1);
  
  try_parse_query_last_status(&context, &token, &success);
  Assert(!success);
  Assert(token.type == UNINITIALIZED);
  Assert(token.token_index_start == -1);
  Assert(context.error);
  Assert(context.index == 0);
  Assert(context.error_start_index == 1);
}

void test_tokenize_01(void)
{
  const char parse_input_sample[] = "echo teste teste2 * > arquivo.txt";

  Parse_Context context = create_parse_context(parse_input_sample);
  Assert(context.error == NULL);
  Assert(context.index == 0);
  Assert(context.length == strlen(parse_input_sample));

  Sequence_Of_Tokens *tokens = tokenize(&context);

  Assert(tokens->index == 6);

  Assert(context.error == NULL);
  Assert(context.index == context.length);
  Assert(context.length == strlen(parse_input_sample));

  
  Assert(tokens->data[0].type == STRING);
  Assert(tokens->data[0].data.string.cstring && strcmp(tokens->data[0].data.string.cstring, "echo") == 0);
  Assert(tokens->data[0].token_index_start == 0);

  Assert(tokens->data[1].type == STRING);
  Assert(tokens->data[1].data.string.cstring && strcmp(tokens->data[1].data.string.cstring, "teste") == 0);
  Assert(tokens->data[1].token_index_start == 5);

  Assert(tokens->data[2].type == STRING);
  Assert(tokens->data[2].data.string.cstring && strcmp(tokens->data[2].data.string.cstring, "teste2") == 0);
  Assert(tokens->data[2].token_index_start == 11);

  Assert(tokens->data[3].type == GLOBBING);
  Assert(tokens->data[3].data.globbing.cstring && strcmp(tokens->data[3].data.globbing.cstring, "*") == 0);
  Assert(tokens->data[3].token_index_start == 18);

  Assert(tokens->data[4].type == REDIRECT);
  Assert(tokens->data[4].data.redirect.cstring && strcmp(tokens->data[4].data.redirect.cstring, ">") == 0);
  Assert(tokens->data[4].token_index_start == 20);

  Assert(tokens->data[5].type == STRING);
  Assert(tokens->data[5].data.string.cstring && strcmp(tokens->data[5].data.string.cstring, "arquivo.txt") == 0);
  Assert(tokens->data[5].token_index_start == 22);
}

void test_tokenize_02(void)
{
  const char parse_input_sample[] = "echo \"teste' teste2\" * > arquivo.txt";

  Parse_Context context = create_parse_context(parse_input_sample);
  Assert(context.error == NULL);
  Assert(context.index == 0);
  Assert(context.length == strlen(parse_input_sample));

  Sequence_Of_Tokens *tokens = tokenize(&context);

  Assert(tokens->index == 5);

  Assert(context.error == NULL);
  Assert(context.index == context.length);
  Assert(context.length == strlen(parse_input_sample));

  
  Assert(tokens->data[0].type == STRING);
  Assert(tokens->data[0].data.string.cstring && strcmp(tokens->data[0].data.string.cstring, "echo") == 0);

  Assert(tokens->data[1].type == STRING);
  Assert(tokens->data[1].data.string.cstring && strcmp(tokens->data[1].data.string.cstring, "teste' teste2") == 0);

  Assert(tokens->data[2].type == GLOBBING);
  Assert(tokens->data[2].data.globbing.cstring && strcmp(tokens->data[2].data.globbing.cstring, "*") == 0);

  Assert(tokens->data[3].type == REDIRECT);
  Assert(tokens->data[3].data.redirect.cstring && strcmp(tokens->data[3].data.redirect.cstring, ">") == 0);

  Assert(tokens->data[4].type == STRING);
  Assert(tokens->data[4].data.string.cstring && strcmp(tokens->data[4].data.string.cstring, "arquivo.txt") == 0);
}

void test_tokenize_03(void)
{
  const char parse_input_sample[] = "echo teste | grep teste";

  Parse_Context context = create_parse_context(parse_input_sample);
  Assert(context.error == NULL);
  Assert(context.index == 0);
  Assert(context.length == strlen(parse_input_sample));

  Sequence_Of_Tokens *tokens = tokenize(&context);

  Assert(tokens->index == 5);

  Assert(context.error == NULL);
  Assert(context.index == context.length);
  Assert(context.length == strlen(parse_input_sample));

  
  Assert(tokens->data[0].type == STRING);
  Assert(tokens->data[0].data.string.cstring && strcmp(tokens->data[0].data.string.cstring, "echo") == 0);

  Assert(tokens->data[1].type == STRING);
  Assert(tokens->data[1].data.string.cstring && strcmp(tokens->data[1].data.string.cstring, "teste") == 0);

  Assert(tokens->data[2].type == PIPE);
  Assert(tokens->data[2].data.pipe.cstring && strcmp(tokens->data[2].data.globbing.cstring, "|") == 0);

  Assert(tokens->data[3].type == STRING);
  Assert(tokens->data[3].data.string.cstring && strcmp(tokens->data[3].data.redirect.cstring, "grep") == 0);

  Assert(tokens->data[4].type == STRING);
  Assert(tokens->data[4].data.string.cstring && strcmp(tokens->data[4].data.string.cstring, "teste") == 0);
}

void test_tokenize_04(void)
{
  const char parse_input_sample[] = "echo \"teste";

  Parse_Context context = create_parse_context(parse_input_sample);
  Assert(context.error == NULL);
  Assert(context.index == 0);
  Assert(context.length == strlen(parse_input_sample));

  Sequence_Of_Tokens *tokens = tokenize(&context);

  Assert(tokens->index == 1);

  Assert(context.length == strlen(parse_input_sample));

  
  Assert(tokens->data[0].type == STRING);
  Assert(tokens->data[0].data.string.cstring && strcmp(tokens->data[0].data.string.cstring, "echo") == 0);

  Assert(context.error);
  Assert(context.error_start_index);
}

// @todo João, reestruturar o teste do parse command para testar essa função também `parse_execute_command_node`

extern void test_suit_parser(void)
{
  Register_Test(test_try_parse_string_01);
  Register_Test(test_try_parse_string_02);
  Register_Test(test_try_parse_string_03);
  Register_Test(test_try_parse_string_04);
  Register_Test(test_try_parse_string_05);
  Register_Test(test_try_parse_string_06);
  Register_Test(test_try_parse_string_07);
  Register_Test(test_try_parse_redirect_01);
  Register_Test(test_try_parse_redirect_02);
  Register_Test(test_try_parse_redirect_03);
  Register_Test(test_try_parse_redirect_04);
  Register_Test(test_try_parse_redirect_05);
  Register_Test(test_try_parse_redirect_06);
  Register_Test(test_try_parse_redirect_07);
  Register_Test(test_try_parse_redirect_08);
  Register_Test(test_try_parse_redirect_09);
  Register_Test(test_try_parse_redirect_10);
  Register_Test(test_try_parse_redirect_11);
  Register_Test(test_try_parse_redirect_12);
  Register_Test(test_try_parse_globbing_01);
  Register_Test(test_try_parse_globbing_02);
  Register_Test(test_try_parse_globbing_03);
  Register_Test(test_try_parse_pipe01);
  Register_Test(test_try_parse_pipe02);
  Register_Test(test_try_parse_and01);
  Register_Test(test_try_parse_and02);
  Register_Test(test_try_parse_and03);
  Register_Test(test_try_parse_and04);
  Register_Test(test_try_parse_and05);
  Register_Test(test_try_parse_and06);
  Register_Test(test_try_query_status_01);
  Register_Test(test_try_query_status_02);
  Register_Test(test_try_query_status_03);
  Register_Test(test_try_query_status_04);
  Register_Test(test_tokenize_01);
  Register_Test(test_tokenize_02);
  Register_Test(test_tokenize_03);
  Register_Test(test_tokenize_04);
}
