/*
 * A mini-library for dealing with tokens more as a stream.
 */

// TODO(jawilson): see if we can use roci_eof to find errors earlier.

boolean_t roci_eof(roci_compiler_state_t* state) {
  return state->position >= state->tokens->length;
}

void roci_skip_token(roci_compiler_state_t* state) { roci_next_token(state); }

token_t* roci_peek_token(roci_compiler_state_t* state) {
  return token_at(state->tokens, state->position);
}

token_t* roci_peek_over_tokens(roci_compiler_state_t* state, uint64_t n) {
  return token_at(state->tokens, state->position + n);
}

token_t* roci_next_token(roci_compiler_state_t* state) {
  return token_at(state->tokens, state->position++);
}

void roci_expect_token(roci_compiler_state_t* state, char* token_string) {
  token_t* token = roci_next_token(state);
  if (!token_matches(token, token_string)) {
    log_warn("roci expected %s as the next token but got %s at line %d col %d",
             token_string, token_to_string(token), token->line_number,
             token->column_number);
    roci_compiler_error(state, ROCI_COMPILE_TIME_ERROR);
  }
}
