#line 2 "pstate.c"
#ifndef _PSTATE_H_
#define _PSTATE_H_


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
