#ifndef _TEST_RUNNER_C_
#define _TEST_RUNNER_C_

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "../src/list.macro.h"
#include "../src/list.implementations.h"

typedef void (*Test_Proc)(void);


static int number_of_tests = 0;


MAKE_LIST_IMPLEMENTATION(List_Of_Tests, list_of_tests, Test_Proc)

static List_Of_Tests *tests = NULL;
static List_Of_Strings *proc_names = NULL;

void ensure_is_initialized()
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

/**
 * @brief Mecanismo que faz a asserção no framework de teste
 * 
 */
#define Assert(EXPR) {                        \
  if (EXPR) {                                 \
    current_state.passed += 1;                \
  }                                           \
  else                                        \
  {                                           \
    current_state.failed += 1;                \
    current_state.at_least_one_failed = true; \
    current_state.filename = __FILE__;        \
    current_state.expr = # EXPR;              \
    current_state.line_number = __LINE__;     \
    return;                                   \
  }                                           \
}

/**
 * @brief Facilita o registro de uma função usando seu nome como nome do teste
 * 
 */
#define Register_Test(FUNCTION_SYMBOL_NAME) register_test(FUNCTION_SYMBOL_NAME, # FUNCTION_SYMBOL_NAME); 

void test_runner(void)
{
  ensure_is_initialized();

  static const char green[] = "\x1B[32m";
  static const char red[] =  "\x1B[31m";
  static const char reset[] = "\x1B[0m";
  
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
      printf("%04d %-69s %sFAILED%s\n", i + 1, name, red, reset);
      printf("     %s:%d Assertion: '%s'\n", current_state.filename, current_state.line_number, current_state.expr);
    }
    else
    {
      printf("%04d %-69s %sOK%s\n", i + 1, name, green, reset);
    } 
  }
}

#endif // _TEST_RUNNER_C_
