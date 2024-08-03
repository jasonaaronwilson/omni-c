#line 2 "pstate.c"
#ifndef _PSTATE_H_
#define _PSTATE_H_

/**
 * @struct pstate_t
 *
 * After writing some of the parser by hand, one observation is that
 * the "calling convention" itself is kind of clunky, especially the
 * error path which there are many. Another observation is that while
 * we like the freedom of a random access token array (especially
 * because this makes "roll-back" simpler versus an actual stream) we
 * mostly really do what something that operates more like a stream so
 * that we manage the stream position in a more predictable/natural
 * way.
 *
 * The new "calling convention" is inspired by the old one but:
 * 
 * 1. makes it about as easy to *return* a result but much easier to
 * "handle" the failure case. This is because we return a boolean
 * derived type which can be used directly in an if statement. The
 * actual error information is "passed back" by modifying the passed
 * in pstate_t structure. It also means we can easily add other error
 * fields.
 *
 * 2. allows for "returning" either a token or a parse node where as
 * previously we mostly focused on returning parse nodes.
 *
 * The consequence of #1 is that we can now use "||" (or "&&") which
 * makes certain things much denser (so it will feel easier).
 *
 * In term of efficieny, we will still be doing lots of string
 * comparisons to match terminal tokens. In the future we hope to use
 * "hashing" to provide token identifiers which will allow us to use
 * switch which should be much cheaper in certain circumstances
 * without radically changing the basic structure.
 */
typedef struct {
  value_array_t* tokens;
  uint64_t position;
  // error code
  uint64_t error_position;
  parse_node_t* result_node;
  token_t* result_token;
} pstate_t;

// #include "mode.c.generated.h"

typedef boolean_t pstatus_t;

#define PSTATUS_SUCCESS true
#define PSTATUS_FAILURE false

#endif /* _PSTATE_H_ */

pstatus_t pstate_set_result_token(pstate_t* pstate, token_t* token) {
  if (pstate->token || pstate->node || pstate->error_code) {
    fatal_error(ILLEGAL_STATE_EXCEPTION);
  }
  pstate->token = token;
  return true;
}

pstatus_t pstate_set_result_node(pstate_t* pstate, node_t* node) {
  if (pstate->token || pstate->node || pstate->error_code) {
    fatal_error(ILLEGAL_STATE_EXCEPTION);
  }
  pstate->node = node;
  return true;
}

// pstate_token()
// pstate_node()
// token_t* pstate_peek(pstate_t* pstate, int n);

/**
 * @parse pstate_match_token_string
 *
 * Simply advance the token position by one token (and return the
 * token). Note that this will cause a fatal error if we are already
 * in an error state.
 */
token_t* pstate_advance(pstate_t* pstate) {
  if (pstate->error_code) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  token_t* token = pstate_peek(pstate, 0);
  pstate->position++;
  return token;
}

/**
 * @parse pstate_match_token_string
 */
boolean_t pstate_match_token_string(pstate_t* pstate, char* token_string) {
  token_t* token = pstate_peek(pstate, 0);
  return token_matches(token, token_string);
}

boolean_t pstate_match_token_type(pstate_t* pstate, "");

pstatus_t pstate_expect_token_string(pstate_t* pstate, "");
pstatus_t pstate_expect_token_type(pstate_t* pstate, "");
