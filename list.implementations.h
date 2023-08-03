#ifndef LIST_IMPLEMENTATIONS_H
#define LIST_IMPLEMENTATIONS_H

#include "./list.macro.h"

MAKE_LIST_IMPLEMENTATION(List_Of_Strings, list_of_strings, const char *)
MAKE_LIST_IMPLEMENTATION(List_Of_Floats, list_of_floats, float)

#endif // LIST_IMPLEMENTATIONS_H
