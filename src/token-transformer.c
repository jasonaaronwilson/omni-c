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
} token_transformer_options_t;

value_array_t* transform_tokens(value_array_t* tokens,
                                token_transformer_options_t xform_options);

#endif /* _TOKEN_TRANSFORMER_H_ */

/**
 * @function transform_tokens
 *
 * Given an array of tokens, for example from tokenize(), return an
 * easier sequence of tokens to parse by eliminating comments and
 * whitespace.
 *
 * Also put line and column numbers on these tokens.
 *
 * These tokens MUST share the same buffer (so the same input file or
 * generated code string) or else column and line number information
 * will be inaccurate.
 */
value_array_t* transform_tokens(value_array_t* tokens,
                                token_transformer_options_t xform_options) {
  // Depending on xform_options, we will usually end up with fewer,
  // tokens though in theory it is possible to increase the token
  // count (though value arrays grow in length when necessary).
  value_array_t* result = make_value_array(tokens->length + 1);

  // TODO(jawilson): whitespace is currently getting skipped by the
  // lexer itself according to FLAG_print_tokens_include_whitespace
  // and FLAG_print_tokens_include_comments so refactor that into
  // xform_options.

  // TODO(jawilson): add line and column to every token.

  for (int position = 0; position < tokens->length; position++) {
    boolean_t keep_token = false;
    oc_token_t* token
        = cast(oc_token_t*, value_array_get(tokens, position).ptr);

    switch (token->type) {
    case TOKEN_TYPE_WHITESPACE:
      keep_token = xform_options.keep_whitespace;
      break;
    case TOKEN_TYPE_COMMENT:
      if (token_starts_with(token, "/**")) {
        keep_token = xform_options.keep_javadoc_comments;
      } else {
        keep_token = xform_options.keep_comments;
      }
      break;
    default:
      keep_token = true;
      break;
    }

    // HERE: march through each code-point from the start of the token
    // to the end of the token.

    if (keep_token) {
      value_array_add(result, ptr_to_value(token));
    }
  }

  return result;
}
