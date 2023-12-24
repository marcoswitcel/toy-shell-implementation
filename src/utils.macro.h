#ifndef UTILS_MACRO_H
#define UTILS_MACRO_H

#define STRINGIFY(VALUE) #VALUE

#define SIZE_OF_ARRAY(array) (sizeof(array) / sizeof(array[0]))
#define SIZE_OF_STATIC_STRING(STRING) (SIZE_OF_ARRAY(STRING) - 1)

/**
 * @brief Utilitário criado para facilitar a passagem de string estáticas para a função de sistema
 * `write` e outras. Basicamente essa macro expande a string para uma lista de dois itens, separados 
 * por vírgula ideais para parâmetros que esperam o ponteiro e a contagem de elementos.
 * Como uso a macro SIZE_OF_STATIC_STRING fica garantido que o número de bytes escritos será o correto
 * não exigindo alteração manual. E possivelmente o otimizador consiga pegar isso pois se trata de uma expressão
 * constante.
 */
#define EXPAND_STRING_REF_AND_COUNT(STRING) STRING, SIZE_OF_STATIC_STRING(STRING)

/**
 * @brief Como invocar free e apagar a antiga referência é uma boa prática essa macro encapsula o conceito
 * @param LHSREF expressão que resulta no endereço a ser liberado
 */
#define FREE_AND_NULLIFY(LHSREF) { free((void *) LHSREF); LHSREF = NULL; }

#define ALLOC(TYPE, HOW_MANY) ((TYPE *) malloc(sizeof(TYPE) * HOW_MANY))

#define NO_UNDERFLOW_SUBTRACTING(NUMBER, VALUE) (NUMBER >= VALUE)

#define CONCAT_EXPANSION(A, B) A ## B 
#define CONCAT(A, B) CONCAT_EXPANSION(A, B)

/**
 * @brief concatena o símbolo ao número da linha para ter nomes de variáveis com menos
 * chance de colisão.
 * 
 * @link https://stackoverflow.com/questions/1082192/how-to-generate-random-variable-names-in-c-using-macros
 */
#define SUFFIXED_NAME(NAME) CONCAT(NAME, __LINE__)

/**
 * @brief Macro para fazer swap de valores em dois endereços de memória
 * Depende da função memcpy e da macro assert, não faço o import aqui. Deixo a critério de quem usar a macro
 * 
 * Apenas para referência:
 * @link https://stackoverflow.com/questions/3982348/implement-generic-swap-macro-in-c/3982430#3982430
 * @link https://stackoverflow.com/questions/8862136/is-there-a-built-in-swap-function-in-c
 * @link https://cplusplus.com/reference/algorithm/swap/
 */
#define SWAP(A, B) \
{\
  assert(sizeof(A) == sizeof(B) && "Os tamanho deveriam ser idênticos para o swap funcionar corretamente"); \
  unsigned char SUFFIXED_NAME(__temp__)[sizeof(A)]; \
  memcpy(SUFFIXED_NAME(__temp__), &A, sizeof(A));\
  memcpy(&A, &B, sizeof(A));\
  memcpy(&B, SUFFIXED_NAME(__temp__), sizeof(A));\
}

#endif // UTILS_MACRO_H
