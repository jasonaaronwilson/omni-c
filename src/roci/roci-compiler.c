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
  tokenizer_result_t tokenizer_result = tokenize(buffer);
  if (tokenizer_result.tokenizer_error_code) {
    log_warn("Tokenizer error: \"%s\"::%d -- %d", file_name,
             tokenizer_result.tokenizer_error_position,
             tokenizer_result.tokenizer_error_code);
    fatal_error(ERROR_ILLEGAL_INPUT);
  }
  state->tokens = tokenizer_result.tokens;
  state->position = 0;
  roci_new_bblock(state);
  roci_compile_tokens(state);
}

void roci_compile_tokens(roci_compiler_state_t* state) {
  log_warn("roci_compile_tokens begin");
  while (state->position < state->tokens->length) {
    roci_compile_statement(state);
    if (state->compiler_error != ROCI_COMPILE_TIME_ERROR_NONE) {
      return;
    }
  }
  log_warn("roci_compile_tokens end");
}

void roci_compile_statement(roci_compiler_state_t* state) {
  token_t* token = token_at(state->tokens, state->position);
  if (token_matches(token, "return")) {
    token = token_at(state->tokens, ++state->position);
    if (token_matches(token, ";")) {
      buffer_append_byte(state->current_bb->opcodes, ROCI_OPCODE_PUSH_FALSE);
    } else {
      ++state->position;
      roci_compile_expression(state);
    }
    buffer_append_byte(state->current_bb->opcodes, ROCI_OPCODE_RETURN);
  } else if (token_matches(token, "if")) {
  } else if (token_matches(token, "while")) {
  } else {
    state->compiler_error = ROCI_COMPILE_TIME_ERROR_BAD_STATEMENT;
  }
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
      buffer_append_byte(state->current_bb->opcodes, ROCI_OPCODE_GET_VAR);
      value_array_add(state->current_bb->data,
                      u64_to_value(cast(uint64_t, varname)));
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

void roci_new_bblock(roci_compiler_state_t* state) {
  log_warn("roci_new_bblock begin");
  state->current_bb = add_bblock(state->bblocks);
  // Label's only need to be uniquely named within a
  // roci_bb_builder_array_t instance.
  state->current_bb->bblock_label
      = string_printf("bb_%d", state->bb_label_count++);
  log_warn("roci_new_bblock end");
}
