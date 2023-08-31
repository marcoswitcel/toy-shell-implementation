#ifndef UTILS_MACRO_H
#define UTILS_MACRO_H

#define STRINGIFY(VALUE) #VALUE

#define SIZE_OF_ARRAY(array) (sizeof(array) / sizeof(array[0]))
#define SIZE_OF_STATIC_STRING(STRING) (SIZE_OF_ARRAY(STRING) - 1)

/**
 * @brief Como invocar free e apagar a antiga referência é uma boa prática essa macro encapsula o conceito
 * @param LHSREF expressão que resulta no endereço a ser liberado
 */
#define FREE_AND_NULLIFY(LHSREF) { free((void *) LHSREF); LHSREF = NULL; }

#define ALLOC(TYPE, HOW_MANY) ((TYPE *) malloc(sizeof(TYPE) * HOW_MANY))

#endif // UTILS_MACRO_H
