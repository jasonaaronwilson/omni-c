/* Signal definitions and error mapping */

static const int POSIX_SIGNAL_NUMS[]
    = {SIGSEGV, SIGBUS, SIGFPE, SIGILL, SIGABRT, SIGSYS, 0};

/**
 * @function install_signal_handlers
 *
 * Configures handlers for hardware and POSIX faults to translate
 * them into non-local jump errors.
 */
void install_signal_handlers(void) {
  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));
  sa.sa_sigaction = posix_fault_handler;
  sa.sa_flags = SA_SIGINFO | SA_NODEFER;
  sigemptyset(&sa.sa_mask);

  for (size_t i = 0; POSIX_SIGNAL_NUMS[i] != 0; i++) {
    int signum = POSIX_SIGNAL_NUMS[i];
    log_info("Registering handler for signal %d", signum);
    if (sigaction(signum, &sa, NULL) != 0) {
      log_warn("Failed to register signal handler for %d", signum);
    }
  }
}

static void posix_fault_handler(int signum, siginfo_t* info, void* ucontext) {
  write(STDERR_FILENO, "Trapped signal\n", 15);

  /* Explicitly unmask the signal before unwinding so subsequent faults aren't
   * blocked */
  sigset_t mask = {0};
  sigemptyset(&mask);
  sigaddset(&mask, signum);
  sigprocmask(SIG_UNBLOCK, &mask, NULL);

  error_t* error = &sig_default_error;

  switch (signum) {
  case SIGSEGV:
    error = &sigsegv_error;
    break;
  case SIGBUS:
    error = &sigbus_error;
    break;
  case SIGFPE:
    error = &sigfpe_error;
    break;
  case SIGILL:
    error = &sigill_error;
    break;
  case SIGABRT:
    error = &sigabrt_error;
    break;
  case SIGSYS:
    error = &sigsys_error;
    break;
  default:
    break;
  }

  throw_error(error);
}

static error_t sigsegv_error = {.message = "Segmentation fault (SIGSEGV)",
                                .error_code = ERROR_SIGSEGV,
                                .file_name = "<signal_handler>",
                                .function_name = "posix_fault_handler"};

static error_t sigbus_error = {.message = "Bus error (SIGBUS)",
                               .error_code = ERROR_SIGBUS,
                               .file_name = "<signal_handler>",
                               .function_name = "posix_fault_handler"};

static error_t sigfpe_error = {.message = "Floating point exception (SIGFPE)",
                               .error_code = ERROR_SIGFPE,
                               .file_name = "<signal_handler>",
                               .function_name = "posix_fault_handler"};

static error_t sigill_error = {.message = "Illegal instruction (SIGILL)",
                               .error_code = ERROR_SIGILL,
                               .file_name = "<signal_handler>",
                               .function_name = "posix_fault_handler"};

static error_t sigabrt_error = {.message = "Abnormal termination (SIGABRT)",
                                .error_code = ERROR_SIGABRT,
                                .file_name = "<signal_handler>",
                                .function_name = "posix_fault_handler"};

static error_t sigsys_error = {.message = "Bad system call (SIGSYS)",
                               .error_code = ERROR_SIGSYS,
                               .file_name = "<signal_handler>",
                               .function_name = "posix_fault_handler"};

static error_t sig_default_error = {.message = "Unexpected signal",
                                    .error_code = ERROR_ILLEGAL_STATE,
                                    .file_name = "<signal_handler>",
                                    .function_name = "posix_fault_handler"};
