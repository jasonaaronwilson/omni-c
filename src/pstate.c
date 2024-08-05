#line 2 "pstate.c"
#ifndef _PSTATE_H_
#define _PSTATE_H_

#include "parser.h"

// Must come last

#include "pstate.c.generated.h"

#endif /* _PSTATE_H_ */

/**
 * @function ensure_empty_result_state
 *
 * This enforces the condition that we don't currently have a token,
 * node, or error code as the "result" in pstate. This prevents us
 * from silently ignoring previous results instead of being explicit
 * about when we don't care about them.
 */
void ensure_empty_result_state(pstate_t* pstate) {
  if (pstate->result_token != NULL || pstate->result_node != NULL
      || pstate->error.parse_error_code) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
}

/**
 * @function pstate_error
 *
 * Sets the error in state (possibly overwriting an existing error
 * contained in pstate).
 */
pstatus_t pstate_error(pstate_t* pstate, uint64_t saved_position,
                       parse_error_code_t parse_error_code) {
  // ensure mostly empty.
  // TODO(jawilson): fill in more of compiler error.
  pstate->error.parse_error_code = parse_error_code;
  return false;
}

/**
 * @function pstate_ignore_error
 *
 * If we are parsing something that is optional or for which there are
 * multiple possible things to parse, typically we want to ignore the
 * error and continue. The contract is that pstate position may only
 * be modified by the routines we call when they return are successful
 * pstatus_t so we don't need to manipulate the position to continue
 * parsing at the same exact spot.
 *
 * We return the passed in pstate_t* because this allows us to code up
 * long blocks
 */
pstate_t* pstate_ignore_error(pstate_t* pstate) {
  /*
  if (!pstate->error.parse_error_code) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  */
  pstate->error = (compiler_error_t){0};
  ensure_empty_result_state(pstate);
  return pstate;
}

/**
 * @function pstate_ignore_error
 *
 * This just makes sure we really do currently have an error in pstate
 * and then restores the saved position to meet our contract of not
 * changing position unless we were successful.
 */
pstatus_t pstate_propagate_error(pstate_t* pstate, uint64_t saved_position) {
  if (!pstate->error.parse_error_code) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return false;
}

/**
 * @function pstate_set_result_token
 *
 * This is much less common that returning a node (perhaps we don't
 * even need it?)
 */
pstatus_t pstate_set_result_token(pstate_t* pstate, token_t* token) {
  ensure_empty_result_state(pstate);
  pstate->result_token = token;
  return true;
}

/**
 * @function pstate_set_result_node
 *
 * This is the common path for returning a node when the parse was
 * successful.
 */
pstatus_t pstate_set_result_node(pstate_t* pstate, parse_node_t* node) {
  ensure_empty_result_state(pstate);
  pstate->result_node = node;
  return true;
}

/**
 * @parse pstate_get_token
 *
 * Return the result token in the pstate. This is not necessarily the
 * token that is currently at the head of the stream (for that use
 * pstate_peek).
 */
token_t* pstate_get_result_token(pstate_t* pstate) {
  // TODO(jawilson): check error
  token_t* token = pstate->result_token;
  pstate->result_token = NULL;
  return token;
}

/**
 * @parse pstate_get_result_node
 *
 * Return the result node in the pstate. This will never return NULL
 * (see pstate_get_optional_result_node);
 */
parse_node_t* pstate_get_result_node(pstate_t* pstate) {
  // TODO(jawilson): check error
  parse_node_t* result = pstate->result_node;
  if (result == NULL) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  pstate->result_node = NULL;
  return result;
}

/**
 * @parse pstate_get_optional_result_node
 *
 * Return the result node in the pstate. This will never return NULL
 * (see pstate_get_optional_result_node);
 */
parse_node_t* pstate_get_optional_result_node(pstate_t* pstate) {
  // TODO(jawilson): clear error
  parse_node_t* result = pstate->result_node;
  pstate->result_node = NULL;
  return result;
}

/**
 * @parse pstate_peek
 *
 * Return the result token in the pstate. This is not necessarily the
 * token that is currently at the head of the stream (for that use
 * pstate_peek).
 */
token_t* pstate_peek(pstate_t* pstate, int offset) {
  ensure_empty_result_state(pstate);
  token_t* token = token_at(pstate->tokens, pstate->position + offset);
  return token;
}

/**
 * @parse pstate_advance
 *
 * Advance the token position by one token (and return the
 * token). Note that this will cause a fatal error if we are already
 * in an error state.
 */
token_t* pstate_advance(pstate_t* pstate) {
  if (pstate->error.parse_error_code) {
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

// TODO(jawilson):
// boolean_t pstate_match_token_type(pstate_t* pstate, "");

/**
 * @parse pstate_expect_token_string
 *
 * This is similar to pstate_match_token_string except that it is used
 * when the head token must match the given token or the parse fails
 * and the caller will return the error. On error, the token match
 * string is saved to the error so that we can present it in error
 * messages.
 */
pstatus_t pstate_expect_token_string(pstate_t* pstate, char* token_string) {
  token_t* token = pstate_peek(pstate, 0);
  if (token_matches(token, token_string)) {
    pstate->position += 1;
    return true;
  }
  pstate->error.parse_error_code = PARSE_ERROR_EXPECTED_TOKEN;
  return false;
}

// TODO(jawilson):
// pstatus_t pstate_expect_token_type(pstate_t* pstate, "");
