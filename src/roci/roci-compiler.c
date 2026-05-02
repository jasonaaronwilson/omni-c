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
}

void roci_compile_tokens(roci_compiler_state_t* state) {
  while (state->position < state->tokens->length) {
    roci_compile_statement(state);
    if (state->compiler_error != ROCI_COMPILE_TIME_ERROR_NONE) {
      return;
    }
  }
}

void roci_compile_statement(roci_compiler_state_t* state) {
  token_t* token = token_at(state->tokens, state->position);
  if (token_matches(token, "return")) {
    token = token_at(state->tokens, ++state->position);
    if (token_matches(token, ";")) {
      buffer_append_byte(state->current_bb->opcodes, ROCI_OPCODE_PUSH_FALSE);
    } else {
      // TODO(jawilson): parse/compile expression
    }
  } else {
    state->compiler_error = ROCI_COMPILE_TIME_ERROR_BAD_STATEMENT;
  }
}

void roci_compile_expression(roci_compiler_state_t* state) {}

void roci_new_bblock(roci_compiler_state_t* state) {
  state->current_bb = add_bblock(state->bblocks);
  state->current_bb->bblock_label = "FIXME";
}
