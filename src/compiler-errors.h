// SSCF generated file from: compiler-errors.c

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
  PARSE_ERROR_CLOSE_BRACKET_EXPECTED,
  PARSE_ERROR_UNRECOGNIZED_TOP_LEVEL_DECLARATION,
  PARSE_ERROR_SEMICOLON_EXPECTED,
  PARSE_ERROR_CONFLICTING_STORAGE_CLASS_SPECIFIER,
  PARSE_ERROR_CONFLICTING_FUNCTION_SPECIFIER,
  PARSE_ERROR_EXPECTED_OPEN_PAREN_AFTER_UNDERSCORE_ATTRIBUTE,
  PARSE_ERROR_EXPECTED_MATCHING_CLOSE_PAREN_AFTER_UNDERSCORE_ATTRIBUTE,
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
  char* file_name;
} compiler_error_t;

typedef struct {
  char* previous_lines;
  char* current_line;
  char* next_lines;
} src_code_snippets_t;

// Reinclude generated prototypes...
#include "compiler-errors.c.generated.h"

#endif /* _COMPILER_ERRORS_H_ */
