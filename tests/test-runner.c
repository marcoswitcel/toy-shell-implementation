#ifndef _TEST_RUNNER_C_
#define _TEST_RUNNER_C_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

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
  size_t test_number_length = strlen(test_number);
  
  if (reserved_space > test_number_length)
  {
    for (unsigned i = 0; i < reserved_space - test_number_length; i++)
    {
      buffer_push(buffer, '0');
    }
  }

  buffer_push_all(buffer, test_number, test_number_length);

  free(test_number);
}

static void print_time_to_buffer(Buffer *buffer, time_t time)
{
  char formatted_date_buff[20];
  strftime(formatted_date_buff, 20, "%Y-%m-%d %H:%M:%S", localtime(&time));

  buffer_push_all(buffer, formatted_date_buff, strlen(formatted_date_buff));
}

static inline unsigned get_index_size_in_chars(int number_of_tests)
{
  char* number_of_tests_as_text = int_to_cstring(number_of_tests);
  int result = strlen(number_of_tests_as_text);
  free(number_of_tests_as_text);
  
  return result;
}

/**
 * @note João, uma ideia interessante seria melhorar o report do teste, algumas formas que podería usar seriam.
 * Adicionar algum mecanismo para contar os testes, adicionar um mecanismo para medir o tempo, fazer os testes rodam independentes
 * uns dos outros, apresentar ao final quantidade de sucessos e falhas, junto ao tempo decorrido.
 * Printar uma linha para cada teste, talvez colorido? formatação bonita.
 */
void test_runner(void)
{
  printf("-------------------------------------------------------------------------\n");
  printf("|                           Executando Testes                           |\n");
  printf("-------------------------------------------------------------------------\n");

  const clock_t start_clock = clock(); 

  time_t start_time, end_time;
  time(&start_time);

  ensure_is_initialized();

  Buffer *buffer = create_buffer(1024, 1024);
  unsigned index_size_in_chars = get_index_size_in_chars(number_of_tests);
  const unsigned column_size = 80 - 1 - SIZE_OF_STATIC_STRING(" FAILED");

  buffer_push_all(buffer, EXPAND_STRING_REF_AND_COUNT("\nIniciado em: "));
  print_time_to_buffer(buffer, start_time);
  buffer_push_all(buffer, EXPAND_STRING_REF_AND_COUNT("\n\n"));

  for (unsigned i = 0; i < tests->index; i++)
  {
    current_state.at_least_one_failed = false;
    current_state.passed = 0;
    current_state.failed = 0;
    current_state.filename = NULL;
    current_state.line_number = 0;
    current_state.expr = NULL;
    
    const char *name = proc_names->data[i];
    size_t name_length = strlen(name);
    Test_Proc test = tests->data[i];
    test();

    buffer_push_all(buffer, EXPAND_STRING_REF_AND_COUNT(HI_BLACK));
    write_test_index(buffer, index_size_in_chars, i + 1);
    buffer_push_all(buffer, EXPAND_STRING_REF_AND_COUNT(RESET));

    buffer_push(buffer, ' ');
    
    buffer_push_all(buffer, name, name_length);

    buffer_push_all(buffer, EXPAND_STRING_REF_AND_COUNT(HI_BLACK));

    if (column_size > (name_length + index_size_in_chars + 1))
    {
      for (unsigned i = 0; i < (column_size - (name_length + index_size_in_chars + 1)); i++)
      {
        buffer_push(buffer, '.');
      }
    }
    buffer_push_all(buffer, EXPAND_STRING_REF_AND_COUNT(RESET));

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

  // O resumo começa daqui pra baixo

  time(&end_time);
  double diff = ((double) (clock() - start_clock)) / CLOCKS_PER_SEC; // @note poderia fazer dom `difftime`, mas tem pouca precisão

  buffer_push_all(buffer, EXPAND_STRING_REF_AND_COUNT("\nTerminado em: "));
  print_time_to_buffer(buffer, start_time);
  buffer_push_all(buffer, EXPAND_STRING_REF_AND_COUNT("\n"));

  write(STDOUT_FILENO, buffer->buffer, buffer->index);
  buffer_clear(buffer);

  printf("Tempo total: %f (segundos)\n\n", diff);

  printf("Total de testes: %d", number_of_tests);
  if (number_of_tests == number_of_success_tests)
  {
    printf("\n100%% sucesso\n");
  }
  else
  {
    printf("\n%0.2f%% sucesso", number_of_success_tests / (float) number_of_tests * 100.00);
  }
}

#endif // _TEST_RUNNER_C_
