#line 2 "fatal-error.c"
/**
 * @file fatal-error.c
 *
 * The intent is that everything but a normal program exit will end up
 * here. (To catch SIGSIGV errors you may call
 * configure_fatal_errors() first with catch_sigsegv set.)
 *
 * Note that you can use fatal_error's to your advantage by setting
 * the environment variable ARMYKNIFE_FATAL_ERROR_SLEEP_SECONDS to
 * some value to give yourself enough time to attach a debugger.
 *
 * In this case C's macros are paying off as the file and line number
 * are easy to obtain.
 *
 * TODO(jawilson): environment variable to be quieter...
 */

// ======================================================================
// This is block is extraced to fatal-error.h
// ======================================================================

#ifndef _FATAL_ERROR_H_
#define _FATAL_ERROR_H_

struct fatal_error_config_S {
  boolean_t catch_sigsegv;
};

typedef struct fatal_error_config_S fatal_error_config_t;

/**
 * @constants error_code_t
 */
typedef enum {
  ERROR_UKNOWN,
  ERROR_SIGSEGV,
  ERROR_ACCESS_OUT_OF_BOUNDS,
  ERROR_BAD_COMMAND_LINE,
  ERROR_DYNAMICALLY_SIZED_TYPE_ILLEGAL_IN_CONTAINER,
  ERROR_ILLEGAL_ENUM_VALUE,
  ERROR_ILLEGAL_INITIAL_CAPACITY,
  ERROR_ILLEGAL_NULL_ARGUMENT,
  ERROR_ILLEGAL_ZERO_HASHCODE_VALUE,
  ERROR_ILLEGAL_RANGE,
  ERROR_MEMORY_ALLOCATION,
  ERROR_MEMORY_FREE_NULL,
  ERROR_NOT_REACHED,
  ERROR_REFERENCE_NOT_EXPECTED_TYPE,
  ERROR_UNIMPLEMENTED,
  ERROR_OPEN_LOG_FILE,
  ERROR_TEST,
  ERROR_INTERNAL_ASSERTION_FAILURE,
  ERROR_BAD_ALLOCATION_SIZE,
  ERROR_ILLEGAL_ARGUMENT,
  ERROR_MEMORY_START_PADDING_ERROR,
  ERROR_MEMORY_END_PADDING_ERROR,
  ERROR_FATAL,
  ERROR_ILLEGAL_STATE,
  ERROR_ILLEGAL_INPUT,
  ERROR_ILLEGAL_UTF_8_CODE_POINT,
  ERROR_ILLEGAL_TERMINAL_COORDINATES,
} error_code_t;

extern _Noreturn void fatal_error_impl(char* file, int line, int error_code);
extern const char* fatal_error_code_to_string(int error_code);
extern void configure_fatal_errors(fatal_error_config_t config);

/**
 * @macro fatal_error
 *
 * Terminates the program with a fatal error.
 */
#define fatal_error(code) fatal_error_impl(__FILE__, __LINE__, code)

#endif /* _FATAL_ERROR_H_ */

// ======================================================================

#include <execinfo.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

fatal_error_config_t fatal_error_config = {0};

void segmentation_fault_handler(int signal_number) {
  fatal_error(ERROR_SIGSEGV);
}

void configure_fatal_errors(fatal_error_config_t config) {
  fatal_error_config = config;
  if (config.catch_sigsegv) {
    signal(SIGSEGV, segmentation_fault_handler);
  }
}

void print_fatal_error_banner();
void print_backtrace();
void print_error_code_name(int error_code);

char* get_command_line() {
  buffer_t* buffer
      = buffer_append_file_contents(make_buffer(1), "/proc/self/cmdline");
  buffer_replace_matching_byte(buffer, 0, ' ');
  return buffer_to_c_string(buffer);
}

char* get_program_path() {
  char buf[4096];
  int n = readlink("/proc/self/exe", buf, sizeof(buf));
  if (n > 0) {
    return string_duplicate(buf);
  } else {
    return "<program-path-unknown>";
  }
}

_Noreturn void fatal_error_impl(char* file, int line, int error_code) {
  print_fatal_error_banner();
  print_backtrace();
  fprintf(stderr, "%s:%d: FATAL ERROR %d", file, line, error_code);
  print_error_code_name(error_code);
  fprintf(stderr, "\nCommand line: %s\n\n", get_command_line());
  char* sleep_str = getenv("ARMYKNIFE_FATAL_ERROR_SLEEP_SECONDS");
  if (sleep_str != NULL) {
    value_result_t sleep_time = string_parse_uint64(sleep_str);
    if (is_ok(sleep_time)) {
      fprintf(stderr,
              "Sleeping for %lu seconds so you can attach a debugger.\n",
              sleep_time.u64);
      fprintf(stderr, "  gdb -tui %s %d\n", get_program_path(), getpid());
      sleep(sleep_time.u64);
    }
  } else {
    fprintf(stderr, "(ARMYKNIFE_FATAL_ERROR_SLEEP_SECONDS is not set)\n");
  }
  fprintf(stderr, "Necessaria Morte Mori...\n");
  exit(-(error_code + 100));
}

void print_fatal_error_banner() {
  // As the first thing we print, also responsible for at least one
  // newline to start a new line if we may not be at one.
  fprintf(stderr, "\n========== FATAL_ERROR ==========\n");
}

void print_backtrace() {
#ifndef NO_VM_BACKTRACE_ON_FATAL_ERROR
  do {
    void* array[10];
    int size = backtrace(array, 10);
    char** strings = backtrace_symbols(array, size);

    // Print the backtrace
    for (int i = 0; i < size; i++) {
      printf("#%d %s\n", i, strings[i]);
    }
  } while (0);
#endif /* NO_VM_BACKTRACE_ON_FATAL_ERROR */
}

const char* fatal_error_code_to_string(int error_code) {
  switch (error_code) {
  case ERROR_UKNOWN:
    return "ERROR_UKNOWN";
  case ERROR_MEMORY_ALLOCATION:
    return "ERROR_MEMORY_ALLOCATION";
  case ERROR_MEMORY_FREE_NULL:
    return "ERROR_MEMORY_FREE_NULL";
  case ERROR_REFERENCE_NOT_EXPECTED_TYPE:
    return "ERROR_REFERENCE_NOT_EXPECTED_TYPE";
  case ERROR_ILLEGAL_INITIAL_CAPACITY:
    return "ERROR_ILLEGAL_INITIAL_CAPACITY";
  case ERROR_DYNAMICALLY_SIZED_TYPE_ILLEGAL_IN_CONTAINER:
    return "ERROR_DYNAMICALLY_SIZED_TYPE_ILLEGAL_IN_CONTAINER";
  case ERROR_ACCESS_OUT_OF_BOUNDS:
    return "ERROR_ACCESS_OUT_OF_BOUNDS";
  case ERROR_NOT_REACHED:
    return "ERROR_NOT_REACHED";
  case ERROR_ILLEGAL_ZERO_HASHCODE_VALUE:
    return "ERROR_ILLEGAL_ZERO_HASHCODE_VALUE";
  case ERROR_UNIMPLEMENTED:
    return "ERROR_UNIMPLEMENTED";
  case ERROR_ILLEGAL_NULL_ARGUMENT:
    return "ERROR_ILLEGAL_NULL_ARGUMENT";
  case ERROR_ILLEGAL_ARGUMENT:
    return "ERROR_ILLEGAL_ARGUMENT";
  case ERROR_MEMORY_START_PADDING_ERROR:
    return "ERROR_MEMORY_START_PADDING_ERROR";
  case ERROR_MEMORY_END_PADDING_ERROR:
    return "ERROR_MEMORY_END_PADDING_ERROR";

  default:
    return "error";
  }
}

void print_error_code_name(int error_code) {
  fprintf(stderr, " ");
  fprintf(stderr, "*** ");
  fprintf(stderr, "%s", fatal_error_code_to_string(error_code));
  fprintf(stderr, " ***\n");
}
