#ifndef _TEST_RUNNER_H_
#define _TEST_RUNNER_H_

#include <stdbool.h>

/**
 * @brief Estrutura interna usada para representar e conter o estado de um 
 * teste que foi executado.
 * @note João, uma possível melhoria seria adicionar o tempo por teste, mas
 * posso esperar um caso real de uso antes de decidir implementar.
 */
typedef struct Test_State {
  bool at_least_one_failed;
  unsigned passed;
  unsigned failed;
  const char *filename;
  const char *expr;
  unsigned line_number;
} Test_State;

Test_State current_state = { 0 };

/**
 * @brief Definição da função de teste. A transmissão de informação acontece através das rotinas 
 * de asserção.
 */
typedef void (*Test_Proc)(void);

/**
 * @brief Faz o registro de uma nova função de teste com o seu respectivo nome de apresentação.
 * Ver a macro `Register_Test` para uma versão de um argumento que usa o nome do símbolo como nome do teste.
 * 
 * @param test função de teste
 * @param name nome da função de teste, usado na apresentação do log de sucesso ou erro
 */
void register_test(Test_Proc test, const char *name);

/**
 * @brief Função responsável por disparar os testes.
 */
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
 * @brief Macro utilitária para facilitar a asserção de strings
 */
#define Assert_String_Equals(EXPR1, EXPR2) Assert(strcmp(EXPR1, EXPR2) == 0)

/**
 * @brief Macro utilitária para facilitar a asserção de nulo
 */
#define Assert_Is_Null(EXPR) Assert((EXPR) == NULL)

/**
 * @brief Macro utilitária para facilitar a asserção de não nulos
 */
#define Assert_Is_Not_Null(EXPR) Assert((EXPR) != NULL)

/**
 * @brief Facilita o registro de uma função usando o nome do símbolo como nome do teste
 * 
 */
#define Register_Test(FUNCTION_SYMBOL_NAME) register_test(FUNCTION_SYMBOL_NAME, # FUNCTION_SYMBOL_NAME); 

#endif // _TEST_RUNNER_H_
