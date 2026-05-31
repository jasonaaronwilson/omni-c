/**
 * @file roci-debugger.c
 *
 * When an error occurs and we are running in a terminal, a simple
 * source level debugger is available. This allows the stack to be
 * examined, the current environment, single stepping by instruction,
 * or by statement.
 */

void roci_debug_error(roci_vm_state_t* state, char* error_message) {
  log_fatal(error_message);
  fatal_error(ERROR_ILLEGAL_STATE);
}
