// SSCF generated file from: token-transformer.c

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

#include "token-transformer.c.generated.h"

#endif /* _TOKEN_TRANSFORMER_H_ */
