#ifndef UTILS_MACRO_H
#define UTILS_MACRO_H

#define STRINGIFY(VALUE) #VALUE

#define SIZE_OF_ARRAY(array) (sizeof(array) / sizeof(array[0]))
#define SIZE_OF_STATIC_STRING(STRING) (SIZE_OF_ARRAY(STRING) - 1)

/**
 * @brief Utilitário criado para facilitar a passagem de string estáticas para a função de sistema
 * `write` e outras. Basicamente essa macro expande a string para uma lista de dois itens, separados 
 * por vírgula ideias para parâmetros que esperam o ponteiro e a contagem de elementos.
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

#endif // UTILS_MACRO_H
