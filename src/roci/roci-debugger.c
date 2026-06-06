/**
 * @file roci-debugger.c
 *
 * The Roci language has a debugger "interface" built in.
 *
 * This is the crude first version.
 */

/** ================================================================ */

// A quick little abstraction. I'm considering also adding a column
// number for expression type stuff so I wanted a bit of freedom.

typedef roci_src_info_t = uint64_t;

static inline roci_src_info_t token_to_roci_src_info(uint64_t file_number,
                                                     token_t* token) {
  uint64_t line_number = token->line_number;
  return cast(roci_src_info_t, file_number | line_number << 32);
}

static inline uint64_t roci_src_file_number(roci_src_info_t info) {
  return info & 0xffffffff;
}

static inline uint64_t roci_src_line_number(roci_src_info_t info) {
  return info >> 32;
}

/** ================================================================ */

void roci_debug_error(roci_vm_state_t* state, char* error_message) {
  log_fatal(error_message);
  fatal_error(ERROR_ILLEGAL_STATE);
}

void roci_debug_trace(roci_vm_state_t* state, buffer_t* buffer) {
  boolean_t is_tty = roci_is_tty();

  buffer_clear(buffer);

  if (!is_tty) {
    buffer_printf(
        buffer,
        "--------------------------------------------------------------"
        "--------\n");
  } else {
    term_home(buffer);
    term_alt_buffer(buffer);
    term_clear_screen(buffer);
  }

  /*
  buffer_printf(
      buffer, "%s:     ", uint64_to_string(cast(uint64_t, state->opcode_ptr)));
  */

  bblock_to_buffer(buffer, state->current_bb, state->opcode_ptr);
  roci_dump_env(state->env, buffer);
  roci_dump_stack(state, buffer);

  buffer_write_all(stderr, buffer);
  fflush(stderr);

  if (is_tty) {
    sleep(3);
    term_main_buffer(buffer);
  }
}

void roci_debug_breakpoint(void) {}

void roci_dump_stack(roci_vm_state_t* state, buffer_t* buffer) {

  for (int i = 1; i < 1024; i++) {
    roci_value_t value = roci_debug_peek_value(state, i);
    if (value.tag == ROCI_TAG_STACK_MARKER || value.tag == ROCI_TAG_UNKNOWN) {
      return;
    }
    buffer_printf(buffer, "stack[%d] = %s\n", i, roci_value_to_c_string(value));
  }
  log_fatal("ROCI_TAG_STACK_MARKER not found.");
  fatal_error(ERROR_ILLEGAL_STATE);
}

boolean_t roci_is_tty(void) {
  return isatty(fileno(stdout)) && !string_equal("dumb", getenv("TERM"));
}
