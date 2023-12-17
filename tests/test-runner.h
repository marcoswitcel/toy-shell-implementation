#ifndef _TEST_RUNNER_H_
#define _TEST_RUNNER_H_

#include <stdbool.h>

typedef void (*Test_Proc)(void);

void register_test(Test_Proc test, const char *name);
void test_runner(void);

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

#endif // _TEST_RUNNER_H_
