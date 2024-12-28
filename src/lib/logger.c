#line 2 "logger.c"

/**
 * @file logger.c
 *
 * A "logger" is a type of code instrumentation and provides the
 * ability to put explicit "print statements" into your code without
 * necessarily having a large impact on the performance of that code
 * as long as the logger is turned off for that "level" of detail.
 *
 * If you've temporarily inserted print statements into your program,
 * you've probably already learned that logging is very useful and
 * also somewhat expensive in terms of run-time performance. [^1]
 *
 * Since logging is expensive, it is useful to be able to turn it on
 * and off (even without recompiling) which is done based on the
 * logging level. When logging is turned off, the cost is meant to be
 * equivalent to a load, compare, and branch though logging can effect
 * the C compiler's optimizations, so it is **not** recommended to be
 * left in critical loops. Obviously if the code is compiled into the
 * binary, even if the code to skip the logging doesn't considerably
 * increase run-time performance, it may still have an impact for
 * example on the output binary size.
 *
 * The default log level is "WARN" though it is possible to override
 * this with #define ARMYKNIFE_LIB_DEFAULT_LOG_LEVEL <level> when
 * including the library implementation (or from your build command
 * which allows C preprocessor definitions to be injected into your
 * source code, one reason you may want a debug vs production builds).
 *
 * Additionally, when the first log statement is encountered, we
 * examine the environment variable named ARMYKNIFE_LIB_LOG_LEVEL if
 * you want to adjust the level after compilation. Future versions
 * will certainly provide more control such as turn on logging only
 * for specific files as well as giving the C compiler enough
 * information to remove some logging code completely from the
 * binary).
 *
 * There are a set number of levels and they are defined like so:
 *
 * OFF = 0
 * TRACE = 1
 * DEBUG = 2
 * INFO = 3
 * WARN = 4
 * FATAL = 5
 * TEST = 6
 *
 * The most overlooked part of logging may be that putting PII or
 * other information into logs may violate GDPR and other privacy laws
 * depending on how the logs are processed and retained. Our
 * recommendation is to never intentionally log PII. It's especially
 * important to keep this in mind if you are developing an internet
 * application that the user isn't running on their own machine which
 * isn't an initial focus of this library.
 *
 * [^1]: For this implementation, getting a timestamp is probably one
 * kernel call and doing the actual output, since logging is less
 * useful when buffered, requires at least another kernel
 * call. Finally, formatting strings for human readability is
 * relatively expensive itself. For example, printing a large number
 * may require dozens or hundreds of cycles while adding two numbers
 * may take less than a single cycle on a modern pipelined processor).
 */

#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <stdarg.h>
#include <stdio.h>

#define LOGGER_OFF 0
#define LOGGER_TRACE 1
#define LOGGER_DEBUG 2
#define LOGGER_INFO 3
#define LOGGER_WARN 4
#define LOGGER_FATAL 5
#define LOGGER_TEST 6

struct logger_state_S {
  boolean_t initialized;
  int level;
  char* logger_output_filename;
  FILE* output;
};

typedef struct logger_state_S logger_state_t;

#ifndef LOGGER_DEFAULT_LEVEL
#define LOGGER_DEFAULT_LEVEL LOGGER_WARN
#endif /* LOGGER_DEFAULT_LEVEL */

extern logger_state_t global_logger_state;

extern void logger_init(void);

__attribute__((format(printf, 5, 6))) extern void
    logger_impl(char* file, int line_number, const char* function, int level,
                char* format, ...);

/**
 * @macro log_none
 *
 * This will never ever log and should have essentially zero impact on
 * compilation (including detecting errors). In other words it should
 * behave like an empty statment ";".
 *
 * On the other hand, errors in these statements will not be caught
 * and therefore it can't be depended on to keep working as you
 * refactor code and decide later that you want to turn it on.
 */
#define log_none(format, ...)                                                  \
  do {                                                                         \
  } while (0);

/**
 * @macro log_off
 *
 * This will never log however the compiler *should* still check to
 * make sure the code is legal and compiles. Any sufficiently smart
 * compiler with some level of optimization turned on should not
 * change it's code generation strategy at all if you leave one of these
 * statements in your source code and you should easily be able to
 * upgrade them to a real level later.
 */
#define log_off(format, ...)                                                   \
  do {                                                                         \
    if (0) {                                                                   \
      logger_impl(__FILE__, __LINE__, __FUNCTION__, LOGGER_TRACE, format,      \
                  ##__VA_ARGS__);                                              \
    }                                                                          \
  } while (0)

/**
 * @macro log_trace
 *
 * Log at the TRACE level using printf style formatting.
 */
#define log_trace(format, ...)                                                 \
  do {                                                                         \
    if (global_logger_state.level <= LOGGER_TRACE) {                           \
      logger_impl(__FILE__, __LINE__, __FUNCTION__, LOGGER_TRACE, format,      \
                  ##__VA_ARGS__);                                              \
    }                                                                          \
  } while (0)

/**
 * @macro log_debug
 *
 * Log at the DEBUG level using printf style formatting.
 */
#define log_debug(format, ...)                                                 \
  do {                                                                         \
    if (global_logger_state.level <= LOGGER_DEBUG) {                           \
      logger_impl(__FILE__, __LINE__, __FUNCTION__, LOGGER_DEBUG, format,      \
                  ##__VA_ARGS__);                                              \
    }                                                                          \
  } while (0)

/**
 * @macro log_info
 *
 * Log at the INFO level using printf style formatting.
 */
#define log_info(format, ...)                                                  \
  do {                                                                         \
    if (global_logger_state.level <= LOGGER_INFO) {                            \
      logger_impl(__FILE__, __LINE__, __FUNCTION__, LOGGER_INFO, format,       \
                  ##__VA_ARGS__);                                              \
    }                                                                          \
  } while (0)

/**
 * Determine if logging at the INFO level is enabled.
 */
static inline boolean_t should_log_info() {
  return global_logger_state.level <= LOGGER_INFO;
}

/**
 * @macro log_warn
 *
 * Log at the WARN level using printf style formatting.
 */
#define log_warn(format, ...)                                                  \
  do {                                                                         \
    if (global_logger_state.level <= LOGGER_WARN) {                            \
      logger_impl(__FILE__, __LINE__, __FUNCTION__, LOGGER_WARN, format,       \
                  ##__VA_ARGS__);                                              \
    }                                                                          \
  } while (0)

/**
 * @macro log_fatal
 *
 * Log at the FATAL level using printf style formatting.
 *
 * Typically this is only done before invoking fatal_error though I
 * don't have a convenient way to enforce this.
 */
#define log_fatal(format, ...)                                                 \
  do {                                                                         \
    if (global_logger_state.level <= LOGGER_FATAL) {                           \
      logger_impl(__FILE__, __LINE__, __FUNCTION__, LOGGER_FATAL, format,      \
                  ##__VA_ARGS__);                                              \
    }                                                                          \
  } while (0)

/**
 * @macro log_test
 *
 * Log at the TEST level using printf style formatting. This should
 * only be used inside of test code to communicate very basic
 * information back to the user when running a test and is therefore
 * independent of the actual log level.
 */
#define log_test(format, ...)                                                  \
  do {                                                                         \
    logger_impl(__FILE__, __LINE__, __FUNCTION__, LOGGER_TEST, format,         \
                ##__VA_ARGS__);                                                \
  } while (0)

#endif /* _LOGGER_H_ */

logger_state_t global_logger_state
    = compound_literal(logger_state_t, {.level = LOGGER_DEFAULT_LEVEL});

value_result_t parse_log_level_enum(char* str) {
  if (strcmp("FATAL", str) == 0 || strcmp("fatal", str) == 0) {
    return compound_literal(value_result_t, {.u64 = LOGGER_FATAL});
  } else if (strcmp("WARN", str) == 0 || strcmp("warn", str) == 0) {
    return compound_literal(value_result_t, {.u64 = LOGGER_WARN});
  } else if (strcmp("INFO", str) == 0 || strcmp("info", str) == 0) {
    return compound_literal(value_result_t, {.u64 = LOGGER_INFO});
  } else if (strcmp("DEBUG", str) == 0 || strcmp("debug", str) == 0) {
    return compound_literal(value_result_t, {.u64 = LOGGER_DEBUG});
  } else if (strcmp("TRACE", str) == 0 || strcmp("trace", str) == 0) {
    return compound_literal(value_result_t, {.u64 = LOGGER_TRACE});
  } else if (strcmp("OFF", str) == 0 || strcmp("off", str) == 0) {
    return compound_literal(value_result_t, {.u64 = LOGGER_OFF});
  } else {
    return compound_literal(value_result_t,
                            {.nf_error = NF_ERROR_NOT_PARSED_AS_EXPECTED_ENUM});
  }
}

// FORWARD DECLARATION
char* logger_level_to_string(int level);

/**
 * @function logger_init
 *
 * This function modifies the logging level based on the environment
 * variable ARMYKNIFE_LIB_LOG_LEVEL (which currently must be a
 * number).
 *
 * While not required to actually use logging, the logging level will
 * be set to LOGGER_WARN unless you change it in a debugger, and those
 * logging statements will be sent to stderr which is probably not
 * convenient.
 */
void logger_init(void) {
  char* level_string = getenv("ARMYKNIFE_LIB_LOG_LEVEL");
  if (level_string != NULL) {
    value_result_t parsed = string_parse_uint64(level_string);
    if (is_ok(parsed)) {
      global_logger_state.level = parsed.u64;
    } else {
      value_result_t parsed = parse_log_level_enum(level_string);
      if (is_ok(parsed)) {
        global_logger_state.level = parsed.u64;
      } else {
        log_warn("%s could not be converted to a log level.", level_string);
      }
    }
  }

  fprintf(stderr, "Log level is set to %s (%d)\n",
          logger_level_to_string(global_logger_state.level),
          global_logger_state.level);

  char* output_file_name = getenv("ARMYKNIFE_LIB_LOG_FILE");

  // It's pretty standard to include the "pid" in the filename at
  // least when writing to /tmp/. We aren't quite doing that yet...
  //
  // pid_t pid = getpid(); -- pid is a number of some sort...

  if (output_file_name != NULL) {
    global_logger_state.output = fopen(output_file_name, "w");
    if (!global_logger_state.output) {
      fatal_error(ERROR_OPEN_LOG_FILE);
    }
    // Set the stream to unbuffered
    // if (setvbuf(log_file, NULL, _IONBF, 0) != 0) {
    // perror("Failed to set stream to unbuffered");
    // exit(EXIT_FAILURE);
    // }
    global_logger_state.logger_output_filename = output_file_name;
  } else {
    global_logger_state.output = stderr;
    global_logger_state.initialized = true;
  }
}

// Convert the level to a human readable string (which will also
// appear as the name in the log file).
char* logger_level_to_string(int level) {
  switch (level) {
  case LOGGER_OFF:
    return "LOGGER_OFF";
  case LOGGER_TRACE:
    return "TRACE";
  case LOGGER_DEBUG:
    return "DEBUG";
  case LOGGER_INFO:
    return "INFO";
  case LOGGER_WARN:
    return "WARN";
  case LOGGER_FATAL:
    return "FATAL";
  default:
    return "LEVEL_UNKNOWN";
  }
}

/**
 * @function logger_impl
 *
 * This is the non macro version entry point into the logger. Normally
 * it wouldn't be called directly since it is less convenient than the
 * macro versions.
 */
__attribute__((format(printf, 5, 6))) void
    logger_impl(char* file, int line_number, const char* function, int level,
                char* format, ...) {

  FILE* output = global_logger_state.output;

  // Ensure that logging to somewhere will happen though a later call
  // to logger_init() may send the output to somewhere else.
  if (output == NULL) {
    output = stderr;
  }

  if (level >= global_logger_state.level) {
    fprintf(output, "%s ", logger_level_to_string(level));
    va_list args;
    fprintf(output, "%s:%d %s | ", file, line_number, function);

    va_start(args, format);
    vfprintf(output, format, args);
    va_end(args);

    fprintf(output, "\n");
  }
}
