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

buffer_t* buffer_append_human_readable_error(buffer_t* buffer,
                                             compiler_error_t* error);

// Reinclude generated prototypes...
// #include "compiler-errors.c.generated.h"

#endif /* _COMPILER_ERRORS_H_ */

#include "lexer.h"

buffer_t* buffer_append_human_readable_tokenizer_error(buffer_t* buffer,
                                                       compiler_error_t* error);
buffer_t* buffer_append_human_readable_parser_error(buffer_t* buffer,
                                                    compiler_error_t* error);

/**
 * @function buffer_append_human_readable_error
 *
 * Append a human readable version of the error to the buffer.
 */
buffer_t* buffer_append_human_readable_error(buffer_t* buffer,
                                             compiler_error_t* error) {
  if (error->tokenizer_error_code != TOKENIZER_ERROR_UNKNOWN) {
    buffer = buffer_append_human_readable_tokenizer_error(buffer, error);
  }
  if (error->parser_error_code != PARSE_ERROR_UNKNOWN) {
    buffer = buffer_append_human_readable_parser_error(buffer, error);
  }
  return buffer;
}

typedef struct {
  char* previous_lines;
  char* current_line;
  char* next_lines;
} src_code_snippets_t;

/**
 * @function get_source_code_snippet
 *
 * While there are many different kinds of compiler error messages,
 * the ones that get the most praise actually include surrounding
 * context. "value_array_t" of previous_lines and next_lines strings
 * would probably more convenient to allow indentation, etc., it's not
 * clear that's necessary yet and the caller could call a split
 * routine I guess...
 */
src_code_snippets_t get_source_code_snippet(buffer_t* buffer, uint64_t location,
                                            int before_lines, int after_lines) {
  src_code_snippets_t result = {0};

  uint64_t current_begin = buffer_beginning_of_line(buffer, location);
  uint64_t current_end = buffer_end_of_line(buffer, location);

  result.current_line = buffer_c_substring(buffer, current_begin, current_end);

  uint64_t prefix_begin = current_begin;
  for (int i = 0; i < before_lines && prefix_begin > 0; i++) {
    prefix_begin = buffer_beginning_of_line(buffer, prefix_begin - 1);
  }
  result.previous_lines
      = buffer_c_substring(buffer, prefix_begin, current_begin);

  uint64_t suffix_end = current_end;
  for (int i = 0; i < after_lines && suffix_end < buffer->length; i++) {
    suffix_end = buffer_end_of_line(buffer, suffix_end + 1);
  }
  result.next_lines = buffer_c_substring(buffer, current_end, suffix_end);

  return result;
}

char* formatted_snippet
    = "======================================================================\n"
      "{error-prefix-lines}"
      "{error-highlight-on}"
      "{error-current-line}"
      "{error-highlight-off}"
      "{error-suffix-lines}"
      "\n======================================================================"
      "\n";

char* do_common_replacements(char* template, compiler_error_t* error) {
  buffer_t* buffer = make_buffer(256);

  char* file_name = error->file_name;
  src_code_snippets_t snippet = {0};

  if (error->error_token) {
    line_and_column_t position_info = buffer_position_to_line_and_column(
        error->error_token->buffer, error->error_token->start);

    // This is the gnu standard for error names, lines, and columns
    buffer = buffer_printf(buffer, "%s:%d.%d: ", file_name,
                           position_info.line & 0xffffffff,
                           position_info.column & 0xffffffff);

    snippet = get_source_code_snippet(error->error_token->buffer,
                                      error->error_token->start, 5, 3);
  } else {
    buffer = buffer_printf(buffer, "%s:%d.%d: ", file_name, 0, 0);
  }

  buffer = buffer_append_string(buffer, template);

  // This comes first itself contains replacable bits!
  buffer = buffer_replace_all(buffer, "{formatted_snippet}", formatted_snippet);

  // This might create whacky effects when looking at out own source
  // code or anything containing our magic strings.

  //// For now, just turn on bold...
  buffer = buffer_replace_all(buffer, "{error-highlight-on}", "\033[1m");
  /// And then reset the terminal attributes...
  buffer = buffer_replace_all(buffer, "{error-highlight-off}", "\033[0m");

  buffer = buffer_replace_all(buffer, "{error-prefix-lines}",
                              snippet.previous_lines);
  buffer = buffer_replace_all(buffer, "{error-current-line}",
                              snippet.current_line);
  buffer
      = buffer_replace_all(buffer, "{error-suffix-lines}", snippet.next_lines);

  return buffer_to_c_string(buffer);
}

char* parse_error_unknown
    = "A parse error has occurred but the error message is unavailable.\n"
      "{formatted_snippet}";

char* error_field_width_or_semicolon
    = "A parse error has occurred while trying to read a structure or union "
      "field.\n"
      "{formatted_snippet}"
      "Expected a field width or a semi-colon.";

char* error_open_brace_expected
    = "A parse error has occurred while trying to parse after an opening '[' "
      "character. Expected a closing ']'.\n"
      "{formatted_snippet}";

char* error_open_semicolon_expected
    = "A parse error has occurred since a semicolon was expected.\n"
      "{formatted_snippet}";

char* error_unrecognized_top_level_declaration
    = "Unable to parse a top-level declaration.\n"
      "{formatted_snippet}"
      "\nVariables, functions, "
      "structures, unions, and typedefs are currently supported.\n\n"
      "(Additionally, C pre-processor directives are allowed but "
      "are currently skipped before parsing.)";

char* error_conflicting_storage_class_specifier
    = "Conflicting storage class specifiers.\n"
      "{formatted_snippet}"
      "\nA storage class specifier is either static, extern, auto, or "
      "register\n"
      "For each 'declaration', only one of them is allowed.";

char* error_conflicting_function_specifier
    = "Conflicting function specifier.\n"
      "{formatted_snippet}"
      "\nThe only known function specifier is inline so you likely repeated "
      "it.";

char* error_expected_open_paren_for_underscore_attribute
    = "This doesn't look like a valid attribute specification.\n"
      "{formatted_snippet}"
      "\nAttributes roughly look like __attribute__((...))\n"
      "\nYou might have unbalanced parens...";

char* error_expected_matching_close_paren_after_underscore_attribute
    = "This doesn't look like a valid attribute specification.\n"
      "{formatted_snippet}"
      "\nAttributes roughly look like __attribute__((...))\n"
      "\nYou might have unbalanced parens...";

// ======================================================================

buffer_t*
    buffer_append_human_readable_tokenizer_error(buffer_t* buffer,
                                                 compiler_error_t* error) {
  // TODO(jawilson):
  //
  // These don't seem to happen that often with real world code.

  buffer = buffer_printf(buffer, "\nlexer error code = %d\n",
                         error->tokenizer_error_code);
  return buffer;
}

buffer_t* buffer_append_human_readable_parser_error(buffer_t* buffer,
                                                    compiler_error_t* error) {
  buffer = buffer_printf(buffer, "\nparser error code = %d\n",
                         error->parser_error_code);
  char* template = NULL;
  switch (error->parser_error_code) {
  case PARSE_ERROR_EXPECTED_FIELD_WIDTH_OR_SEMICOLON:
    template = error_field_width_or_semicolon;
    break;

  case PARSE_ERROR_CLOSE_BRACKET_EXPECTED:
    template = error_open_brace_expected;
    break;

  case PARSE_ERROR_UNRECOGNIZED_TOP_LEVEL_DECLARATION:
    template = error_unrecognized_top_level_declaration;
    break;

  case PARSE_ERROR_SEMICOLON_EXPECTED:
    template = error_open_semicolon_expected;
    break;

  case PARSE_ERROR_CONFLICTING_STORAGE_CLASS_SPECIFIER:
    template = error_conflicting_storage_class_specifier;
    break;

  case PARSE_ERROR_CONFLICTING_FUNCTION_SPECIFIER:
    template = error_conflicting_function_specifier;
    break;

  case PARSE_ERROR_EXPECTED_OPEN_PAREN_AFTER_UNDERSCORE_ATTRIBUTE:
    template = error_expected_open_paren_for_underscore_attribute;
    break;

  case PARSE_ERROR_EXPECTED_MATCHING_CLOSE_PAREN_AFTER_UNDERSCORE_ATTRIBUTE:
    template = error_expected_matching_close_paren_after_underscore_attribute;
    break;

  default:
    template = parse_error_unknown;
    break;
  }

  char* template_string = do_common_replacements(template, error);
  return buffer_append_string(buffer, template_string);
}
