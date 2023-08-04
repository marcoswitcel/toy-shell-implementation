#ifndef LIST_IMPLEMENTATIONS_H
#define LIST_IMPLEMENTATIONS_H

#include "./list.macro.h"
#include "./tokens.h"

MAKE_LIST_IMPLEMENTATION(List_Of_Strings, list_of_strings, const char *)
MAKE_LIST_IMPLEMENTATION(List_Of_Floats, list_of_floats, float)
MAKE_LIST_IMPLEMENTATION(Sequence_Of_Tokens, sequence_of_tokens, Token)

#endif // LIST_IMPLEMENTATIONS_H
