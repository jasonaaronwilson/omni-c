/**
 * @file code-points.c
 *
 * This program reads "stdin" assuming a UTF-8 encoding and outputs
 * each unicode code-point as 4 hexideciaml digits followed by a
 * newline. This seems useful overall but this implementation is
 * fatally flawed because it currently reads all the input before any
 * ouput is emitted (or even checking is done) which means it uses way
 * more RAM than necessary and won't deliver any ouput until all the
 * input has been read. In the future we should handle streaming UTF-8
 * though for now this serves as a useful test for our UTF-8 decoding
 * skills.
 */

#include <stdlib.h>

#define C_ARMYKNIFE_LIB_IMPL
#include "../c-armyknife-lib.h"

int main(int argc, char** argv) {
  value_array_t* FLAG_files = NULL;

  flag_program_name(argv[0]);
  flag_description("Reads utf8 encoded files and outputs the code-points");
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

  // The main program.
  buffer_t* buffer = make_buffer(1);
  buffer = buffer_append_all(buffer, stdin);
  uint8_t* str = (uint8_t*) buffer_to_c_string(buffer);
  int offset = 0;
  while (offset < buffer_length(buffer)) {
    utf8_decode_result_t result = utf8_decode(&str[offset]);
    if (result.error) {
      log_fatal("An error was encountered at file byte offset %d", offset);
      fatal_error(ERROR_ILLEGAL_INPUT);
    }
    fprintf(stdout, "%04x\n", result.code_point);
    offset += result.num_bytes;
  }

  exit(0);
}
