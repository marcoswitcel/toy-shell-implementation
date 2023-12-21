#ifndef _TEST_RUNNER_C_
#define _TEST_RUNNER_C_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "./test-runner.h"
#include "../src/buffer.h"
#include "../src/list.macro.h"
#include "../src/utils.c"
#include "../src/utils.macro.h"
#include "../src/list.implementations.h"
#include "../src/ansi-escape-sequences.h"

MAKE_LIST_IMPLEMENTATION(List_Of_Tests, list_of_tests, Test_Proc)

static int number_of_tests = 0;
static int number_of_success_tests = 0;
static int number_of_failed_tests = 0;
static List_Of_Tests *tests = NULL;
static List_Of_Strings *proc_names = NULL;

typedef struct Test_State {
  bool at_least_one_failed;
  unsigned passed;
  unsigned failed;
  const char *filename;
  const char *expr;
  unsigned line_number;
  // @todo João, adicionar o tempo por teste
} Test_State;

static Test_State current_state = { 0 };

static void ensure_is_initialized()
{
  if (tests == NULL)
  {
    tests = create_list_of_tests(1024, 124);
  }

  if (proc_names == NULL)
  {
    proc_names = create_list_of_strings(1024, 124);
  }
}

void register_test(Test_Proc test, const char *name)
{
  ensure_is_initialized();

  assert(test);

  number_of_tests += 1;

  list_of_tests_push(tests, test);
  list_of_strings_push(proc_names, name);
}

static void write_test_index(Buffer *buffer, unsigned reserved_space, unsigned current_index)
{
  char *test_number = int_to_cstring(current_index);
    
  for (unsigned i = 0; i < reserved_space - strlen(test_number); i++)
  {
    buffer_push(buffer, '0');
  }

  buffer_push_all(buffer, test_number, strlen(test_number));

  free(test_number);
}

void test_runner(void)
{
  ensure_is_initialized();

  Buffer *buffer = create_buffer(1024, 1024);
  // @leak
  unsigned index_size_in_chars = strlen(int_to_cstring(number_of_tests));
  const int column_size = 80 - 1 - SIZE_OF_STATIC_STRING(" FAILED");

  for (unsigned i = 0; i < tests->index; i++)
  {
    // @todo João, resetar estrutura com contador de asserts e erros 
    current_state.at_least_one_failed = false;
    current_state.passed = 0;
    current_state.failed = 0;
    current_state.filename = NULL;
    current_state.line_number = 0;
    current_state.expr = NULL;
    
    Test_Proc test = tests->data[i];
    test();

    write_test_index(buffer, index_size_in_chars, i + 1);

    buffer_push_all(buffer, EXPAND_STRING_REF_AND_COUNT(" "));

    // @todo João, pendências
    // - número de testes com zero a esquerda (fazer o max dos testes registrados para saber quantos zeros)
    // - limitar o número de caracteres por linha
    // - em caso de erro reportar dados úteis (line number, filename, etc...)
    // - usar um print apenas e um buffer para montar a linha
    const char *name = proc_names->data[i];
    
    buffer_push_all(buffer, name, strlen(name));

    // @todo João, possível loop infinito(até o limite numérico do inteiro) por causa de underflow
    for (unsigned i = 0; i < (column_size - strlen(name) - index_size_in_chars - 1); i++)
    {
      buffer_push(buffer, '.');
    }

    if (current_state.at_least_one_failed)
    {
      number_of_failed_tests += 1;
      buffer_push_all(buffer, EXPAND_STRING_REF_AND_COUNT(RED " FAILED" RESET));
    }
    else
    {
      number_of_success_tests += 1;
      buffer_push_all(buffer, EXPAND_STRING_REF_AND_COUNT(GREEN " OK" RESET));
    }
    buffer_push(buffer, '\n');

    if (current_state.at_least_one_failed)
    {
      for (unsigned i = 0; i < index_size_in_chars + 1; i++)
      {
        buffer_push(buffer, ' ');
      }

      buffer_push_all(buffer, current_state.filename, strlen(current_state.filename));
      buffer_push(buffer, ':');

      char *line = int_to_cstring(current_state.line_number);
      buffer_push_all(buffer, line, strlen(line));
      free(line);
      buffer_push_all(buffer, EXPAND_STRING_REF_AND_COUNT(" Assertion: '"));
      buffer_push_all(buffer, current_state.expr, strlen(current_state.expr));
      buffer_push(buffer, '\'');
      buffer_push(buffer, '\n');
    }


    write(STDOUT_FILENO, buffer->buffer, buffer->index);

    buffer_clear(buffer);
  }

  // resumo
  printf("\n");
  printf("Total de testes: %d", number_of_tests);
  if (number_of_tests == number_of_success_tests)
  {
    printf(" - 100%% sucesso\n");
  }
  else
  {
    printf(" - %0.2f%% sucesso", number_of_success_tests / (float) number_of_tests * 100.00);
  }
}

#endif // _TEST_RUNNER_C_
