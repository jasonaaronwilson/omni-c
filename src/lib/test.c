#line 2 "test.c"
/**
 * @file test.c
 *
 * A set of macros and inline functions to help write tests for the
 * armyknife library (or your own tests).
 */

// ======================================================================
// This section is extraced to test.h
// ======================================================================

#ifndef _TEST_H_
#define _TEST_H_

/**
 * @macro test_fail
 *
 * Immediately fail a test after printing a message (supplied the same
 * way as printf, log_fatal, etc.)
 *
 * This is a macro instead of a function because it automatically
 * passes the current file and line number to test_fail_and_exit which
 * enables uniform and convenient error messagages according to GNU
 * guidelines (so they will look like gcc compiler errors if your IDE
 * understands those).
 *
 * It is *advised* (at least for now) to not pass complicated
 * expressions to `test_fail` if those are likely to fail. Instead run
 * the test in a debugger and set a break-point on
 * `test_fail_and_exit`.
 */
#define test_fail(format, ...)                                                 \
  do {                                                                         \
    test_fail_and_exit(__FILE__, __LINE__, format, ##__VA_ARGS__);             \
  } while (0)


/**
 * @macro test_assert
 *
 * Evaluate the condition and fail the test if the result is false.
 */
#define test_assert(condition)                                                 \
  do {                                                                         \
    if (!(condition))                                                          \
      test_fail("A test assertion failed. Condition expression was: %s",       \
                #condition);                                                   \
  } while (0)

/**
 * @macro test_assert_uint64_equal
 *
 * Assert that two values are the same by casting both of them to
 * uint64_t and seeing if they are "==".
 *
 * Except that we lose the "location" information, this would be
 * better as an inline function rather than a macro.
 */
#define test_assert_integer_equal(a, b)                                        \
  do {                                                                         \
    unsigned long long casted_a = (unsigned long long) (a);                    \
    unsigned long long casted_b = (unsigned long long) (b);                    \
    if (a != b) {                                                              \
      test_fail(                                                               \
          "An integer comparision failed\n  Expected:\n    ⟦%llu⟧\n  "     \
          "But was:\n    ⟦%llu⟧\n",                                            \
          casted_a, casted_b);                                                 \
    }                                                                          \
  } while (0)

/**
 * @macro test_assert_string_equal
 *
 * Assert that two c strings are the same.
 */
#define test_assert_string_equal(a, b)                                          \
  do {                                                                          \
    if (!b) {                                                                   \
      test_fail(                                                                \
          "A test string equal assertion failed\n  Expected:\n    ⟦%s⟧\n  " \
          "But was:\n    nullptr\n",                                            \
          a);                                                                   \
    }                                                                           \
    if (!string_equal(a, b)) {                                                  \
      test_fail(                                                                \
          "A test string equal assertion failed\n  Expected:\n    ⟦%s⟧\n  " \
          "But was:\n    ⟦%s⟧\n",                                               \
          a, b);                                                                \
    }                                                                           \
  } while (0)

static inline void open_arena_for_test(void) {
  /*
#ifdef C_ARMYKNIFE_LIB_USE_ARENAS
  arena_open(4096 * 256);
#endif
  */
}

static inline void close_arena_for_test(void) {
  /*
#ifdef C_ARMYKNIFE_LIB_USE_ARENAS
  arena_close();
#endif
  */
}

#endif /* _TEST_H_ */

/**
 * @function test_fail_and_exit
 *
 * Set a break-point here to debug a test but normally you will use
 * the macro `test_fail` since it is much more convenient.
 */
__attribute__((format(printf, 3, 4))) void
    test_fail_and_exit(char* file_name, int line_number, char* format, ...) {
  va_list args;
  fprintf(stdout, "%s:%d: ", file_name, line_number);
  va_start(args, format);
  vfprintf(stdout, format, args);
  fprintf(stdout, "\n");
  va_end(args);
  exit(1);
}
