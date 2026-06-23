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
  roci_repl(state->env);
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

  if (is_tty) {
    roci_debugger_banner(buffer, "*** Disassembly ***");
  }
  bblock_to_buffer(buffer, state->current_bb, state->opcode_ptr);

  if (is_tty) {
    roci_debugger_instructions(buffer);
  }

  buffer_write_all(stderr, buffer);
  fflush(stderr);

  if (is_tty) {
    buffer_t* input_buffer = make_buffer(10);

    struct termios oldt = term_echo_off();

    while (true) {
      buffer_clear(buffer);
      buffer = buffer_read_ready_bytes(buffer, stdin, 10);
      if (buffer_length(buffer) > 0) {
        uint8_t byte = buffer_get(buffer, 0);
        if (byte == ' ') {
          term_echo_restore(oldt);
          break;
        } else if (byte == 'q') {
          term_echo_restore(oldt);
          term_main_buffer(buffer);
          fprintf(stderr, "\nExiting program from roci debugger.\n");
          exit(1);
        } else if (byte == 'c') {
          term_echo_restore(oldt);
          state->debug->n_instructions = 0xffffffffffffffff;
          state->debug->break_on_call_target = false;
          state->debug->break_on_return = false;
          state->debug->break_on_next_statement = false;
          state->debug->trace = false;
          break;
        } else if (byte == 'e') {
          buffer_t* buffer = make_buffer(10);
          if (is_tty) {
            roci_debugger_banner(buffer, "*** Environment ***");
          }
          roci_dump_env(state->env, buffer);
          fprintf(stdout, buffer_to_c_string(buffer));
          fflush(stdout);
        } else if (byte == 'k') {
          buffer_t* buffer = make_buffer(10);
          if (is_tty) {
            roci_debugger_banner(buffer, "*** Stack ***");
          }
          roci_dump_stack(state, buffer);
          fprintf(stdout, buffer_to_c_string(buffer));
          fflush(stdout);
        }
      }
      usleep(1000);
    }
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

void roci_debugger_banner(buffer_t* buffer, char* text) {
  int width = term_width();
  term_set_background_color(buffer, 0xbfbfbf);
  term_set_foreground_color(buffer, 0xff0000);
  term_bold(buffer);
  buffer_printf(buffer, "               %s", text);
  buffer_append_repeated_byte(buffer, ' ', width - strlen(text) - 15);
  buffer_printf(buffer, "\n", text);
  term_set_background_color(buffer, 0x000000);
  term_set_foreground_color(buffer, 0xffffff);
  term_reset_formatting(buffer);
}

void roci_debugger_instructions(buffer_t* buffer) {
  buffer_printf(buffer,
                "\n[Space] step instruction [n] next statement [c] continue "
                "[q] quit [e] dump environment [k] dump stack\n");
}
