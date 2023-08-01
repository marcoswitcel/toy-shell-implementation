#include <stdlib.h>
#include <stdio.h>

typedef enum Token_Type {
  UNINITIALIZED = 0, STRING = 1, GLOBBING = 2,
} Token_Type;

typedef struct String_Token {
  const char *cstring;
} String_Token;

typedef struct Globbing_Token {
  const char *cstring;
} Globbing_Token;

typedef struct Token {
  Token_Type type;
  union {
    String_Token string;
    Globbing_Token globbing;
  } data;
} Token;

typedef struct Sequence_Of_Tokens
{
  unsigned internal_memory_size;
  unsigned grouth_by;
  unsigned index;
  Token *sequence;
} Sequence_Of_Tokens;

Sequence_Of_Tokens *create_sequence_of_tokens(unsigned internal_memory_size, unsigned grouth_by)
{
  Sequence_Of_Tokens *new_list_of_tokens = (Sequence_Of_Tokens *) malloc(sizeof(Sequence_Of_Tokens));
  Token *sequence = (Token *) malloc(sizeof(Token) * internal_memory_size);
  
  new_list_of_tokens->sequence = sequence;
  new_list_of_tokens->internal_memory_size = internal_memory_size;
  new_list_of_tokens->grouth_by = grouth_by;
  new_list_of_tokens->index = 0;

  return new_list_of_tokens;
}

void destroy_sequence_of_tokens(Sequence_Of_Tokens *sequence_of_tokens)
{
  assert(sequence_of_tokens->sequence && "Sempre deve ser válido"); // @note poderia adicionar um guarda aqui

  free(sequence_of_tokens->sequence);
  free(sequence_of_tokens);
}

static inline bool sequence_of_tokens_ensure_enough_space(Sequence_Of_Tokens *sequence_of_tokens)
{
  if (sequence_of_tokens->index >= sequence_of_tokens->internal_memory_size)
  {
    sequence_of_tokens->internal_memory_size += sequence_of_tokens->grouth_by;
    Token *new_sequence = (Token *) realloc(sequence_of_tokens->sequence, sizeof(Token) * sequence_of_tokens->internal_memory_size);
    if (new_sequence)
    {
      sequence_of_tokens->sequence = new_sequence;
      return true;
    }
    else
    {
      printf("Erro alocando memória para buffer\n");
    }
  }

  return false;
}

bool push(Sequence_Of_Tokens *sequence_of_tokens, Token value)
{
  sequence_of_tokens_ensure_enough_space(sequence_of_tokens);

  sequence_of_tokens->sequence[sequence_of_tokens->index] = value;
  sequence_of_tokens->index++;

  return true;
}

bool sequence_of_tokens_pop(Sequence_Of_Tokens *sequence_of_tokens)
{
  if (sequence_of_tokens->index)
  {
    sequence_of_tokens->index--;
    return true;
  }
  return false;
}
