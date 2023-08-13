#ifndef UTILS_MACRO_H
#define UTILS_MACRO_H

#define STRINGIFY(VALUE) #VALUE

#define SIZE_OF_ARRAY(array) (sizeof(array) / sizeof(array[0]))
#define SIZE_OF_STATIC_STRING(STRING) (SIZE_OF_ARRAY(STRING) - 1)

#endif // UTILS_MACRO_H
