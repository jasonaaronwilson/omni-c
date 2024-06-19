#line 2 "token-transformer.c"
#ifndef _TOKEN_TRANSFORMER_H_
#define _TOKEN_TRANSFORMER_H_

#include "lexer.h"
#include <c-armyknife-lib.h>

/**
 * @file token-transformer
 *
 * Processs the result of tokenize() to make the token stream more
 * amenable to parsing and analysis without actually running the C
 * pre-processor.
 *
 * (As a convenience, we also augment tokens with their starting line
 * and column numbers (technically not necessary because we can
 * recover this from the buffer and the start of a token but I
 * strongly suspect this will keep us a tiny bit more sane during
 * debugging.
 *
 * C mostly doesn't care about whitespace/comments but the C
 * pre-processor cares about newlines (and perhaps others whitespace
 * because of token pasting?) because of "#define" (which uses the
 * continuation character "\" instead of say something consistent with
 * the rest of C.
 *
 * Since I'm not sure we can just completely ignore the C
 * pre-processor at this point or even ever (FFI for a "fringe"
 * language is necessary to be seamless), we introduce a synthetic
 * token, "#$$_end_define_$$#" to represent the end of a C
 * pre-processor "#define" construct (allowing us to also eliminate
 * the special "\\" macro line continuation tokens).
 */

/**
 * @structure token_transformer_options_t
 *
 * Controls how the token transformer operates. Normally we want to
 * filter out all comments but another tool than a compiler (like say
 * a formatter) might want to keep all the tokens.
 */
typedef struct token_transformer_options_S {
  boolean_t keep_whitespace;
  boolean_t keep_javadoc_comments;
  boolean_t keep_comments;
  boolean_t keep_c_preprocessor_lines;
} token_transformer_options_t;

#ifdef HAND_WRITTEN_PROTOTYPES
value_array_t* transform_tokens(value_array_t* tokens,
                                token_transformer_options_t xform_options);
#endif

#include "token-transformer.c.generated.h"

#endif /* _TOKEN_TRANSFORMER_H_ */

/**
 * @function transform_tokens
 *
 * Given an array of tokens, for example from tokenize(), return an
 * easier sequence of tokens to parse by eliminating comments and
 * whitespace.
 */
value_array_t* transform_tokens(value_array_t* tokens,
                                token_transformer_options_t xform_options) {
  value_array_t* result = make_value_array(tokens->length);
  boolean_t is_c_preprocessor_line = false;
  for (int position = 0; position < tokens->length; position++) {
    oc_token_t* token = token_at(tokens, position);

    boolean_t keep_token = false;
    switch (token->type) {
    case TOKEN_TYPE_WHITESPACE:
      keep_token = xform_options.keep_whitespace;
      if (token_contains(token, "\n")) {
        is_c_preprocessor_line = false;
      }
      break;
    case TOKEN_TYPE_COMMENT:
      if (token_starts_with(token, "/**")) {
        keep_token = xform_options.keep_javadoc_comments;
      } else {
        keep_token = xform_options.keep_comments;
      }
      // This is super confusing and is why you should only use non
      // line comments in C macros...
      // if (is_c_preprocessor_line && token_starts_with(token, "//")) {
      // is_c_preprocessor_line = false;
      // }
      break;
    default:
      keep_token = true;
      break;
    }

    if (token->type == TOKEN_TYPE_PUNCTUATION) {
      if (!is_c_preprocessor_line && token_matches(token, "#")) {
        is_c_preprocessor_line = true;
      }
    }

    if (!xform_options.keep_c_preprocessor_lines) {
      keep_token = keep_token && !is_c_preprocessor_line;
    }

    if (keep_token) {
      value_array_add(result, ptr_to_value(token));
    }
  }

  return result;
}
