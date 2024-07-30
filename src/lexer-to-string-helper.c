#include <c-armyknife-lib.h>

// OUCH! This is a copy of the token_type_t so that we can get stuff
// to compile correctly without implementing condition compiltion
// yet..

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

/**
 * @function token_type_to_string
 *
 * Return a string representation of a token type.
 */
char* token_type_to_string(int type) {
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
