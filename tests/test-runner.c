#ifndef _TEST_RUNNER_C_
#define _TEST_RUNNER_C_

#include <stdio.h>
#include <stdlib.h>

#include "./test-runner.h"
#include "../src/list.macro.h"
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

void test_runner(void)
{
  ensure_is_initialized();
  
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

    // @todo João, pendências
    // - número de testes com zero a esquerda (fazer o max dos testes registrados para saber quantos zeros)
    // - limitar o número de caracteres por linha
    // - em caso de erro reportar dados úteis (line number, filename, etc...)
    // - usar um print apenas e um buffer para montar a linha
    const char *name = proc_names->data[i];
    // @note por hora estou usando padding do C 
    // @reference https://stackoverflow.com/questions/276827/string-padding-in-c
    if (current_state.at_least_one_failed)
    {
      printf("%04d %-69s %sFAILED%s\n", i + 1, name, RED, RESET);
      printf("     %s:%d Assertion: '%s'\n", current_state.filename, current_state.line_number, current_state.expr);
      number_of_failed_tests += 1;
    }
    else
    {
      number_of_success_tests += 1;
      printf("%04d %-69s %sOK%s\n", i + 1, name, GREEN, RESET);
    } 
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
