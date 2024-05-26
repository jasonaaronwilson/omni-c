#line 2 "token-transformer.c"
#ifndef _TOKEN_TRANSFORMER_H_
#define _TOKEN_TRANSFORMER_H_

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
 * recover this from the buffer and the start of a token - I strongly
 * suspect this will keep us a tiny bit more sane during
 * debugging... Maybe We'll eventually "#ifdef" this out for
 * "production"... Hmmm...).
 *
 * C mostly doesn't care about whitespace/comments but the C
 * pre-processor cares about newlines (and perhaps others whitespace
 * because of token pasting?) because of "#define" (which uses the
 * continuation character "\" instead of say something consistent with
 * the rest of C though making simple definitions like #define
 * C_IS_WRONG 1 would look much uglier because I guess
 * #macro/#endmacro were too wordy to fit on a punch card...), one of
 * many reasons that C has a more complex grammar than Go (though way
 * smaller than C++). Since I don't know how to actually measure the
 * complexity of a grammer, I'm using the file size the the
 * tree-sitter parser system (which happens to produce a huge C file)
 * as a proxy for our intuitive sense of the complexity of the
 * grammar. Rust and other "C-like" languages use keywords like "fn"
 * (Rust) or "func" (Go) or "function" (Javascript) to make them
 * easier (and probably faster!)  to parse but our goal remains to
 * look as much like C as possible and it least as this level in our
 * translator, kind of agnostic of the input language as long as it's
 * sufficiently like C.
 *
 * Since I'm not sure we can just completely ignore the C
 * pre-processor at this point or even ever (FFI for a "fringe"
 * language written by one guy by necessity needs to be seamless), we
 * introduce a synthetic token, "#$$_end_define_$$#" to represent the
 * end of a C pre-processor "#define" construct (allowing us to also
 * eliminate the special "\\" macro line continuation token though we
 * can't really take back the backslash...).
 *
 * The C pre-processor serves multiple functions, such as "conditional
 * compilation" in addition to serving as "macros". For macros, we
 * will eventually provide something interesting that will be easier
 * to read and "get right" (possibly even sometimes supporting
 * overloading) but we have to at least be able to parse C header
 * files to not re-invent the entire world all at once.
 *
 * Closeing thoughts:
 *
 * Is this legal as the last line of a file "#define foo() bar \"
 * (with no trailing newline, i.e., it ends without a newline))?
 * Frankly, I don't care that much since it appears that only clang
 * with -pedantic could even detect this and therefore we wouldn't be
 * radically out of line to barf on it
 */

typedef struct token_transformer_options_S {
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
  // TODO(jawilson): whitespace is currently getting skipped by the
  // lexer itself according to FLAG_print_tokens_include_whitespace
  // and FLAG_print_tokens_include_comments so refactor that into
  // xform_options.

  // TODO(jawilson): add line and column to every token. I can hear
  // one compiler writer from the 1990s melting down over a proposed
  // change that may even have been much less offsensive as this!

  return tokens;
}
