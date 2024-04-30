#line 2 "lexer.c"
#ifndef _LEXER_H_
#define _LEXER_H_

#include <c-armyknife-lib.h>
#include <ctype.h>

/**
 * @file lexer
 *
 * Break up a UTF-8 byte sequence into an array C tokens.
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

/**
 * @enum tokenizer_error
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

oc_tokenizer_result_t tokenize(buffer_t* buffer, boolean_t include_whitespace,
                               boolean_t include_comments);

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

/* ========================================================================= */
// Everything after here supports the "tokenize" function which I'm
// ordering like this to avoid having to write prototypes (though
// technically either token_or_error_t shouldn't exist or we should
// have some kind of file_private annotation on it... Or
// namespaces...)
/* ========================================================================= */

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

/* ========================================================================= */
// Tokenize whitespace
/* ========================================================================= */

/**
 * @function tokenize_whitespace
 *
 * Read white-space until the buffer is exhausted or a non-whitespace
 * UTF-8 code point is read.
 */
token_or_error_t tokenize_whitespace(buffer_t* buffer,
                                     uint64_t start_position) {
  uint64_t pos = start_position;
  while (pos < buffer_length(buffer)) {
    utf8_decode_result_t decode_result = buffer_utf8_decode(buffer, pos);
    if (decode_result.error) {
      return (token_or_error_t){.error_code = TOKENIZER_ERROR_UTF_DECODE_ERROR,
                                .error_position = pos};
    }
    uint32_t code_point = decode_result.code_point;
    if (!isspace(code_point)) {
      break;
    }
    pos += decode_result.num_bytes;
  }

  return (token_or_error_t){.token = (oc_token_t){.buffer = buffer,
                                                  .type = TOKEN_TYPE_WHITESPACE,
                                                  .start = start_position,
                                                  .end = pos}};
}

/* ========================================================================= */
// Tokenize identifiers, keywords, etc.
/* ========================================================================= */

boolean_t is_identifier_start(uint32_t code_point) {
  // TODO(jawilson): it would be very cool to be able to try
  // converting this into different forms and see how different C
  // compilers perform in a benchmark. We could include different
  // standard libraries...
  switch (code_point) {
  case '$':
    return true;
  case '_':
    return true;
  default:
    return isalpha(code_point);
  }
}

/**
 * @function tokenize_identifier
 *
 * Read the next C identifier/keyword or return an error.
 */
token_or_error_t tokenize_identifier(buffer_t* buffer,
                                     uint64_t start_position) {
  uint64_t pos = start_position;
  while (pos < buffer_length(buffer)) {
    utf8_decode_result_t decode_result = buffer_utf8_decode(buffer, pos);
    if (decode_result.error) {
      return (token_or_error_t){.error_code = TOKENIZER_ERROR_UTF_DECODE_ERROR,
                                .error_position = pos};
    }
    uint32_t code_point = decode_result.code_point;
    if (!(is_identifier_start(code_point) || isdigit(code_point))) {
      break;
    }
    pos += decode_result.num_bytes;
  }

  return (token_or_error_t){.token = (oc_token_t){.buffer = buffer,
                                                  .type = TOKEN_TYPE_IDENTIFIER,
                                                  .start = start_position,
                                                  .end = pos}};
}

token_or_error_t tokenize_numeric(buffer_t* buffer, uint64_t start_position) {
  token_type_t token_type = TOKEN_TYPE_INTEGER_LITERAL;
  uint64_t pos = start_position;
  while (pos < buffer_length(buffer)) {
    utf8_decode_result_t decode_result = buffer_utf8_decode(buffer, pos);
    if (decode_result.error) {
      return (token_or_error_t){.error_code = TOKENIZER_ERROR_UTF_DECODE_ERROR,
                                .error_position = pos};
    }
    uint32_t code_point = decode_result.code_point;
    // FIXME: hexidecimal plus, "e", plus suffixes
    if (code_point == '.') {
      token_type = TOKEN_TYPE_FLOAT_LITERAL;
    }
    if (!(isdigit(code_point) || code_point == '.')) {
      break;
    }
    pos += decode_result.num_bytes;
  }

  return (token_or_error_t){.token = (oc_token_t){.buffer = buffer,
                                                  .type = token_type,
                                                  .start = start_position,
                                                  .end = pos}};
}

static char* c_punctuation[] = {
    "!=",
    "%=",
    "&&",
    "&=",
    "*=",
    "++",
    "+=",
    "--",
    "-=",
    "->",
    "...",
    "/=",
    "::",
    "<<=",
    "<=",
    "==",
    ">=",
    ">>=",
    "^=",
    "|=",
    "||",

    // Single character tokens should come after any two character
    // token that starts with the same character.

    "!",
    "%",
    "&",
    "(",
    ")",
    "*",
    "+",
    ",",
    "-",
    ".",
    "/",
    ":",
    ";",
    ";",
    "<",
    "=",
    ">",
    "?",
    "[",
    "]",
    "^",
    "{",
    "|",
    "}",
    "~",

    // Hopefully enough to parse C preprocessor stuff
    "#",
    "\\",
};

token_or_error_t tokenize_punctuation(buffer_t* buffer,
                                      uint64_t start_position) {
  int num_elements = sizeof(c_punctuation) / sizeof(c_punctuation[0]);
  for (int i = 0; i < num_elements; i++) {
    if (buffer_match_string_at(buffer, start_position, c_punctuation[i])) {
      return (token_or_error_t){
          .token
          = (oc_token_t){.buffer = buffer,
                         .type = TOKEN_TYPE_PUNCTUATION,
                         .start = start_position,
                         .end = start_position + strlen(c_punctuation[i])}};
    }
  }
  return (token_or_error_t){.error_code
                            = TOKENIZER_ERROR_UNRECOGNIZED_PUNCTUATION,
                            .error_position = start_position};
}

boolean_t is_comment_start(buffer_t* buffer, uint64_t position) {
  return buffer_match_string_at(buffer, position, "//")
         || buffer_match_string_at(buffer, position, "/*");
}

token_or_error_t tokenize_comment(buffer_t* buffer, uint64_t start_position) {
  if (buffer_match_string_at(buffer, start_position, "//")) {
    for (uint64_t position = start_position + 2; position < buffer->length;
         position++) {
      if (buffer_match_string_at(buffer, position, "\n")) {
        return (token_or_error_t){.token
                                  = (oc_token_t){.buffer = buffer,
                                                 .type = TOKEN_TYPE_COMMENT,
                                                 .start = start_position,
                                                 .end = position + 1}};
      }
    }
  } else {
    for (uint64_t position = start_position + 2; position < buffer->length;
         position++) {
      if (buffer_match_string_at(buffer, position, "*/")) {
        return (token_or_error_t){.token
                                  = (oc_token_t){.buffer = buffer,
                                                 .type = TOKEN_TYPE_COMMENT,
                                                 .start = start_position,
                                                 .end = position + 2}};
      }
    }
  }

  return (token_or_error_t){.error_code = TOKENIZER_ERROR_UNTERMINATED_COMMENT,
                            .error_position = start_position};
}

boolean_t is_string_literal_start(buffer_t* buffer, uint64_t position) {
  return buffer_match_string_at(buffer, position, "\"");
}

token_or_error_t tokenize_quoted_literal_common(
    buffer_t* buffer, uint64_t start_position, char* opening_sequence,
    char* quoted_closing_sequence, char* closing_sequence,
    token_type_t token_type, tokenizer_error_t unterminated_error_code) {
  // We don't try to fully understand or verify the string or
  // character literal (even to see if it is written in valid UTF-8),
  // only find the closing sequence. Of course we have to skip over
  // any instance of the closing sequence when it is escaped with
  // backslash.

  if (!buffer_match_string_at(buffer, start_position, opening_sequence)) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }

  for (uint64_t position = start_position + strlen(opening_sequence);
       position < buffer->length;) {
    if (buffer_match_string_at(buffer, position, quoted_closing_sequence)) {
      position += strlen(quoted_closing_sequence);
    } else if (buffer_match_string_at(buffer, position, closing_sequence)) {
      return (token_or_error_t){
          .token = (oc_token_t){.buffer = buffer,
                                .type = token_type,
                                .start = start_position,
                                .end = position + strlen(closing_sequence)}};
    } else {
      // TODO(jawilson): add an option to iterate "properly" over
      // utf-8 code-points one by one. This is not necessary since no
      // bytes of a single or multi-byte code-point will match either
      // double quote or backslash so matching in the middle of a
      // code-point like is then potentially
      position += 1;
    }
  }
  return (token_or_error_t){.error_code = unterminated_error_code,
                            .error_position = start_position};
}

token_or_error_t tokenize_string_literal(buffer_t* buffer,
                                         uint64_t start_position) {
  return tokenize_quoted_literal_common(
      buffer, start_position, "\"", "\\\"", "\"", TOKEN_TYPE_STRING_LITERAL,
      TOKENIZER_ERROR_UNTERMINATED_STRING_LITERAL);
}

boolean_t is_character_literal_start(buffer_t* buffer, uint64_t position) {
  return buffer_match_string_at(buffer, position, "'");
}

token_or_error_t tokenize_character_literal(buffer_t* buffer,
                                            uint64_t start_position) {
  // Treat character literals exactly like string literals. If the
  // character literal ends up containing more than one character
  // (which apparently is legal in some C compilers BTW), the backend
  // C or C++ compiler will determine if this is legal or not.
  return tokenize_quoted_literal_common(
      buffer, start_position, "'", "\\'", "'", TOKEN_TYPE_CHARACTER_LITERAL,
      TOKENIZER_ERROR_UNTERMINATED_CHARACTER_LITERL);
}


static inline oc_token_t* heap_allocate_token(oc_token_t token) {
  oc_token_t* result = malloc_struct(oc_token_t);
  *result = token;
  return result;
}

#define read_or_skip_token(token_reader_function_name, include)                \
  do {                                                                         \
    token_or_error_t token_or_error = token_reader_function_name(buffer, pos); \
    if (token_or_error.error_code) {                                           \
      return (oc_tokenizer_result_t){                                          \
          .tokenizer_error_code = token_or_error.error_code,                   \
          .tokenizer_error_position = token_or_error.error_position};          \
    }                                                                          \
    if (include) {                                                             \
      value_array_add(result_tokens, ptr_to_value(heap_allocate_token(         \
                                         token_or_error.token)));              \
    }                                                                          \
    pos = token_or_error.token.end;                                            \
  } while (0)

#define read_token(token_reader_function_name)                                 \
  read_or_skip_token(token_reader_function_name, true)

/**
 * @function tokenize
 *
 * Return a value array of the tokens OR return an error (such as when
 * a character or string literal is unterminated).
 */
oc_tokenizer_result_t tokenize(buffer_t* buffer, boolean_t include_whitespace,
                               boolean_t include_comments) {
  oc_tokenizer_result_t result = {0};

  // Sizing this array initially can avoid some copying but for now
  // let's just be dumb and optimize for smallish input sizes.
  value_array_t* result_tokens = make_value_array(1024);

  uint32_t pos = 0;
  while (pos < buffer_length(buffer)) {

    utf8_decode_result_t decode_result = buffer_utf8_decode(buffer, pos);
    if (decode_result.error) {
      return (oc_tokenizer_result_t){.tokenizer_error_code
                                     = TOKENIZER_ERROR_UTF_DECODE_ERROR};
    }

    uint32_t code_point = decode_result.code_point;

    if (isspace(code_point)) {
      read_or_skip_token(tokenize_whitespace, include_whitespace);
    } else if (is_identifier_start(code_point)) {
      read_token(tokenize_identifier);
    } else if (isdigit(code_point)) {
      read_token(tokenize_numeric);
    } else if (is_comment_start(buffer, pos)) {
      read_or_skip_token(tokenize_comment, include_comments);
    } else if (is_string_literal_start(buffer, pos)) {
      read_token(tokenize_string_literal);
    } else if (is_character_literal_start(buffer, pos)) {
      read_token(tokenize_character_literal);
    } else {
      read_token(tokenize_punctuation);
    }
  }

  result.tokens = result_tokens;
  return result;
}
