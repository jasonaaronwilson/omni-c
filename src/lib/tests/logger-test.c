//
// Tests the logger facility.
//

#include <stdlib.h>

#define C_ARMYKNIFE_LIB_IMPL
#include "../c-armyknife-lib.h"

void test_logger() {
  // You probably wouldn't use this form yourself but the macros are a
  // bit tricky to get right...
  logger_impl(__FILE__, __LINE__, "FOO", LOGGER_TRACE, "Hello World!");

  // clang-format off
  //
  // This correctly produced a warning: I'm too lazy to actually make this
  // a unit test right now:
  //
  // logger_impl(__FILE__, __LINE__, LOGGER_TRACE, "Hello World: %s", 0xdead);
  //
  //
  // logger-test.c:15:68: warning: format specifies type 'char *' but the argument has type
  //      'int' [-Wformat]
  // logger_impl(__FILE__, __LINE__, LOGGER_TRACE, "Hello World: %s", 0xdead);
  //                                                              ~~   ^~~~~~
  //                                                             %d
  // clang-format on

  // Do the easy case first.
  log_trace("Hello World!");

  // Now make sure all of the standard levels work.
  log_trace("Hello World str is '%s', number is %d!", "a string", 12345);
  log_debug("Hello World str is '%s', number is %d!", "a string", 12345);
  log_info("Hello World str is '%s', number is %d!", "a string", 12345);
  log_warn("Hello World str is '%s', number is %d!", "a string", 12345);
  log_fatal("Hello World str is '%s', number is %d!", "a string", 12345);
}

int main(int argc, char** argv) {
  logger_init();
  test_logger();
  exit(0);
}
