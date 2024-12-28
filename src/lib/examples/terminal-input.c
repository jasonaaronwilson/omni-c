/**
 * @file terminal-input.c
 *
 * A little interactive test program for input from the terminal.
 */

#include <ctype.h>
#include <stdlib.h>
#include <time.h>

#define C_ARMYKNIFE_LIB_IMPL
#include "../c-armyknife-lib.h"

int main(int argc, char** argv) {
  value_array_t* FLAG_files = NULL;

  flag_program_name(argv[0]);
  flag_description(
      "A little interactive test program for testing terminal input");
  flag_file_args(&FLAG_files);

  char* error = flag_parse_command_line(argc, argv);
  if (error) {
    flag_print_help(stderr, error);
    exit(1);
  }

  if (FLAG_files->length > 0) {
    flag_print_help(stderr, error);
    exit(1);
  }

  struct termios oldt = term_echo_off();

  time_t start_time = time(NULL);
  time_t end_time = start_time + 60; // 30 seconds from start

  buffer_t* buffer = make_buffer(10);
  while (time(NULL) < end_time) {
    buffer_clear(buffer);
    buffer = buffer_read_ready_bytes(buffer, stdin, 10);

    if (buffer_length(buffer) > 0) {
      for (uint64_t i = 0; i < buffer_length(buffer); i++) {
        uint8_t byte = buffer_get(buffer, i);
        fprintf(stdout, "%d ", byte);
      }
      fprintf(stdout, "\n");
      fflush(stdout);
      for (uint64_t i = 0; i < buffer_length(buffer); i++) {
        uint8_t byte = buffer_get(buffer, i);
        if (isprint(byte)) {
          fprintf(stdout, "'%c' ", byte);
        } else {
          fprintf(stdout, "'-' ");
        }
      }

      // This is already seeming like an adhoc tool, so let's have a
      // bit of fun with it. Let's classify just the first byte
      // according to some functions in ctype.

      if (buffer_length(buffer) > 0) {
        uint8_t byte = buffer_get(buffer, 0);
        fprintf(stdout, "\n  isspace=%d", isspace(byte));
        fprintf(stdout, "\n  isalpha=%d", isalpha(byte));
        // TODO(jawilson): etc.
      }

      fprintf(stdout, "\n");
      fprintf(stdout, "\n");
      fflush(stdout);
    }

    usleep(1000);
  }

  // TODO(jawilson): maybe use atexit to make this a little more robust.
  term_echo_restore(oldt);

  exit(0);
}
