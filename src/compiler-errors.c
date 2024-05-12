#line 2 "compiler-errors.c"
#ifndef _COMPILER_ERRORS_H_
#define _COMPILER_ERRORS_H_

#include <c-armyknife-lib.h>

/**
 * @enum tokenizer_error_t
 *
 * Represents
 */
typedef enum {
  TOKENIZER_ERROR_UNKNOWN,
  TOKENIZER_ERROR_UTF_DECODE_ERROR,
  TOKENIZER_ERROR_UNRECOGNIZED_PUNCTUATION,
  TOKENIZER_ERROR_UNTERMINATED_COMMENT,
  TOKENIZER_ERROR_UNTERMINATED_STRING_LITERAL,
  TOKENIZER_ERROR_UNTERMINATED_CHARACTER_LITERL
} tokenizer_error_t;

/**
 * @enum parse_error_code_t
 *
 * Represents a particular type of parse error so that a particular
 * error template can be used to present these errors to the user.
 */
typedef enum {
  PARSE_ERROR_UNKNOWN,
  PARSE_ERROR_COMMA_OR_EQUAL_SIGN_EXPECTED,
  PARSE_ERROR_EXPECTED_FIELD_WIDTH_OR_SEMICOLON,
  PARSE_ERROR_EXPECTED_SEMICOLON,
  PARSE_ERROR_IDENTIFIER_EXPECTED,
  PARSE_ERROR_INTEGER_LITERAL_EXPECTED,
  PARSE_ERROR_OPEN_BRACE_EXPECTED,
  PARSE_ERROR_UNRECOGNIZED_TOP_LEVEL_DECLARATION,
} parse_error_code_t;

struct oc_token_S;

/**
 * @struct compiler_error_t
 *
 * Represents errors in many phases of the compiler.
 */
typedef struct compiler_error_S {
  tokenizer_error_t tokenizer_error_code;
  parse_error_code_t parser_error_code;
  struct oc_token_S* error_token;
} compiler_error_t;

buffer_t* buffer_append_human_readable_error(buffer_t* buffer,
                                             compiler_error_t* error);

#endif /* _COMPILER_ERRORS_H_ */

/**
 * @function buffer_append_human_readable_error
 *
 * Append a human readable version of the error to the buffer.
 */
buffer_t* buffer_append_human_readable_error(buffer_t* buffer,
                                             compiler_error_t* error) {
  return buffer;
}
