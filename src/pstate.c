/**
 * @function pstate_error
 *
 * Sets the error in state (possibly overwriting an existing error
 * contained in pstate).
 */
pstatus_t pstate_error(pstate_t* pstate, uint64_t saved_position,
                       parse_error_code_t parse_error_code) {
  pstate->result_token = nullptr;
  pstate->result_node = nullptr;
  pstate->error.parse_error_code = parse_error_code;
  pstate->error.error_position = pstate->position;
  pstate->error.error_token = token_at(pstate->tokens, pstate->position);
  // TODO(jawilson): file_name?
  pstate->position = saved_position;
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
  pstate->error = compound_literal(compiler_error_t, {0});
  return pstate;
}

/**
 * @function pstate_propagate_error
 *
 * This just makes sure we really do currently have an error in pstate
 * and then restores the saved position to meet our contract of not
 * changing position unless we were successful.
 */
pstatus_t pstate_propagate_error(pstate_t* pstate, uint64_t saved_position) {
  pstate->position = saved_position;
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
  pstate->error = compound_literal(compiler_error_t, {0});
  pstate->result_node = nullptr;
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
  pstate->error = compound_literal(compiler_error_t, {0});
  pstate->result_node = node;
  pstate->result_token = nullptr;
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
  if (pstate->error.parse_error_code != PARSE_ERROR_UNKNOWN) {
    log_debug("error code is not zero");
    pstate->error = compound_literal(compiler_error_t, {0});
    // fatal_error(ERROR_ILLEGAL_STATE);
  }
  token_t* token = pstate->result_token;
  pstate->result_token = nullptr;
  return token;
}

/**
 * @parse pstate_get_result_node
 *
 * Return the result node in the pstate. This will never return nullptr
 * (see pstate_get_optional_result_node);
 */
parse_node_t* pstate_get_result_node(pstate_t* pstate) {
  if (pstate->error.parse_error_code != PARSE_ERROR_UNKNOWN) {
    log_debug("error code is not zero");
    pstate->error = compound_literal(compiler_error_t, {0});
    // fatal_error(ERROR_ILLEGAL_STATE);
  }
  parse_node_t* result = pstate->result_node;
  if (result == nullptr) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  pstate->result_node = nullptr;
  return result;
}

/**
 * @parse pstate_get_optional_result_node
 *
 * Return the result node in the pstate. This will never return nullptr
 * (see pstate_get_optional_result_node);
 */
parse_node_t* pstate_get_optional_result_node(pstate_t* pstate) {
  pstate->error = compound_literal(compiler_error_t, {0});
  parse_node_t* result = pstate->result_node;
  pstate->result_node = nullptr;
  return result;
}

/**
 * @parse pstate_peek
 *
 * Return a token at a relative offset from the current position in
 * pstate.
 */
token_t* pstate_peek(pstate_t* pstate, int offset) {
  return token_at(pstate->tokens, pstate->position + offset);
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
    log_debug("error code is not zero");
    pstate->error = compound_literal(compiler_error_t, {0});
    // fatal_error(ERROR_ILLEGAL_STATE);
  }
  token_t* token = pstate_peek(pstate, 0);
  pstate->position++;
  return token;
}

/**
 * @parse pstate_match_token_string
 *
 * This does NOT advance the position like the expect version.
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
    pstate->result_node = nullptr;
    pstate->result_token = token;
    pstate->position += 1;
    return true;
  }
  pstate->error.parse_error_code = PARSE_ERROR_EXPECTED_TOKEN;
  return false;
}

/**
 * @parse pstate_expect_token_type
 *
 * This is similar to pstate_match_token_string except that it is used
 * when the head token must match the given token or the parse fails
 * and the caller will return the error. On error, the token match
 * string is saved to the error so that we can present it in error
 * messages.
 */
pstatus_t pstate_expect_token_type(pstate_t* pstate, token_type_t token_type) {
  token_t* token = pstate_peek(pstate, 0);
  if (token_type == token->type) {
    pstate->result_node = nullptr;
    pstate->result_token = token;
    pstate->position += 1;
    return true;
  }
  pstate->error.parse_error_code = PARSE_ERROR_EXPECTED_TOKEN_TYPE;
  return false;
}
