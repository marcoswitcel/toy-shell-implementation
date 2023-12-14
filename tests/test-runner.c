#include <stdio.h>
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

void test_runner(void)
{
  ensure_is_initialized();

  static const char green[] = "\x1B[32m";
  static const char reset[] = "\x1B[0m";
  
  printf("\n");

  for (unsigned i = 0; i < tests->index; i++)
  {
    const char *name = proc_names->data[i];
    Test_Proc test = tests->data[i];
    // @todo João, resetar estrutura com contador de asserts e erros 
    test();

    printf("%d %s ......................................... %sOK%s\n", i + 1, name, green, reset);
  }
}
