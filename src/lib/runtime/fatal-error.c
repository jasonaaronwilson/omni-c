/**
 * @file fatal-error.c
 *
 * This is for compatibility with older code. fatal_error's aren't
 * necessarily fatal anymore though care must still be taken if error
 * are caught and execution attempts to continue.
 */

/**
 * @macro fatal_error
 *
 * Terminates the program with a fatal error.
 */
#define fatal_error(code) (throw_error(make_error(code)))

/**
 * @function install_signal_handlers
 *
 * Install standard signal handlers that turn things like SIGSEGV into
 * thrown errors.
 */
void install_signal_handlers(void) {
  log_info("Registering SIGSEGV handler.");
  signal(SIGSEGV, segmentation_fault_handler);
}

void segmentation_fault_handler(int signal_number) {
  // log_warn("Caught sigsegv. Throwing an error.");
  write(STDERR_FILENO, "Caught sigsegv.\n", 16);
  throw_error(&fatal_sigsegv_error);
}

static error_t fatal_sigsegv_error = {
  .message = "Segmentation fault (SIGSEGV)",
  .error_code = ERROR_SIGSEGV,
  .file_name = "<signal_handler>",
  .function_name = "segmentation_fault_handler",
  .line = 0
};
