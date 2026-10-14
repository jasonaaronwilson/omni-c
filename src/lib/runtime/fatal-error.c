/**
 * @file fatal-error.c
 *
 * This is for compatibility with older code. fatal_error's aren't
 * necessarily fatal anymore though care must still be taken if error
 * are caught and execution attempts to continue.
 */

// ======================================================================
// This is block is extraced to fatal-error.h
// ======================================================================

typedef fatal_error_config_t = struct {
  boolean_t catch_sigsegv;
};

/**
 * @macro fatal_error
 *
 * Terminates the program with a fatal error.
 */
#define fatal_error(code) (throw_error(make_error(code)))

fatal_error_config_t fatal_error_config = {0};

static error_t fatal_sigsegv_error = {
  .message = "Segmentation fault (SIGSEGV)",
  .error_code = ERROR_SIGSEGV,
  .file_name = "<signal_handler>",
  .function_name = "segmentation_fault_handler",
  .line = 0
};

void segmentation_fault_handler(int signal_number) {
  // log_warn("Caught sigsegv. Throwing an error.");
  write(STDERR_FILENO, "Caught sigsegv.\n", 16);
  throw_error(&fatal_sigsegv_error);
}

void configure_fatal_errors(fatal_error_config_t config) {
  fatal_error_config = config;
  if (config.catch_sigsegv) {
    log_warn("Registering SIGSEGV handler.");
    signal(SIGSEGV, segmentation_fault_handler);
  }
}
