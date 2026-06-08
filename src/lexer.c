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

typedef struct token_S {
  buffer_t* buffer;
  token_type_t type;
  int32_t start;
  int32_t end;
  int32_t line_number;
  int32_t column_number;
  boolean_t is_cpp_token;
} token_t;

typedef struct tokenizer_result_S {
  value_array_t* tokens;
  uint64_t tokenizer_error_position;
  tokenizer_error_t tokenizer_error_code;
} tokenizer_result_t;

static inline token_or_error_t make_token_result(buffer_t* buffer,
                                                 token_type_t token_type,
                                                 uint64_t start, uint64_t end) {
  return compound_literal(
      token_or_error_t, {.token = compound_literal(token_t, {.buffer = buffer,
                                                             .type = token_type,
                                                             .start = start,
                                                             .end = end})});
}

static inline token_or_error_t
    make_token_error_result(tokenizer_error_t tokenizer_error_code,
                            uint64_t position) {
  return compound_literal(
      token_or_error_t,
      {.error_position = position, .error_code = tokenizer_error_code});
}

static inline token_t* token_at(value_array_t* tokens, uint64_t position) {
  if (position >= tokens->length) {
    return nullptr;
  }
  return value_array_get_ptr(tokens, position, typeof(token_t*));
}

static inline boolean_t token_matches(token_t* token, char* str) {
  if (token == nullptr) {
    return false;
  }
  int str_len = strlen(str);
  return str_len == (token->end - token->start)
         && buffer_match_string_at(token->buffer, token->start, str);
}

static inline boolean_t token_starts_with(token_t* token, char* str) {
  return buffer_match_string_at(token->buffer, token->start, str);
}

static inline boolean_t token_contains(token_t* token, char* str) {
  return buffer_region_contains(token->buffer, token->start, token->end, str);
}

/**
 * @struct token_or_error_t
 *
 * Allow each "sub-tokenizer" to return either a token (or an error)
 * for each token.
 */
struct token_or_error_S {
  token_t token;
  tokenizer_error_t error_code;
  uint64_t error_position;
};

typedef struct token_or_error_S token_or_error_t;

/**
 * @enum numeric_literal_encoding_t
 *
 * Almost the states for a state machine definition for parsing
 * numeric literals. We can probably be more accurate by using
 * additional states for example for each part of a floating point
 * literal (exponent, mantissa, and exponent).
 */
typedef enum {
  NUMERIC_LITERAL_ENCODING_UNDECIDED,
  NUMERIC_LITERAL_ENCODING_FLOAT_OR_DECIMAL,
  NUMERIC_LITERAL_ENCODING_BINARY,
  NUMERIC_LITERAL_ENCODING_OCTAL,
  NUMERIC_LITERAL_ENCODING_HEX,
  NUMERIC_LITERAL_ENCODING_DECIMAL,
  NUMERIC_LITERAL_ENCODING_FLOAT,
} numeric_literal_encoding_t;

/**
 * @function token_to_string
 *
 * Return the token as a character string.
 */
char* token_to_string(token_t* token) {
  return buffer_c_substring(token->buffer, token->start, token->end);
}

/**
 * @function make_derived_token
 *
 * A dervied token is a type of synthetic token based on another
 * token.
 */
token_t* make_derived_token(token_t* source_token) {
  token_t* result = cast(
      token_t*, malloc_copy_of(cast(uint8_t*, source_token), sizeof(token_t)));
  buffer_t* buffer = make_buffer(source_token->end - source_token->start);
  buffer = buffer_append_sub_buffer(buffer, source_token->start,
                                    source_token->end, result->buffer);
  result->buffer = buffer;
  result->start = 0;
  result->end = buffer->length;
  return result;
}

/**
 * @function append_token_debug_string
 *
 * Append a debug representation of a token to the buffer that looks
 * roughly like this:
 *
 * type: TOKEN_TYPE_IDENTIFIER start: 10 end: 15 str: asdf
 */
__attribute__((warn_unused_result)) buffer_t*
    append_token_debug_string(buffer_t* buffer, token_t token) {
  char* str = token_to_string(&token);
  buffer = buffer_printf(
      buffer, "type: %s start: %d end: %d line=%d column=%d str: %s",
      token_type_to_string(token.type), token.start, token.end,
      token.line_number, token.column_number, str);
  free_bytes(str);
  return buffer;
}

/**
 * @function buffer_append_token_string
 *
 * Append the UTF-8 for a token to the buffer. This is generally only
 * used for tokens that don't contain a newline or else we will need
 * to start quoting tokens.
 */
buffer_t* buffer_append_token_string(buffer_t* buffer, token_t* token) {
  // TODO(jawilson): don't rely on token_to_string so we don't have to
  // allocate and deallocate something.
  char* str = token_to_string(token);
  buffer = buffer_printf(buffer, "%s", str);
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

/* ========================================================================= */
// Tokenize whitespace
/* ========================================================================= */

/**
 * @function tokenize_whitespace
 *
 * Read white-space until the buffer is exhausted or a non-whitespace
 * UTF-8 code point is read. Additionally, break after seeing a
 * newline so that a white-space token will never include more than
 * one newline and also if a newline is present in a white-space
 * token, it will always appear as the last code-point of a
 * white-space token.
 */
token_or_error_t tokenize_whitespace(buffer_t* buffer,
                                     uint64_t start_position) {
  uint64_t pos = start_position;
  while (pos < buffer_length(buffer)) {
    utf8_decode_result_t decode_result = buffer_utf8_decode(buffer, pos);
    if (decode_result.error) {
      return compound_literal(token_or_error_t,
                              {.error_code = TOKENIZER_ERROR_UTF_DECODE_ERROR,
                               .error_position = pos});
    }
    uint32_t code_point = decode_result.code_point;
    if (!isspace(code_point)) {
      break;
    }
    pos += decode_result.num_bytes;
    // Always terminate a white-space token after seeing a
    // newline. This helps us a tiny bit with dealing with C
    // preprocessor "lines" (as they are sometimes called - C is
    // otherwise not really line oriented which makes the C
    // pre-processor a bit weird...)
    if (code_point == '\n') {
      break;
    }
  }

  return compound_literal(
      token_or_error_t,
      {.token = compound_literal(token_t, {.buffer = buffer,
                                           .type = TOKEN_TYPE_WHITESPACE,
                                           .start = start_position,
                                           .end = pos})});
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
      return compound_literal(token_or_error_t,
                              {.error_code = TOKENIZER_ERROR_UTF_DECODE_ERROR,
                               .error_position = pos});
    }
    uint32_t code_point = decode_result.code_point;
    if (!(is_identifier_start(code_point) || isdigit(code_point))) {
      break;
    }
    pos += decode_result.num_bytes;
  }

  return compound_literal(
      token_or_error_t,
      {.token = compound_literal(token_t, {.buffer = buffer,
                                           .type = TOKEN_TYPE_IDENTIFIER,
                                           .start = start_position,
                                           .end = pos})});
}

/**
 * @function tokenize_numeric
 *
 * Read a numeric literal.
 *
 * We want to support all the formats that C supports:
 * * 1
 * * 3.14
 * * 0x1
 * * 0o1
 * * etc.
 *
 * We also want to support using 0b* for binary constants and allow
 * using either "_" or "'" as a grouping seperator. Extensions should
 * probably all be transformed on output to be a simple decimal
 * number.
 */
token_or_error_t tokenize_numeric(buffer_t* buffer, uint64_t start_position) {
  numeric_literal_encoding_t encoding = NUMERIC_LITERAL_ENCODING_UNDECIDED;
  uint32_t previous_code_point = 0;

  token_type_t token_type = TOKEN_TYPE_INTEGER_LITERAL;

  uint64_t offset = 0;
  uint64_t pos = start_position;
  while (pos < buffer_length(buffer)) {
    utf8_decode_result_t decode_result = buffer_utf8_decode(buffer, pos);
    if (decode_result.error) {
      return compound_literal(token_or_error_t,
                              {.error_code = TOKENIZER_ERROR_UTF_DECODE_ERROR,
                               .error_position = pos});
    }
    uint32_t code_point = decode_result.code_point;

    if (offset == 0 && code_point != '0') {
      encoding = NUMERIC_LITERAL_ENCODING_FLOAT_OR_DECIMAL;
    }

    if (offset == 1 && encoding == NUMERIC_LITERAL_ENCODING_UNDECIDED) {
      boolean_t changed = false;
      if (code_point == 'x') {
        encoding = NUMERIC_LITERAL_ENCODING_HEX;
        changed = true;
      } else if (code_point == 'o') {
        encoding = NUMERIC_LITERAL_ENCODING_OCTAL;
        changed = true;
      } else if (code_point == 'b') {
        encoding = NUMERIC_LITERAL_ENCODING_HEX;
        changed = true;
      }
      if (changed) {
        pos += decode_result.num_bytes;
        previous_code_point = code_point;
        offset += 1;
        continue;
      }
    }

    if (code_point == '.'
        && (encoding == NUMERIC_LITERAL_ENCODING_UNDECIDED
            || encoding == NUMERIC_LITERAL_ENCODING_FLOAT_OR_DECIMAL)) {
      token_type = TOKEN_TYPE_FLOAT_LITERAL;
      encoding = NUMERIC_LITERAL_ENCODING_FLOAT;
      pos += decode_result.num_bytes;
      previous_code_point = code_point;
      offset += 1;
      continue;
    }

    if (!can_extend_number(encoding, code_point, previous_code_point)) {
      break;
    }

    pos += decode_result.num_bytes;
    previous_code_point = code_point;
    offset += 1;
  }

  return compound_literal(
      token_or_error_t,
      {.token = compound_literal(token_t, {.buffer = buffer,
                                           .type = token_type,
                                           .start = start_position,
                                           .end = pos})});
}

boolean_t can_extend_number(numeric_literal_encoding_t encoding,
                            uint32_t code_point, uint32_t previous_code_point) {
  switch (encoding) {
  case NUMERIC_LITERAL_ENCODING_UNDECIDED:
    return string_contains_char("0123456789.eEobxLlUuFf", code_point);

  case NUMERIC_LITERAL_ENCODING_FLOAT_OR_DECIMAL:
    return string_contains_char("0123456789.eELlUuFf", code_point);

  case NUMERIC_LITERAL_ENCODING_BINARY:
    return string_contains_char("01LlUu", code_point);

  case NUMERIC_LITERAL_ENCODING_OCTAL:
    return string_contains_char("01234567LlUu", code_point);

  case NUMERIC_LITERAL_ENCODING_HEX:
    return string_contains_char("0123456789abcdefABCDEFLlUu", code_point);

  case NUMERIC_LITERAL_ENCODING_DECIMAL:
    return string_contains_char("0123456789LlUu", code_point);

  case NUMERIC_LITERAL_ENCODING_FLOAT:
    return string_contains_char("0123456789FfeE", code_point)
           || ((previous_code_point == 'e' || previous_code_point == 'E')
               && string_contains_char("+-", code_point));

  default:
    fatal_error(ERROR_ILLEGAL_STATE);
    return false;
  }

  return false;
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
    "[[",
    "]]",
    "^=",
    "|=",
    "||",
    "<<",
    ">>",

    // Since we match from top-to-bottom, we generally want the
    // longest tokens first.

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

    // *****************************************************************
    // Hopefully enough to parse C preprocessor stuff (after going
    // through the token-transformer...)
    // *****************************************************************
    "#",
    // This won't work forever... We special case this in
    // transform_tokens()...
    "\\\n",
};

token_or_error_t tokenize_punctuation(buffer_t* buffer,
                                      uint64_t start_position) {
  int num_elements = sizeof(c_punctuation) / sizeof(c_punctuation[0]);
  for (int i = 0; i < num_elements; i++) {
    if (buffer_match_string_at(buffer, start_position, c_punctuation[i])) {
      return compound_literal(
          token_or_error_t,
          {.token = compound_literal(
               token_t, {.buffer = buffer,
                         .type = TOKEN_TYPE_PUNCTUATION,
                         .start = start_position,
                         .end = start_position + strlen(c_punctuation[i])})});
    }
  }
  return compound_literal(
      token_or_error_t, {.error_code = TOKENIZER_ERROR_UNRECOGNIZED_PUNCTUATION,
                         .error_position = start_position});
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
        return compound_literal(
            token_or_error_t,
            {.token = compound_literal(token_t, {.buffer = buffer,
                                                 .type = TOKEN_TYPE_COMMENT,
                                                 .start = start_position,
                                                 .end = position + 1})});
      }
    }
  } else {
    for (uint64_t position = start_position + 2; position < buffer->length;
         position++) {
      if (buffer_match_string_at(buffer, position, "*/")) {
        return compound_literal(
            token_or_error_t,
            {.token = compound_literal(token_t, {.buffer = buffer,
                                                 .type = TOKEN_TYPE_COMMENT,
                                                 .start = start_position,
                                                 .end = position + 2})});
      }
    }
  }

  return compound_literal(token_or_error_t,
                          {.error_code = TOKENIZER_ERROR_UNTERMINATED_COMMENT,
                           .error_position = start_position});
}

boolean_t is_string_literal_start(buffer_t* buffer, uint64_t position) {
  return buffer_match_string_at(buffer, position, "\"");
}

boolean_t is_character_literal_start(buffer_t* buffer, uint64_t position) {
  return buffer_match_string_at(buffer, position, "'");
}

/**
 * @function tokenize_string_literal
 *
 * Parse a C style string literal.
 */
token_or_error_t tokenize_string_literal(buffer_t* buffer,
                                         uint64_t start_position) {
  uint64_t position = start_position;
  if (is_string_literal_start(buffer, position)) {
    position++;
    while (true) {
      if (position > buffer_length(buffer)) {
        make_token_error_result(position,
                                TOKENIZER_ERROR_UNTERMINATED_STRING_LITERAL);
      }
      if (is_string_literal_start(buffer, position)) {
        return make_token_result(buffer, TOKEN_TYPE_STRING_LITERAL,
                                 start_position, position + 1);
      }
      position = advance_char_literal_position(buffer, position);
    }
  }
  return make_token_error_result(position,
                                 TOKENIZER_ERROR_UNTERMINATED_STRING_LITERAL);
}

/**
 * @function tokenize_character_literal
 *
 * Parse a C style string character literal.
 */
token_or_error_t tokenize_character_literal(buffer_t* buffer,
                                            uint64_t start_position) {
  uint64_t position = start_position;
  if (is_character_literal_start(buffer, position)) {
    position++;
    position = advance_char_literal_position(buffer, position);
    if (is_character_literal_start(buffer, position)) {
      return make_token_result(buffer, TOKEN_TYPE_CHARACTER_LITERAL,
                               start_position, position + 1);
    }
  }
  return make_token_error_result(
      position, TOKENIZER_ERROR_UNTERMINATED_CHARACTER_LITERAL);
}

#define UNICODE_BACKSLASH_CODE_POINT 92
#define UNICODE_QUOTE_CODE_POINT 39
#define UNICODE_DOUBLE_QUOTE_CODE_POINT 34
#define UNICODE_LOWER_N_CODE_POINT 110

uint64_t advance_char_literal_position(buffer_t* buffer, uint64_t position) {
  // log_fatal("advance_char_literal_position = %d", position & 0xffffffff);
  utf8_decode_result_t result = buffer_utf8_decode(buffer, position);
  if (result.num_bytes == 0) {
    log_fatal("We seem to have non-utf-8 input...");
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  if (result.code_point == UNICODE_BACKSLASH_CODE_POINT) {
    position += result.num_bytes;
    result = buffer_utf8_decode(buffer, position);
    // log_fatal("cp = %d %d", result.code_point, result.num_bytes);
    if (result.num_bytes == 0) {
      log_fatal("We seem to have non-utf-8 input issue #2...");
      fatal_error(ERROR_ILLEGAL_STATE);
    }
    // case '?'
    switch (result.code_point) {
    case UNICODE_LOWER_N_CODE_POINT:
    case 't':
    case 'r':
    case UNICODE_BACKSLASH_CODE_POINT:
    case UNICODE_QUOTE_CODE_POINT:
    case UNICODE_DOUBLE_QUOTE_CODE_POINT:
      position += result.num_bytes;
      return position;
    case 'x':
      position += result.num_bytes + 2;
      return position;
    case 'u':
      position += result.num_bytes + 4;
      return position;

    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7': {
      // Advance past the first octal digit we already decoded
      position += result.num_bytes;

      // Check for a second optional octal digit
      utf8_decode_result_t next = buffer_utf8_decode(buffer, position);
      if (next.code_point >= '0' && next.code_point <= '7') {
        position += next.num_bytes;

        // Check for a third optional octal digit
        next = buffer_utf8_decode(buffer, position);
        if (next.code_point >= '0' && next.code_point <= '7') {
          position += next.num_bytes;
        }
      }
      return position;
    }

    default:
      // log_fatal("%s", buffer_to_c_string(buffer));
      fatal_error(ERROR_ILLEGAL_INPUT);
      break;
    }
  }
  return position + result.num_bytes;
}

static inline token_t* heap_allocate_token(token_t token) {
  token_t* result = malloc_struct(token_t);
  *result = token;
  return result;
}

#define read_token(token_reader_function_name)                                 \
  do {                                                                         \
    token_or_error_t token_or_error = token_reader_function_name(buffer, pos); \
    if (token_or_error.error_code) {                                           \
      return compound_literal(                                                 \
          tokenizer_result_t,                                                  \
          {.tokenizer_error_code = token_or_error.error_code,                  \
           .tokenizer_error_position = token_or_error.error_position});        \
    }                                                                          \
    token = heap_allocate_token(token_or_error.token);                         \
    value_array_add(result_tokens, ptr_to_value(token));                       \
    pos = token_or_error.token.end;                                            \
  } while (0)

/**
 * @function tokenize
 *
 * Return a value array of the tokens OR return an error (such as when
 * a character or string literal is unterminated).
 */
tokenizer_result_t tokenize(buffer_t* buffer) {
  tokenizer_result_t result = {0};

  // Sizing this array initially can avoid some copying but for now
  // let's just be dumb and optimize for smallish input sizes.
  value_array_t* result_tokens = make_value_array(1024);

  uint64_t line_number = 1;
  uint64_t column_number = 1;

  uint32_t pos = 0;
  while (pos < buffer_length(buffer)) {
    utf8_decode_result_t decode_result = buffer_utf8_decode(buffer, pos);
    if (decode_result.error) {
      return compound_literal(
          tokenizer_result_t,
          {.tokenizer_error_code = TOKENIZER_ERROR_UTF_DECODE_ERROR});
    }

    uint32_t code_point = decode_result.code_point;
    token_t* token = nullptr;

    if (isspace(code_point)) {
      read_token(tokenize_whitespace);
    } else if (is_identifier_start(code_point)) {
      read_token(tokenize_identifier);
    } else if (isdigit(code_point)) {
      read_token(tokenize_numeric);
    } else if (is_comment_start(buffer, pos)) {
      read_token(tokenize_comment);
    } else if (is_string_literal_start(buffer, pos)) {
      read_token(tokenize_string_literal);
    } else if (is_character_literal_start(buffer, pos)) {
      read_token(tokenize_character_literal);
    } else {
      read_token(tokenize_punctuation);
    }

    // If token is null we have a problem...

    if (token != nullptr) {
      token->line_number = line_number;
      token->column_number = column_number;
      for (int i = token->start; i < token->end; i++) {
        uint8_t ch = buffer_get(token->buffer, i);
        if (ch == '\n') {
          line_number++;
          column_number = 1;
        } else {
          column_number++;
        }
      }
    }
  }

  result.tokens = result_tokens;
  return result;
}
