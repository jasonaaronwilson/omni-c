#line 2 "compiler-errors.c"
#ifndef _PARSER_ERRORS_H_
#define _PARSER_ERRORS_H_

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
 * @struct parse_error_t
 *
 * This is the common return result for the various node parse
 * functions.
 */
typedef struct parse_error_S {
  parse_error_code_t error_code;
  struct oc_token_S* error_token;
} parse_error_t;

#endif /* _PARSER_ERRORS_H_ */
