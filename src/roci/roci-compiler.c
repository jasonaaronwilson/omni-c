/**
 * @file
 *
 * The roci compiler works by using the omni-c tokenizer to produce a
 * list of tokens which are then parsed and converted directly into
 * virtual machine instructions eliminating the need for a parse
 * tree. The grammar is simple enough that generally only a single
 * look ahead token is required. Technically we don't need semi-colons
 * given our grammar but given what a mess JavaScript got into, and
 * because we don't mind looking like C (and in fact even want to),
 * semi-colons are required as a statement terminator in cases where C
 * would require them.
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
 * record pair { car, cdr }
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
  ROCI_COMPILE_TIME_ERROR_INTERNAL_ERROR,
  ROCI_COMPILE_TIME_ERROR,
  ROCI_COMPILE_TIME_TOKENIZER_ERROR,
  ROCI_COMPILE_TIME_ERROR_BAD_STATEMENT,
  ROCI_COMPILE_TIME_ERROR_BAD_EXPRESSION,
  ROCI_COMPILE_TIME_ERROR_TOO_MANY_FIELDS,
  ROCI_COMPILE_TIME_ERROR_EXPECTED_COMMA,
  ROCI_COMPILE_TIME_ERROR_ARGUMENT_AFTER_REST,
  ROCI_COMPILE_TIME_ERROR_UNEXPECTED_ASSIGNMENT,
};

typedef roci_compiler_state_t = struct {
  roci_compile_time_error_t compiler_error;
  long bb_label_count;
  roci_bb_builder_array_t* bblocks;
  roci_bb_builder_t* current_bb;
  value_array_t* tokens;
  long position;
  long env_depth;
  uint32_t buffer_number;
};

jmp_buf roci_compiler_jmp_buf;

void roci_compiler_error(roci_compiler_state_t* state,
                         roci_compile_time_error_t error) {
  state->compiler_error = error;
  longjmp(roci_compiler_jmp_buf, 1);
}

typedef roci_compile_buffer_error_callback_data_t = struct {
  roci_compiler_state_t* state;
  char* file_name;
  buffer_t* buffer;
};

roci_compile_buffer_error_callback_data_t*
    make_compile_error_callback_data(roci_compiler_state_t* state,
                                     char* file_name, buffer_t* buffer) {
  roci_compile_buffer_error_callback_data_t* result
      = malloc_struct(roci_compile_buffer_error_callback_data_t);
  result->state = state;
  result->file_name = file_name;
  result->buffer = buffer;
  return result;
}

void roci_compile_buffer_error_callback(char* file, int line, int error_code,
                                        void* data_in) {
  roci_compile_buffer_error_callback_data_t* data
      = cast(roci_compile_buffer_error_callback_data_t*, data_in);
  log_fatal("An internal error occurred compiling %s", data->file_name);
  log_fatal("The last known position was %d", data->state->position);
  roci_compiler_error(data->state, ROCI_COMPILE_TIME_ERROR_INTERNAL_ERROR);
}

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

  set_fatal_error_callback(
      &roci_compile_buffer_error_callback,
      make_compile_error_callback_data(state, file_name, buffer));

  int jump_result = setjmp(roci_compiler_jmp_buf);
  if (jump_result == 0) {
    state->position = 0;
    state->current_bb = roci_new_bblock(state, "bb_file_start");
    state->tokens = roci_tokenize_file(state, file_name, buffer);
    roci_compile_tokens(state);
  } else {
    log_warn("roci compiler exited abnormally.");
    if (state->compiler_error == ROCI_COMPILE_TIME_ERROR_NONE) {
      state->compiler_error = ROCI_COMPILE_TIME_ERROR;
    }
  }

  set_fatal_error_callback(nullptr, nullptr);
}

/**
 * @function roci_tokenize_file
 *
 * Since roci is meant to look like C, we simply reuse the omni-c
 * tokenizer.
 */
value_array_t* roci_tokenize_file(roci_compiler_state_t* state, char* file_name,
                                  buffer_t* buffer) {
  tokenizer_result_t tokenizer_result = tokenize(buffer);
  if (tokenizer_result.tokenizer_error_code) {
    log_warn("Tokenizer error: \"%s\"::%d -- %d", file_name,
             tokenizer_result.tokenizer_error_position,
             tokenizer_result.tokenizer_error_code);
    roci_compiler_error(state, ROCI_COMPILE_TIME_TOKENIZER_ERROR);
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
  log_info("roci_compile_tokens begin");
  while (state->position < state->tokens->length) {
    roci_compile_statement(state);
  }
  log_info("roci_compile_tokens end");
  roci_emit_opcode(state, ROCI_OPCODE_TRAP);
}
