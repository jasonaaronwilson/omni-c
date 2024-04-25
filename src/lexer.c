#line 2 "lexer.c"
#ifndef _LEXER_H_
#define _LEXER_H_

#include <c-armyknife-lib.h>

/**
 * @file lexer
 *
 * This is hand-written tokenizer for Omni C (so should work fine for
 * C and C++).
 */

/**
 * @enum token_type
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

/**
 * @enum tokenizer_error
 *
 * Represents
 */
typedef enum {
  TOKENIZER_ERROR_UNKNOWN,
  TOKENIZER_ERROR_UTF_DECODE_ERROR
} tokenizer_error_t;

struct oc_token_S {
  buffer_t* buffer;
  token_type_t type;
  int32_t start;
  int32_t end;
};

typedef struct oc_token_S oc_token_t;

struct oc_tokenizer_result_S {
  value_array_t* tokens;
  uint64_t tokenizer_error_position;
  tokenizer_error_t tokenizer_error_code;
};

typedef struct oc_tokenizer_result_S oc_tokenizer_result_t;

char* token_to_string(oc_token_t token);

char* token_type_to_string(token_type_t type);

buffer_t* append_token_debug_string(buffer_t* buffer, oc_token_t token);

oc_tokenizer_result_t tokenize(buffer_t* buffer);

#endif /* _LEXER_H_ */

/**
 * @function token_to_string
 *
 * Return the token as a character string.
 */
char* token_to_string(oc_token_t token) {
  return buffer_c_substring(token.buffer, token.start, token.end);
}

/**
 * @function token_type_to_string
 *
 * Return a string representation of a token type.
 */
char* token_type_to_string(token_type_t type) {
  switch (type) {
  case TOKEN_TYPE_WHITESPACE:
    return "TOKEN_TYPE_WHITESPACE";
  case TOKEN_TYPE_COMMENT:
    return "TOKEN_TYPE_COMMENT";
  case TOKEN_TYPE_IDENTIFIER:
    return "TOKEN_TYPE_IDENTIFIER";
  case TOKEN_TYPE_PUNCTUATION:
    return "TOKEN_TYPE_PUNCTUATION";
  case TOKEN_TYPE_INTEGER_LITERAL:
    return "TOKEN_TYPE_INTEGER_LITERAL";
  case TOKEN_TYPE_FLOAT_LITERAL:
    return "TOKEN_TYPE_FLOAT_LITERAL";
  case TOKEN_TYPE_STRING_LITERAL:
    return "TOKEN_TYPE_STRING_LITERAL";
  case TOKEN_TYPE_CHARACTER_LITERAL:
    return "TOKEN_TYPE_CHARACTER_LITERAL";
  }
  fatal_error(ERROR_ILLEGAL_STATE);
}

/**
 * @function append_token_debug_string
 *
 * Append a debug representation of a token to the buffer that looks
 * roughly like this:
 *
 * type: TOKEN_TYPE_IDENTIFIER start: 10 end: 15 str: asdf
 */
buffer_t* append_token_debug_string(buffer_t* buffer, oc_token_t token) {
  char* str = token_to_string(token);
  buffer = buffer_printf(buffer, "type: %s start: %d end: %d str: %s",
                         token_type_to_string(token.type), token.start,
                         token.end, str);
  free_bytes(str);
  return buffer;
}

/**
 * @function tokenize
 *
 * Return a value array of the tokens OR return an error (such as when
 * a character or string literal is unterminated).
 */
oc_tokenizer_result_t tokenize(buffer_t* buffer) {
  oc_tokenizer_result_t result = {0};
  uint32_t pos = 0;
  while (pos < buffer_length(buffer)) {
    utf8_decode_result_t decode_result = buffer_utf8_decode(buffer, pos);
    if (decode_result.error) {
      return (oc_tokenizer_result_t){.tokenizer_error_code
                                     = TOKENIZER_ERROR_UTF_DECODE_ERROR};
    }
    pos += decode_result.num_bytes;
  }
  return result;
}
