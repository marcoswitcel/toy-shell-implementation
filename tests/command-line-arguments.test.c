#include  <stddef.h>

#include "./test-runner.h"
#include "../src/utils.macro.h"
#include "../src/command-line-arguments.c"

void test_command_line_arguments_apply_argv01()
{
  const char *argv[] = {
    "--colorful",
  };
  int argc = SIZE_OF_ARRAY(argv);

  Command_Line_Arguments arguments = { 0 };

  command_line_arguments_apply_argv(&arguments, argc, argv);
  Assert_Is_True(arguments.colorful);
  Assert_Is_False(arguments.no_sound);
}

void test_command_line_arguments_apply_argv02()
{
  const char *argv[] = {
    "--no_sound",
    "--colorful",
  };
  int argc = SIZE_OF_ARRAY(argv);

  Command_Line_Arguments arguments = { 0 };

  command_line_arguments_apply_argv(&arguments, argc, argv);
  Assert_Is_True(arguments.no_sound);
  Assert_Is_True(arguments.colorful);
}

void test_command_line_arguments_apply_argv03()
{
  const char *argv[] = {
    "--no_sound",
  };
  int argc = SIZE_OF_ARRAY(argv);

  Command_Line_Arguments arguments = { .colorful = true };

  command_line_arguments_apply_argv(&arguments, argc, argv);
  Assert_Is_False(arguments.colorful);
  Assert_Is_True(arguments.no_sound);
}

void test_command_line_arguments_apply_argv04()
{
  const char *argv[] = {
    "--no_sound",
    "--help",
  };
  int argc = SIZE_OF_ARRAY(argv);

  Command_Line_Arguments arguments = { 0 };

  command_line_arguments_apply_argv(&arguments, argc, argv);
  Assert_Is_True(arguments.no_sound);
  Assert_Is_True(arguments.help);
  Assert_Is_False(arguments.colorful);
}

extern void test_suit_test_command_line_arguments()
{
  Register_Test(test_command_line_arguments_apply_argv01);
  Register_Test(test_command_line_arguments_apply_argv02);
  Register_Test(test_command_line_arguments_apply_argv03);
  Register_Test(test_command_line_arguments_apply_argv04);
}
