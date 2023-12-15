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

#include "./test-runner.c"
#include "../src/types.h"
#include "../src/tokens.h"
#include "../src/parser.h"
#include "../src/list.implementations.h"
#include "../src/utils.macro.h"
#include "../src/shell.c"
#include "./buffer.test.c"
#include "./sorting.test.c"
#include "./utils.test.c"
#include "./parser.test.c"

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

  list_of_strings_pop_at(list, 0);
  assert(list->index == 1);

  list_of_strings_pop_at(list, 0);
  assert(list->index == 0);

  list_of_strings_pop_at(list, 0);
  assert(list->index == 0);

  const char *other_list[] = {
    "banana",
    "maçã",
    "abacaxi",
    "abacate",
  };

  list_of_strings_push_all(list, other_list, SIZE_OF_ARRAY(other_list));
  assert(list->index == SIZE_OF_ARRAY(other_list));
}

void test_list_char_prt_allocation_implementation(void)
{
  List_Of_Strings *list = create_list_of_strings(1, 1);
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

  list_of_strings_pop_at(list, 0);
  assert(list->index == 1);

  list_of_strings_pop_at(list, 0);
  assert(list->index == 0);

  list_of_strings_pop_at(list, 0);
  assert(list->index == 0);

  const char *other_list[] = {
    "banana",
    "maçã",
    "abacaxi",
    "abacate",
  };

  list_of_strings_push_all(list, other_list, SIZE_OF_ARRAY(other_list));
  assert(list->index == SIZE_OF_ARRAY(other_list));
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

void test_shell_parse_command01(void)
{
  const char parse_input_sample[] = "echo teste > arquivo.txt";

  Parse_Context context = create_parse_context(parse_input_sample);
  assert(context.error == NULL);
  assert(context.error_start_index == -1);
  assert(context.index == 0);
  assert(context.length == strlen(parse_input_sample));

  Execute_Command_Node execute_command_node = shell_parse_command(&context);
  assert(context.error == NULL);
  assert(context.error_start_index == -1);
  assert(context.index == strlen(parse_input_sample));

  
  assert(execute_command_node.args != NULL);

  assert(execute_command_node.args[0] != NULL);
  assert(strcmp(execute_command_node.args[0], "echo") == 0);
  assert(execute_command_node.args[1] != NULL);
  assert(strcmp(execute_command_node.args[1], "teste") == 0);
  assert(execute_command_node.args[2] == NULL);

  assert(execute_command_node.append_mode == false);
  assert(execute_command_node.next_command == NULL);
  assert(strcmp(execute_command_node.stdout_redirect_filename, "arquivo.txt") == 0);
  assert(strcmp(execute_command_node.stderr_redirect_filename, "arquivo.txt") == 0);
  assert(execute_command_node.pipe == NULL);
  // @todo João, curiosamente não lembrava que esse atributo aponta para o primeiro caractere
  // do último token parseado, nesse caso a string "arquivo.txt"
  assert(execute_command_node.token_index_start == 13);
}

void test_shell_parse_command02(void)
{
  const char parse_input_sample[] = "echo teste && echo teste2";

  Parse_Context context = create_parse_context(parse_input_sample);
  assert(context.error == NULL);
  assert(context.error_start_index == -1);
  assert(context.index == 0);
  assert(context.length == strlen(parse_input_sample));

  Execute_Command_Node execute_command_node = shell_parse_command(&context);
  assert(context.error == NULL);
  assert(context.error_start_index == -1);
  assert(context.index == strlen(parse_input_sample));

  assert(execute_command_node.args != NULL);

  assert(execute_command_node.args[0] != NULL);
  assert(strcmp(execute_command_node.args[0], "echo") == 0);
  assert(execute_command_node.args[1] != NULL);
  assert(strcmp(execute_command_node.args[1], "teste") == 0);
  assert(execute_command_node.args[2] == NULL);

  assert(execute_command_node.append_mode == false);
  assert(execute_command_node.next_command != NULL);
  assert(execute_command_node.stderr_redirect_filename == NULL);
  assert(execute_command_node.stdout_redirect_filename == NULL);
  assert(execute_command_node.pipe == NULL);
  assert(execute_command_node.token_index_start == -1); // @todo João, isso aqui está inconsistente com o caso '01'

  // o comando AND a seguir
  assert(execute_command_node.next_command->args != NULL);

  assert(execute_command_node.next_command->args[0] != NULL);
  assert(strcmp(execute_command_node.next_command->args[0], "echo") == 0);
  assert(execute_command_node.next_command->args[1] != NULL);
  assert(strcmp(execute_command_node.next_command->args[1], "teste2") == 0);
  assert(execute_command_node.next_command->args[2] == NULL);

  assert(execute_command_node.next_command->append_mode == false);
  assert(execute_command_node.next_command->next_command == NULL);
  assert(execute_command_node.next_command->stderr_redirect_filename == NULL);
  assert(execute_command_node.next_command->stdout_redirect_filename == NULL);
  assert(execute_command_node.next_command->pipe == NULL);
  assert(execute_command_node.next_command->token_index_start == -1); // @todo João, isso aqui está inconsistente com o caso '01'
}

void test_shell_parse_command03(void)
{
  const char parse_input_sample[] = "echo primeiro && echo segundo | grep segundo && echo terceiro";

  Parse_Context context = create_parse_context(parse_input_sample);
  assert(context.error == NULL);
  assert(context.error_start_index == -1);
  assert(context.index == 0);
  assert(context.length == strlen(parse_input_sample));

  Execute_Command_Node first_command = shell_parse_command(&context);
  assert(context.error == NULL);
  assert(context.error_start_index == -1);
  assert(context.index == strlen(parse_input_sample));

  assert(first_command.args != NULL);

  assert(first_command.args[0] != NULL);
  assert(strcmp(first_command.args[0], "echo") == 0);
  assert(first_command.args[1] != NULL);
  assert(strcmp(first_command.args[1], "primeiro") == 0);
  assert(first_command.args[2] == NULL);

  assert(first_command.append_mode == false);
  assert(first_command.next_command != NULL);
  assert(first_command.stderr_redirect_filename == NULL);
  assert(first_command.stdout_redirect_filename == NULL);
  assert(first_command.pipe == NULL);
  assert(first_command.token_index_start == -1); // @todo João, isso aqui está inconsistente com o caso '01'

  // o comando AND a seguir
  Execute_Command_Node *second_command = first_command.next_command;
  assert(second_command->args != NULL);

  assert(second_command->args[0] != NULL);
  assert(strcmp(second_command->args[0], "echo") == 0);
  assert(second_command->args[1] != NULL);
  assert(strcmp(second_command->args[1], "segundo") == 0);
  assert(second_command->args[2] == NULL);

  assert(second_command->append_mode == false);
  assert(second_command->next_command);
  assert(second_command->stderr_redirect_filename == NULL);
  assert(second_command->stdout_redirect_filename == NULL);
  assert(second_command->pipe);
  assert(second_command->token_index_start == -1); // @todo João, isso aqui está inconsistente com o caso '01'
  
  // pipe
  Execute_Command_Node *second_command_pipe = second_command->pipe;
  assert(second_command_pipe->args != NULL);

  assert(second_command_pipe->args[0] != NULL);
  assert(strcmp(second_command_pipe->args[0], "grep") == 0);
  assert(second_command_pipe->args[1] != NULL);
  assert(strcmp(second_command_pipe->args[1], "segundo") == 0);
  assert(second_command_pipe->args[2] == NULL);

  assert(second_command_pipe->append_mode == false);
  assert(second_command_pipe->next_command == NULL);
  assert(second_command_pipe->stderr_redirect_filename == NULL);
  assert(second_command_pipe->stdout_redirect_filename == NULL);
  assert(second_command_pipe->pipe == NULL);
  assert(second_command_pipe->token_index_start == -1); // @todo João, isso aqui está inconsistente com o caso '01'

  // o comando AND a seguir
  Execute_Command_Node *third_command = first_command.next_command->next_command;
  assert(third_command->args != NULL);

  assert(third_command->args[0] != NULL);
  assert(strcmp(third_command->args[0], "echo") == 0);
  assert(third_command->args[1] != NULL);
  assert(strcmp(third_command->args[1], "terceiro") == 0);
  assert(third_command->args[2] == NULL);

  assert(third_command->append_mode == false);
  assert(third_command->next_command == NULL);
  assert(third_command->stderr_redirect_filename == NULL);
  assert(third_command->stdout_redirect_filename == NULL);
  assert(third_command->pipe == NULL);
  assert(third_command->token_index_start == -1); // @todo João, isso aqui está inconsistente com o caso '01'
}

void test_skip_word_to_the_left_01(void)
{
  const char command[] = "echo teste";
  const unsigned size = SIZE_OF_STATIC_STRING(command);
  unsigned cursor = size;
  unsigned new_cursor_position;

  new_cursor_position = skip_word_to_the_left(command, size, cursor);
  assert(new_cursor_position != cursor);
  assert(new_cursor_position == 5);

  cursor = new_cursor_position;

  new_cursor_position = skip_word_to_the_left(command, size, cursor);
  assert(new_cursor_position != cursor);
  assert(new_cursor_position == 0);
}

void test_skip_word_to_the_left_02(void)
{
  const char command[] = "echo teste | grep       te  ";
  const unsigned size = SIZE_OF_STATIC_STRING(command);
  unsigned cursor = size;
  unsigned new_cursor_position;

  new_cursor_position = skip_word_to_the_left(command, size, cursor);
  assert(new_cursor_position != cursor);
  assert(new_cursor_position == 24);

  cursor = new_cursor_position;

  new_cursor_position = skip_word_to_the_left(command, size, cursor);
  assert(new_cursor_position != cursor);
  assert(new_cursor_position == 13);

  cursor = new_cursor_position;

  new_cursor_position = skip_word_to_the_left(command, size, cursor);
  assert(new_cursor_position != cursor);
  assert(new_cursor_position == 11);

  cursor = new_cursor_position;

  new_cursor_position = skip_word_to_the_left(command, size, cursor);
  assert(new_cursor_position != cursor);
  assert(new_cursor_position == 5);

  cursor = new_cursor_position;

  new_cursor_position = skip_word_to_the_left(command, size, cursor);
  assert(new_cursor_position != cursor);
  assert(new_cursor_position == 0);

  cursor = new_cursor_position;

  new_cursor_position = skip_word_to_the_left(command, size, cursor);
  assert(new_cursor_position == 0);
}

void test_skip_word_to_the_right_01(void)
{
  const char command[] = "echo teste | grep       te  ";
  const unsigned size = SIZE_OF_STATIC_STRING(command);
  unsigned cursor = 0;
  unsigned new_cursor_position;

  new_cursor_position = skip_word_to_the_right(command, size, cursor);
  assert(new_cursor_position != cursor);
  assert(new_cursor_position == 4);

  cursor = new_cursor_position;

  new_cursor_position = skip_word_to_the_right(command, size, cursor);
  assert(new_cursor_position != cursor);
  assert(new_cursor_position == 10);

  cursor = new_cursor_position;

  new_cursor_position = skip_word_to_the_right(command, size, cursor);
  assert(new_cursor_position != cursor);
  assert(new_cursor_position == 12);

  cursor = new_cursor_position;

  new_cursor_position = skip_word_to_the_right(command, size, cursor);
  assert(new_cursor_position != cursor);
  assert(new_cursor_position == 17);

  cursor = new_cursor_position;

  new_cursor_position = skip_word_to_the_right(command, size, cursor);
  assert(new_cursor_position != cursor);
  assert(new_cursor_position == 26);

  cursor = new_cursor_position;

  new_cursor_position = skip_word_to_the_right(command, size, cursor);
  assert(new_cursor_position == 28);
  
  cursor = new_cursor_position;

  new_cursor_position = skip_word_to_the_right(command, size, cursor);
  assert(new_cursor_position == 28);  
}

// @todo João, testar PIPE (aguardar até ele estar funcional)

// @todo João, reestruturar o teste do parse command para testar essa função também `parse_execute_command_node`

void test_testing_runner_pass(void)
{
  Assert(2 > 1);
}

void test_testing_runner_fails(void)
{
  Assert(2 < 1);
}

int main(void)
{
  /**
   * @note João, uma ideia interessante seria melhorar o report do teste, algumas formas que podería usar seriam.
   * Adicionar algum mecanismo para contar os testes, adicionar um mecanismo para medir o tempo, fazer os testes rodam independentes
   * uns dos outros, apresentar ao final quantidade de sucessos e falhas, junto ao tempo decorrido.
   * Printar uma linha para cada teste, talvez colorido? formatação bonita.
   */
  printf("Executando testes\n");

  test_list_char_prt_implementation();
  test_list_char_prt_allocation_implementation();
  test_list_of_floats_implementation();
  test_shell_parse_command01();
  test_shell_parse_command02();
  test_shell_parse_command03();
  test_skip_word_to_the_left_01();
  test_skip_word_to_the_left_02();
  test_skip_word_to_the_right_01();
  test_suit_buffer();
  test_suit_sorting();
  test_suit_utils();
  test_suit_parser();

  printf("Testes executados com sucesso! Nenhum erro detectado.");

  // @todo João, trabalho em progresso
  // Register_Test(test_testing_runner_pass);
  // Register_Test(test_testing_runner_fails);  
  // test_runner();

  return EXIT_SUCCESS;
}
