// SSCF generated file from: lexer.c

#line 2 "lexer.c"
#ifndef _LEXER_H_
#define _LEXER_H_

#include "compiler-errors.h"
#include <c-armyknife-lib.h>
#include <ctype.h>

/**
 * @file lexer
 *
 * Break up a UTF-8 byte sequence into an array C or C++ tokens.
 */

/**
 * @enum token_type_t
 *
 * Represents a broad classification of a Omni C tokens.
 */
typedef enum {
  TOKEN_TYPE_UNKNOWN,
  TOKEN_TYPE_WHITESPACE,
  TOKEN_TYPE_COMMENT,
  TOKEN_TYPE_IDENTIFIER,
  TOKEN_TYPE_PUNCTUATION,
  TOKEN_TYPE_INTEGER_LITERAL,
  TOKEN_TYPE_FLOAT_LITERAL,
  TOKEN_TYPE_STRING_LITERAL,
  TOKEN_TYPE_CHARACTER_LITERAL
} token_type_t;

struct oc_token_S {
  buffer_t* buffer;
  token_type_t type;
  int32_t start;
  int32_t end;
  int32_t line_number;
  int32_t column_number;
};

typedef struct oc_token_S oc_token_t;

struct oc_tokenizer_result_S {
  value_array_t* tokens;
  uint64_t tokenizer_error_position;
  tokenizer_error_t tokenizer_error_code;
};

typedef struct oc_tokenizer_result_S oc_tokenizer_result_t;

static inline oc_token_t* token_at(value_array_t* tokens, uint64_t position) {
  if (position >= tokens->length) {
    return NULL;
  }
  return cast(oc_token_t*, value_array_get(tokens, position).ptr);
}

static inline boolean_t token_matches(oc_token_t* token, char* str) {
  int str_len = strlen(str);
  return str_len == (token->end - token->start)
         && buffer_match_string_at(token->buffer, token->start, str);
}

static inline boolean_t token_starts_with(oc_token_t* token, char* str) {
  return buffer_match_string_at(token->buffer, token->start, str);
}

static inline boolean_t token_contains(oc_token_t* token, char* str) {
  return buffer_region_contains(token->buffer, token->start, token->end, str);
}

/**
 * @struct token_or_error_t
 *
 * Allow each "sub-tokenizer" to return either a token (or an error)
 * for each token.
 */
struct token_or_error_S {
  oc_token_t token;
  tokenizer_error_t error_code;
  uint64_t error_position;
};

typedef struct token_or_error_S token_or_error_t;

#include "lexer.c.generated.h"

#endif /* _LEXER_H_ */
