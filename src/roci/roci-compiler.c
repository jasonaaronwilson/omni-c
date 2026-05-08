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
};

// value_array_t* file_names

void roci_compile_buffer(roci_compiler_state_t* state, char* file_name,
                         buffer_t* buffer) {
  state->tokens = roci_tokenize_file(file_name, buffer);
  state->position = 0;
  state->current_bb = roci_new_bblock(state);
  roci_compile_tokens(state);
}

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

void roci_compile_tokens(roci_compiler_state_t* state) {
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
}

void roci_compile_statement(roci_compiler_state_t* state) {
  token_t* token = token_at(state->tokens, state->position);
  log_warn("CURRENT TOKEN IS %s", token_to_string(token));
  if (token_matches(token, "return")) {
    state->position++;
    token = token_at(state->tokens, state->position);
    if (token_matches(token, ";")) {
      state->position++;
      buffer_append_byte(state->current_bb->opcodes, ROCI_OPCODE_PUSH_FALSE);
    } else {
      roci_compile_expression(state);
      token = token_at(state->tokens, state->position);
      if (token_matches(token, ";")) {
        state->position++;
      } else {
        log_fatal("expected semicolon");
        fatal_error(ERROR_ILLEGAL_STATE);
      }
    }
    buffer_append_byte(state->current_bb->opcodes, ROCI_OPCODE_RETURN);
  } else if (token_matches(token, "if")) {
    roci_compile_if(state);
  } else if (token_matches(token, "while")) {
  } else {
    log_fatal("unexpected token %s", token_to_string(token));
    fatal_error(ERROR_ILLEGAL_INPUT);
  }
}

void roci_compile_if(roci_compiler_state_t* state) {

  token_t* open = token_at(state->tokens, ++state->position);
  if (!token_matches(open, "(")) {
    fatal_error(ERROR_ILLEGAL_INPUT);
  }
  state->position++;
  roci_compile_expression(state);
  token_t* close = token_at(state->tokens, state->position++);
  if (!token_matches(close, ")")) {
    fatal_error(ERROR_ILLEGAL_INPUT);
  }
  roci_bb_builder_t* if_bb = state->current_bb;

  roci_bb_builder_t* true_bb = roci_compile_block(state);
  roci_bb_builder_t* end_of_true_bb = state->current_bb;

  roci_emit_br_true(if_bb, true_bb);

  roci_bb_builder_t* false_bb = nullptr;

  token_t* peek_token = token_at(state->tokens, state->position);
  if (token_matches(peek_token, "else")) {
    state->position++;
    roci_bb_builder_t* false_bb = roci_compile_block(state);
    roci_bb_builder_t* after_bb = roci_new_bblock(state);
    roci_emit_branch(if_bb, false_bb);
    roci_emit_branch(state->current_bb, after_bb);
    roci_emit_branch(end_of_true_bb, after_bb);
    state->current_bb = after_bb;
  } else {
    roci_bb_builder_t* after_bb = roci_new_bblock(state);
    roci_emit_branch(end_of_true_bb, after_bb);
    roci_emit_branch(if_bb, after_bb);
    state->current_bb = after_bb;
  }
}

roci_bb_builder_t* roci_compile_block(roci_compiler_state_t* state) {
  roci_bb_builder_t* result_bb = roci_new_bblock(state);
  state->current_bb = result_bb;
  roci_expect_token(state, "{");

  while (state->position < state->tokens->length) {
    roci_compile_statement(state);
    token_t* close_b = roci_peek_token(state);
    if (token_matches(close_b, "}")) {
      roci_skip_token(state);
      return result_bb;
    }
  }
  log_fatal("closing brace not found!");
  fatal_error(ERROR_ILLEGAL_INPUT);
}

void roci_compile_expression(roci_compiler_state_t* state) {
  token_t* token = token_at(state->tokens, state->position);
  if (token_matches(token, "fn")) {
    // must be a closure
  } else if (token_matches(token, "(")) {
    // parenthesized expression
  }

  token_t* token_next = token_at(state->tokens, state->position + 1);
  if (token_matches(token_next, "(")) {
    // function call
  } else if (token_matches(token_next, ";") || token_matches(token_next, ",")
             || token_matches(token_next, ")")) {
    switch (token->type) {
    case TOKEN_TYPE_IDENTIFIER:
      char* varname = token_to_string(token);
      if (string_equal(varname, "true")) {
        buffer_append_byte(state->current_bb->opcodes, ROCI_OPCODE_PUSH_TRUE);
      } else if (string_equal(varname, "false")) {
        buffer_append_byte(state->current_bb->opcodes, ROCI_OPCODE_PUSH_FALSE);
      } else {
        buffer_append_byte(state->current_bb->opcodes, ROCI_OPCODE_GET_VAR);
        value_array_add(state->current_bb->data,
                        u64_to_value(cast(uint64_t, varname)));
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
    ++state->position;
  } else {
    state->compiler_error = ROCI_COMPILE_TIME_ERROR_BAD_EXPRESSION;
  }
}

roci_bb_builder_t* roci_new_bblock(roci_compiler_state_t* state) {
  roci_bb_builder_t* result = add_bblock(state->bblocks);
  result->bblock_label = string_printf("bb_%d", state->bb_label_count++);
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
