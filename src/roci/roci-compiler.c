/**
 * @file
 *
 * The roci compiler works by using the omni-c tokenizer to produce a
 * list of tokens which are then parsed and converted directly into
 * virtual machine instructions eliminating the need for a parse
 * tree. The grammar is simple enough that only a single look ahead
 * token is required. Technically we don't need semi-colons given our
 * grammar but given what a mess JavaScript got into, and because we
 * don't mind looking like C (and in fact even want to), semi-colons
 * are required as a statement terminator in cases where C would
 * require them.
 *
 * The roci language looks like:
 *
 * Statements
 *
 * ```
 * let v = expr; // variable definition
 * v = expr; // variable assignment
 * fn_name(exprs...); // function call
 * if (expr) { statements }
 * if (expr) { statements } else { statements }
 * if (expr) { statements } else if (expr) { statements } else { statements }
 * while (expr) { statements } // no break or continue yet
 * return;
 * return expr;
 * ```
 *
 * Expressions
 *
 * ```
 * v // variable reference
 * fn_name(exprs...) // function call
 * fn(args) { statements } // a function definition / lambda expression
 * ```
 */

typedef roci_compile_time_error_t = enum {
  ROCI_COMPILE_TIME_ERROR_NONE,
  ROCI_COMPILE_TIME_ERROR_BAD_STATEMENT,
  ROCI_COMPILE_TIME_ERROR_BAD_EXPRESSION,
};

typedef roci_compiler_state_t = struct {
  roci_compile_time_error_t compiler_error;
  long bb_label_count;
  roci_bb_builder_array_t* bblocks;
  roci_bb_builder_t* current_bb;
  value_array_t* tokens;
  long position;
  long env_depth;
};

/**
 * @function roci_compile_buffer
 *
 * This is the heart of the roci "interpreter". Rather than parse into
 * a syntax tree and try to interpret, we actually parse and compile
 * into the roci VM language and then execute *that*.
 *
 * Most people would be surprised to find a compiler inside of their
 * compiler! This one is pretty simple though and will eventually
 * remove a dependence on lua and other tools like GNU make.
 */
void roci_compile_buffer(roci_compiler_state_t* state, char* file_name,
                         buffer_t* buffer) {
  state->tokens = roci_tokenize_file(file_name, buffer);
  state->position = 0;
  state->current_bb = roci_new_bblock(state, "bb_file_start");
  roci_compile_tokens(state);
}

/**
 * @function roci_tokenize_file
 *
 * Since roci is meant to look like C, we simply reuse the omni-c
 * tokenizer.
 */
value_array_t* roci_tokenize_file(char* file_name, buffer_t* buffer) {
  tokenizer_result_t tokenizer_result = tokenize(buffer);
  if (tokenizer_result.tokenizer_error_code) {
    log_warn("Tokenizer error: \"%s\"::%d -- %d", file_name,
             tokenizer_result.tokenizer_error_position,
             tokenizer_result.tokenizer_error_code);
    fatal_error(ERROR_ILLEGAL_INPUT);
  }
  return transform_tokens(
      tokenizer_result.tokens,
      compound_literal(token_transformer_options_t,
                       {
                           .keep_whitespace = false,
                           .keep_comments = false,
                           .keep_javadoc_comments = false,
                           .keep_c_preprocessor_lines = false,
                       }));
}

/**
 * @function roci_tokenize_file
 *
 * Once tokenization has been performed, compile all of the
 * statements in the file.
 */
void roci_compile_tokens(roci_compiler_state_t* state) {
  // TODO(jawilson): make "return" illegal at this level. We can
  // provide a function like exit though.
  log_warn("roci_compile_tokens begin");
  while (state->position < state->tokens->length) {
    roci_compile_statement(state);
    if (state->compiler_error != ROCI_COMPILE_TIME_ERROR_NONE) {
      log_fatal("compiler error %s",
                roci_compile_time_error_to_string(state->compiler_error));
      fatal_error(ERROR_ILLEGAL_STATE);
    }
  }
  log_warn("roci_compile_tokens end");
  roci_emit_opcode(state, ROCI_OPCODE_TRAP);
}

/**
 * @function roci_compile_statement
 *
 * Compile a roci statement.
 */
void roci_compile_statement(roci_compiler_state_t* state) {
  token_t* token = roci_peek_token(state);
  log_warn("CURRENT TOKEN IS %s", token_to_string(token));
  if (token_matches(token, "return")) {
    roci_compile_return(state);
  } else if (token_matches(token, "if")) {
    roci_compile_if(state);
  } else if (token_matches(token, "let")) {
    roci_compile_let(state);
  } else if (token_matches(token, "while")) {
    roci_compile_while(state);
  } else {
    token = token_at(state->tokens, state->position + 1);
    if (token_matches(token, "=")) {
      roci_compile_assignment(state);
    } else if (token_matches(token, "(")) {
      roci_compile_function_call(state);
      roci_expect_token(state, ";");
      roci_emit_opcode(state, ROCI_OPCODE_DROP);
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
  roci_emit_debug_info(state, "", roci_peek_token(state));
  roci_expect_token(state, "return");
  token_t* token = roci_peek_token(state);
  if (token_matches(token, ";")) {
    roci_skip_token(state);
    roci_emit_opcode(state, ROCI_OPCODE_PUSH_FALSE);
  } else {
    roci_compile_expression(state);
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
  roci_emit_debug_info(state, "", roci_peek_token(state));
  roci_expect_token(state, "let");
  token_t* varname = roci_next_token(state);
  roci_verify_identifier(varname);
  roci_expect_token(state, "=");
  roci_compile_expression(state);
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
  roci_emit_debug_info(state, "", roci_peek_token(state));
  token_t* varname = roci_next_token(state);
  // verify identifier
  roci_expect_token(state, "=");
  roci_compile_expression(state);
  roci_expect_token(state, ";");
  roci_emit_token_string_datum(state, token_to_string(varname));
  roci_emit_opcode(state, ROCI_OPCODE_DEFINE_VAR);
}

/**
 * @function roci_compile_if
 *
 * Compile a roci if statement.
 */
void roci_compile_if(roci_compiler_state_t* state) {
  roci_emit_debug_info(state, "", roci_peek_token(state));
  roci_expect_token(state, "if");
  roci_expect_token(state, "(");
  roci_compile_expression(state);
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

  roci_emit_debug_info(state, "", roci_peek_token(state));
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
  log_fatal("closing brace not found!");
  fatal_error(ERROR_ILLEGAL_INPUT);
}

// This isn't quite right yet...
void roci_compile_while(roci_compiler_state_t* state) {
  roci_emit_debug_info(state, "", roci_peek_token(state));
  roci_expect_token(state, "while");
  roci_expect_token(state, "(");

  // 1. Create a block for the condition and jump there
  roci_bb_builder_t* cond_bb = roci_new_bblock(state, "while_cond_bb");
  roci_emit_branch(state->current_bb, cond_bb);
  state->current_bb = cond_bb;

  // 2. Compile the condition expression
  roci_compile_expression(state);
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

/**
 * @function roci_compile_expression
 *
 * Compile a roci expression. The expressions allowed are quite
 * limited unlike C hence zero effort to port/reuse the pratt parser
 * we already had for omni-c.
 */
void roci_compile_expression(roci_compiler_state_t* state) {
  token_t* token = roci_peek_token(state);
  if (token_matches(token, "fn")) {
    roci_compile_closure(state);
    return;
  } else if (token_matches(token, "(")) {
    // parenthesized expression
  }

  token_t* token_next = token_at(state->tokens, state->position + 1);
  if (token_matches(token_next, "(")) {
    roci_compile_function_call(state);
  } else if (token_matches(token_next, ";") || token_matches(token_next, ",")
             || token_matches(token_next, ")")) {
    switch (token->type) {
    case TOKEN_TYPE_IDENTIFIER:
      char* varname = token_to_string(token);
      if (string_equal(varname, "true")) {
        roci_emit_opcode(state, ROCI_OPCODE_PUSH_TRUE);
      } else if (string_equal(varname, "false")) {
        roci_emit_opcode(state, ROCI_OPCODE_PUSH_FALSE);
      } else {
        roci_emit_get_var(state->current_bb, varname);
      }
      break;

    case TOKEN_TYPE_INTEGER_LITERAL:
      value_result_t parsed = string_parse_uint64(token_to_string(token));
      if (parsed.nf_error != NF_OK) {
        log_fatal("failed to parse integer");
        fatal_error(ERROR_ILLEGAL_INPUT);
      }
      buffer_append_byte(state->current_bb->opcodes, ROCI_OPCODE_PUSH_INTEGER);
      value_array_add(state->current_bb->data, parsed.val);
      break;

    case TOKEN_TYPE_FLOAT_LITERAL:
      double dbl = string_parse_double(token_to_string(token));
      buffer_append_byte(state->current_bb->opcodes, ROCI_OPCODE_PUSH_DOUBLE);
      value_array_add(state->current_bb->data, dbl_to_value(dbl));
      break;

    case TOKEN_TYPE_STRING_LITERAL:
      char* str = token_to_string(token);
      buffer_append_byte(state->current_bb->opcodes, ROCI_OPCODE_PUSH_STRING);
      value_array_add(state->current_bb->data, str_to_value(str));
      break;

    default:
      log_fatal("unexpected token");
      fatal_error(ERROR_ILLEGAL_INPUT);
    }
    roci_skip_token(state);
  } else {
    state->compiler_error = ROCI_COMPILE_TIME_ERROR_BAD_EXPRESSION;
  }
}

void roci_compile_function_call(roci_compiler_state_t* state) {
  roci_emit_debug_info(state, "", roci_peek_token(state));
  int num_args = 0;
  token_t* fn_name = roci_next_token(state);
  roci_expect_token(state, "(");
  while (true) {
    token_t* token = roci_peek_token(state);
    if (token_matches(token, ")")) {
      roci_skip_token(state);
      break;
    }
    roci_compile_expression(state);
    num_args++;
    token = roci_peek_token(state);
    if (token_matches(token, ",")) {
      roci_skip_token(state);
    } else if (!token_matches(token, ")")) {
      log_fatal("Expected comma or close paren.");
      fatal_error(ERROR_ILLEGAL_STATE);
    }
  }

  roci_emit_get_var(state->current_bb, token_to_string(fn_name));

  roci_bb_builder_t* return_bb = roci_new_bblock(state, "return_bb");
  buffer_append_byte(state->current_bb->opcodes, ROCI_OPCODE_CALL_0 + num_args);
  value_array_add(state->current_bb->data,
                  str_to_value(return_bb->bblock_label));
  state->current_bb = return_bb;
}

void roci_compile_closure(roci_compiler_state_t* state) {
  token_t* first_token = roci_peek_token(state);
  roci_expect_token(state, "fn");
  roci_expect_token(state, "(");

  token_t* args[32];
  int64_t arg_num = roci_collect_fn_args(state, args);

  roci_bb_builder_t* current_bb = state->current_bb;
  int current_env_depth = state->env_depth;

  roci_bb_builder_t* fn_entry_bb = roci_new_bblock(state, "fn_header");
  state->env_depth = 1; // calling a closure puts an item on the env stack
  state->current_bb = fn_entry_bb;

  roci_emit_debug_info(state, "", first_token);
  roci_emit_opcode(state, ROCI_OPCODE_CHECK_ARGS);
  roci_emit_int_datum(state, arg_num);

  roci_emit_new_environment(state);
  while (arg_num > 0) {
    token_t* varname = args[--arg_num];
    roci_emit_token_string_datum(state, token_to_string(varname));
    roci_emit_opcode(state, ROCI_OPCODE_DEFINE_VAR);
  }

  roci_bb_builder_t* body_bb = roci_compile_block(state);
  roci_emit_opcode(state, ROCI_OPCODE_PUSH_FALSE);
  roci_emit_return(state);

  roci_emit_branch(fn_entry_bb, body_bb);

  state->env_depth = current_env_depth;
  state->current_bb = current_bb;
  roci_emit_token_string_datum(state, fn_entry_bb->bblock_label);
  roci_emit_opcode(state, ROCI_OPCODE_MAKE_CLOSURE);
}

int64_t roci_collect_fn_args(roci_compiler_state_t* state, token_t** args) {
  int64_t arg_num = 0;
  while (true) {
    token_t* token = roci_peek_token(state);
    if (token_matches(token, ")")) {
      roci_skip_token(state);
      break;
    }
    roci_verify_identifier(token);
    roci_skip_token(state);

    args[arg_num++] = token;
    token = roci_peek_token(state);
    if (token_matches(token, ",")) {
      roci_skip_token(state);
    }
  }
  return arg_num;
}

/*
 * A mini-library for emitting roci VM instructions.
 */
void roci_emit_opcode(roci_compiler_state_t* state, roci_opcode_t opcode) {
  buffer_append_byte(state->current_bb->opcodes, opcode);
}

void roci_emit_token_string_datum(roci_compiler_state_t* state, char* str) {
  value_array_add(state->current_bb->data, str_to_value(str));
}

void roci_emit_int_datum(roci_compiler_state_t* state, uint64_t val) {
  value_array_add(state->current_bb->data, u64_to_value(val));
}

void roci_emit_new_environment(roci_compiler_state_t* state) {
  state->env_depth++;
  roci_emit_opcode(state, ROCI_OPCODE_NEW_ENVIRONMENT);
}

void roci_emit_drop_environment(roci_compiler_state_t* state) {
  state->env_depth--;
  roci_emit_opcode(state, ROCI_OPCODE_DROP_ENVIRONMENT);
}

void roci_emit_return(roci_compiler_state_t* state) {
  roci_emit_opcode(state, ROCI_OPCODE_RETURN);
}

void roci_emit_get_var(roci_bb_builder_t* bb, char* fn_name) {
  buffer_append_byte(bb->opcodes, ROCI_OPCODE_GET_VAR);
  value_array_add(bb->data, ptr_to_value(fn_name));
}

void roci_emit_comment(roci_bb_builder_t* bb, char* str) {
  buffer_append_byte(bb->opcodes, ROCI_OPCODE_COMMENT);
  value_array_add(bb->data, ptr_to_value(str));
}

void roci_emit_debug_info(roci_compiler_state_t* state, char* filename,
                          token_t* token) {
  roci_emit_opcode(state, ROCI_OPCODE_DEBUG_INFO);
  roci_emit_int_datum(state, token_to_roci_src_info(0, token));
}

/**
 * @function roci_new_bblock
 *
 * Return a new roci_bb_builder_t*.
 *
 * The order of bblocks is technically not relevant but effort has
 * been made above, for example compile_if, so that the disassembly
 * looks like "real" assembly language will confuse us humans less. The
 * person writing this is definitely not AI.
 */
roci_bb_builder_t* roci_new_bblock(roci_compiler_state_t* state,
                                   char* label_prefix) {
  if (label_prefix == nullptr) {
    label_prefix = "bb_";
  }
  roci_bb_builder_t* result = add_bblock(state->bblocks);
  result->bblock_label
      = string_printf("%s%d", label_prefix, state->bb_label_count++);
  roci_emit_comment(result, result->bblock_label);
  return result;
}

void roci_emit_branch(roci_bb_builder_t* src_bblock,
                      roci_bb_builder_t* tgt_bblock) {
  buffer_append_byte(src_bblock->opcodes, ROCI_OPCODE_BR);
  value_array_add(src_bblock->data, ptr_to_value(tgt_bblock->bblock_label));
}

void roci_emit_br_true(roci_bb_builder_t* src_bblock,
                       roci_bb_builder_t* tgt_bblock) {
  buffer_append_byte(src_bblock->opcodes, ROCI_OPCODE_BR_TRUE);
  value_array_add(src_bblock->data, ptr_to_value(tgt_bblock->bblock_label));
}

/*
 * A mini-library for dealing with tokens more as a stream.
 *
 * For design reasons, we already know that we don't need to look more
 * than one token ahead.
 */

// TODO(jawilson): see if we can use roci_eof to find errors earlier.

boolean_t roci_eof(roci_compiler_state_t* state) {
  return state->position >= state->tokens->length;
}

void roci_skip_token(roci_compiler_state_t* state) { roci_next_token(state); }

token_t* roci_peek_token(roci_compiler_state_t* state) {
  return token_at(state->tokens, state->position);
}

token_t* roci_next_token(roci_compiler_state_t* state) {
  return token_at(state->tokens, state->position++);
}

void roci_expect_token(roci_compiler_state_t* state, char* token_string) {
  token_t* token = roci_next_token(state);
  if (!token_matches(token, token_string)) {
    log_fatal("roci expected %s as the next token but got %s", token_string,
              token_to_string(token));
    fatal_error(ERROR_ILLEGAL_INPUT);
  }
}

void roci_verify_identifier(token_t* token) {
  if (token->type != TOKEN_TYPE_IDENTIFIER) {
    fatal_error(ERROR_ILLEGAL_INPUT);
  }
  // Make sure our unique keywords don't match either.
}
