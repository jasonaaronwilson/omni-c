/**
 * @file roci-debugger.c
 *
 * When an error occurs and we are running in a terminal, a simple
 * source level debugger is available. This allows the stack to be
 * examined, the current environment, single stepping by instruction,
 * or by statement.
 */

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

void roci_debug_error(roci_vm_state_t* state, char* error_message) {
  log_fatal(error_message);
  fatal_error(ERROR_ILLEGAL_STATE);
}

void roci_debug_trace(roci_vm_state_t* state, buffer_t* buffer) {
  buffer_clear(buffer);
  buffer_printf(buffer,
                "--------------------------------------------------------------"
                "--------\n");
  buffer_printf(
      buffer, "%s:     ", uint64_to_string(cast(uint64_t, state->opcode_ptr)));
  roci_instruction_to_buffer(buffer, state->opcode_ptr, state->data_ptr);
  roci_dump_env(state->env, buffer);
  roci_dump_stack(state, buffer);
  buffer_write_all(stderr, buffer);
  fflush(stderr);
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
