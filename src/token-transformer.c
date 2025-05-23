/**
 * @file token-transformer
 *
 * Processs the result of tokenize() to make the token stream more
 * amenable to parsing and analysis without actually running the C
 * pre-processor.
 *
 * C mostly doesn't care about whitespace/comments but the C
 * pre-processor cares about newlines (and perhaps others whitespace
 * because of token pasting?) because of "#define" (which uses the
 * continuation character "\" instead of say something consistent with
 * the rest of C.
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
  for (int position = 0; position < tokens->length; position++) {
    token_t* token = token_at(tokens, position);
    if (token->is_cpp_token && !xform_options.keep_c_preprocessor_lines) {
      continue;
    }
    if (token->type == TOKEN_TYPE_WHITESPACE
        && !xform_options.keep_whitespace) {
      continue;
    }
    if (token->type == TOKEN_TYPE_COMMENT) {
      if (token_starts_with(token, "/**")) {
        if (!xform_options.keep_javadoc_comments) {
          continue;
        }
      } else if (!xform_options.keep_comments) {
        continue;
      }
    }

    value_array_add(result, ptr_to_value(token));
  }

  return result;
}
