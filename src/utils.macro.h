#ifndef UTILS_MACRO_H
#define UTILS_MACRO_H

#define STRINGIFY(VALUE) #VALUE

#define SIZE_OF_ARRAY(array) (sizeof(array) / sizeof(array[0]))
#define SIZE_OF_STATIC_STRING(STRING) (SIZE_OF_ARRAY(STRING) - 1)

#define EXPAND_STRING_REF_AND_COUNT(STRING) STRING, SIZE_OF_STATIC_STRING(STRING)

/**
 * @brief Como invocar free e apagar a antiga referência é uma boa prática essa macro encapsula o conceito
 * @param LHSREF expressão que resulta no endereço a ser liberado
 */
#define FREE_AND_NULLIFY(LHSREF) { free((void *) LHSREF); LHSREF = NULL; }

#define ALLOC(TYPE, HOW_MANY) ((TYPE *) malloc(sizeof(TYPE) * HOW_MANY))

#define NO_UNDERFLOW_SUBTRACTING(NUMBER, VALUE) (NUMBER >= VALUE)

#endif // UTILS_MACRO_H
