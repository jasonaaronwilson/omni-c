/**
 * @function roci_compile_statement
 *
 * Compile a roci statement.
 */
void roci_compile_statement(roci_compiler_state_t* state) {
  token_t* token = roci_peek_token(state);
  log_info("CURRENT TOKEN IS %s", token_to_string(token));
  if (token_matches(token, "return")) {
    roci_compile_return(state);
  } else if (token_matches(token, "if")) {
    roci_compile_if(state);
  } else if (token_matches(token, "let")) {
    roci_compile_let(state);
  } else if (token_matches(token, "while")) {
    roci_compile_while(state);
  } else if (token_matches(token, "record")) {
    roci_compile_record(state);
  } else {
    token = token_at(state->tokens, state->position + 1);
    if (token_matches(token, "=")) {
      roci_compile_assignment(state);
    } else if (token_matches(token, "(")) {
      roci_compile_function_call(state);
      roci_expect_token(state, ";");
      roci_emit_opcode(state, ROCI_OPCODE_DROP);
    } else {
      buffer_t* buffer = make_buffer(5);
      append_token_debug_string(buffer, *token);
      log_warn("roci_compile_statement is unhappy! %s",
               buffer_to_c_string(buffer));
      roci_compiler_error(state, ROCI_COMPILE_TIME_ERROR_BAD_STATEMENT);
    }
  }
}

/**
 * @function roci_compile_return
 *
 * Compile a roci return statement.
 *
 * roci is a dynamic langage so we kind of need to return something in
 * case the caller then assigns the value somewhere or uses it in a
 * call. For now, we return false when there isn't an associated
 * expression. (I'm thinking of having a mechanism to detect using
 * "void" results when assigning to a variable or calling another
 * function though this will bloat code (just a little) somewhere.)
 */
void roci_compile_return(roci_compiler_state_t* state) {
  roci_emit_debug_info(state, roci_peek_token(state));
  roci_expect_token(state, "return");
  token_t* token = roci_peek_token(state);
  if (token_matches(token, ";")) {
    roci_skip_token(state);
    roci_emit_opcode(state, ROCI_OPCODE_PUSH_FALSE);
  } else {
    roci_compile_expression(state, false);
    roci_expect_token(state, ";");
  }
  roci_emit_return(state);
}

/**
 * @function roci_compile_let
 *
 * Compile a roci let statement. This always creates a new binding
 * kind of like ":=" in go.
 *
 * This binding will disappear once the current basic block finishes
 * execution (unless it's captured in a closure, TBD).
 *
 * Currently it is legal to shadow the same name of a variable defined
 * in a parent environment though this may change once we implement
 * fluid_let.
 */
void roci_compile_let(roci_compiler_state_t* state) {
  roci_emit_debug_info(state, roci_peek_token(state));
  roci_expect_token(state, "let");
  token_t* varname = roci_next_token(state);
  roci_verify_identifier(state, varname);
  roci_expect_token(state, "=");
  roci_compile_expression(state, false);
  roci_expect_token(state, ";");
  roci_emit_token_string_datum(state, token_to_string(varname));
  roci_emit_opcode(state, ROCI_OPCODE_DEFINE_VAR);
}

/**
 * @function roci_compile_assignment
 *
 * Compile a roci assignment statement.
 *
 * Assignments look like "var = expr;".
 */
void roci_compile_assignment(roci_compiler_state_t* state) {
  roci_emit_debug_info(state, roci_peek_token(state));
  token_t* varname = roci_next_token(state);
  // verify identifier
  roci_expect_token(state, "=");
  roci_compile_expression(state, false);
  roci_expect_token(state, ";");
  roci_emit_token_string_datum(state, token_to_string(varname));
  roci_emit_opcode(state, ROCI_OPCODE_SET_VAR);
}

/**
 * @function roci_compile_if
 *
 * Compile a roci if statement.
 */
void roci_compile_if(roci_compiler_state_t* state) {
  roci_emit_debug_info(state, roci_peek_token(state));
  roci_expect_token(state, "if");
  roci_expect_token(state, "(");
  roci_compile_expression(state, false);
  roci_expect_token(state, ")");

  roci_bb_builder_t* if_bb = state->current_bb;

  roci_bb_builder_t* true_bb = roci_compile_block(state);
  roci_bb_builder_t* end_of_true_bb = state->current_bb;

  roci_emit_br_true(if_bb, true_bb);

  token_t* peek_token = roci_peek_token(state);
  if (token_matches(peek_token, "else")) {
    roci_skip_token(state);
    roci_bb_builder_t* false_bb = roci_compile_block(state);
    roci_bb_builder_t* after_bb = roci_new_bblock(state, "if_after_bb");
    roci_emit_branch(if_bb, false_bb);
    roci_emit_branch(state->current_bb, after_bb);
    roci_emit_branch(end_of_true_bb, after_bb);
    state->current_bb = after_bb;
  } else {
    roci_bb_builder_t* after_bb = roci_new_bblock(state, "if_after_bb");
    roci_emit_branch(end_of_true_bb, after_bb);
    roci_emit_branch(if_bb, after_bb);
    state->current_bb = after_bb;
  }
}

/**
 * @function roci_compile_block
 *
 * Compile a roci block and return the initial bblock.
 *
 * Blocks start with '{' and end with '}' and have zero or more
 * statements inside of them. Eventually blocks will create new
 * environments which will allow more abstraction.
 */
roci_bb_builder_t* roci_compile_block(roci_compiler_state_t* state) {
  roci_bb_builder_t* result_bb = roci_new_bblock(state, "block_bb_");
  state->current_bb = result_bb;

  roci_emit_debug_info(state, roci_peek_token(state));
  roci_emit_new_environment(state);
  roci_expect_token(state, "{");

  while (state->position < state->tokens->length) {
    roci_compile_statement(state);
    token_t* close_b = roci_peek_token(state);
    if (token_matches(close_b, "}")) {
      roci_skip_token(state);
      roci_emit_drop_environment(state);
      return result_bb;
    }
  }
  log_warn("closing brace not found!");
  roci_compiler_error(state, ROCI_COMPILE_TIME_ERROR_BAD_STATEMENT);
  // Not reached...
  return nullptr;
}

// This isn't quite right yet...
void roci_compile_while(roci_compiler_state_t* state) {
  roci_emit_debug_info(state, roci_peek_token(state));
  roci_expect_token(state, "while");
  roci_expect_token(state, "(");

  // 1. Create a block for the condition and jump there
  roci_bb_builder_t* cond_bb = roci_new_bblock(state, "while_cond_bb");
  roci_emit_branch(state->current_bb, cond_bb);
  state->current_bb = cond_bb;

  // 2. Compile the condition expression
  roci_compile_expression(state, false);
  roci_expect_token(state, ")");

  // 3. Compile the body
  roci_bb_builder_t* body_bb = roci_compile_block(state);
  roci_bb_builder_t* end_of_body_bb = state->current_bb;

  // 4. Create the exit block for when the loop finishes
  roci_bb_builder_t* after_bb = roci_new_bblock(state, "while_after_bb");

  // Loop back: End of the body jumps back to the condition
  roci_emit_branch(end_of_body_bb, cond_bb);

  // Condition branches: If true, go to body. If false, go to after_bb.
  roci_emit_br_true(cond_bb, body_bb);
  roci_emit_branch(cond_bb, after_bb);

  state->current_bb = after_bb;
}
