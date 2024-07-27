#line 2 "preprocessor.c"
#ifndef _PREPROCESSOR_H_
#define _PREPROCESSOR_H_

#include "preprocessor.h"
#include <c-armyknife-lib.h>

typedef struct {
  boolean_t keep_system_includes;
  boolean_t keep_user_includes;
} c_preprocess_options_t;

typedef struct {
  uint64_t start_token_position;
  uint64_t end_token_position;
  uint64_t start_buffer_position;
  uint64_t end_buffer_position;
} c_preprocessor_directive_range_t;

#endif /* _PREPROCESSOR_H_ */

/**
 * @file preprocessor.c
 *
 * This is meant to understand the C pre-processor enough to be able
 * to:
 *
 * 1. Identify which tokens are part of a C pre-processor directive so
 * that these tokens can be filtered out of the token array (along
 * with white-space tokens) when parsing the Omni C code.
 *
 * 2. Optionally "extract" "#define" and "#include" directives (and
 * possibly other kinds in the future) into the symbol table since we
 * will want to have these appear in the output.
 *
 * Future versions may want to do more C pre-processor work, for
 * example handling conditional compilation in some manner, doing the
 * actual macro expansion, handling line directives, preserving or
 * handling pragmas, etc.
 *
 * The obvious recommendation when using Omni C right now is that
 * conditional compilation be limited to files that are not actually
 * processed by Omni C and compiled with the normal C toolchain and
 * linked with the Omni C processed files. (The Omni C source itself
 * currently has no platform dependent code or other need of advanced
 * C preprocessor stuff so something simple is a good first step.)
 */

/**
 * @function mark_c_preprocessor_directive
 *
 * Determine the extent of a C pre-procesor directive (and mark each
 * token as is_cpp_token so it can be easily removed).
 */
c_preprocessor_directive_range_t 
mark_c_preprocessor_directive(c_preprocess_options_t options,
			      value_array_t* tokens,
			      uint64_t start_position) {

  c_preprocessor_directive_range_t result = {0};

  // Mark all token until the end of line as belonging to a directive
  // so that they can be removed. The lexer conveniently makes line
  // continuations their own tokens that aren't considered whitespace
  // (as well as making sure actual newlines only appear at the very
  // end of a white-space token).
  uint64_t position = start_position;
  for (; position < tokens->length; position++) {
    oc_token_t* token = token_at(tokens, position);
    token->is_cpp_token = true;
    if (position == start_position) {
      result.start_token_position = start_position;
      result.start_buffer_position = token->start;
    }
    result.end_token_position = position;
    result.end_buffer_position = token->end;
    if ((token->type == TOKEN_TYPE_WHITESPACE) && token_ends_with(token, "\n")) {
      break;
    }
  }
  return result;
}

uint64_t handle_c_preprocessor_directive(c_preprocess_options_t options,
					 symbol_table_t* symbol_table,
					 value_array_t* tokens,
					 uint64_t start_position) {
  uint64_t end_position = 
    mark_c_preprocessor_directive(options, tokens, start_position);
  oc_token_t* directive_name = token_at(tokens, start_position + 1);
  if (token_matches(directive_name, "include")) {



    // TODO(jawilson): figure out if this is a user or system include
    // and save a simplified "parse node" for include into the symbol
    // table depending on
  } else if (token_matches(directive_name, "define")) {
    // TODO(jawilson): extract raw text from the start of the first
    // token to the end of the last token (or the end of the buffer)
    // and add a define to the symbol table.
  }
  return end_position;
}

value_array_t* handle_c_preprocessor_directives(c_preprocess_options_t options,
						symbol_table_t* symbol_table,
						value_array_t* tokens) {
  for (uint64 position = 0; position < tokens->length; position++) {
    oc_token_t* token = token_at(tokens, position);
    if (token_matches(token, "#")) {
      position = handle_c_preprocessor_directive(options, symbol_table, tokens, position);
    }
  }
  // TODO(jawilson): filter out directives and whitespace tokens for
  // the result.
  return NULL;
}
