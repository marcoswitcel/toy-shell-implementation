#ifndef _TYPES_H_
#define _TYPES_H_

#include <stdlib.h>
#include <string.h>

#include "./buffer.h"

typedef enum ASCII_Control_Characters {
  BACKSPACE = 127,
  ESC = 27,
  FORM_FEED = 12,
} ASCII_Control_Characters;

#endif // _TYPES_H_
