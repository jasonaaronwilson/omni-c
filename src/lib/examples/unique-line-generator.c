/**
 * @file unique-line-generator.c
 *
 * This program outputs lines containing (mostly) unique
 * numbers. Using a simple for loop these lines would appear in the
 * expected order from smallest to largest which is rather predictable
 * and thus not very useful as a testing input for testing certain
 * algorithms.
 *
 * While many solutions exist, we will begin with just using a PRNG
 * (psuedo-random number generator) to generate N numbers (modulo a
 * maximum number) and assume they are unique. For short sequences and
 * very large maximums, this is probably true but longer sequences
 * with small maximums will accidentally generate more duplicate
 * lines. This isn't a problem with the PRNG since we would expect the
 * same from a truly random number generator.
 */

#include <stdlib.h>

#define C_ARMYKNIFE_LIB_IMPL
#include "../c-armyknife-lib.h"

int main(int argc, char** argv) {
  configure_fatal_errors((fatal_error_config_t){
      .catch_sigsegv = true,
  });

  // Flag Parsing

  uint64_t FLAG_number_of_lines = 1024;
  uint64_t FLAG_max_number = 0xffffffff;
  value_array_t* FLAG_files = NULL;

  flag_program_name(argv[0]);
  flag_description("Generate (highly probable) unique lines.");

  flag_uint64("--number-of-lines", &FLAG_number_of_lines);
  flag_description("Number of lines to generate");

  flag_uint64("--max-number", &FLAG_max_number);
  flag_description("The maximum number to generate");

  flag_file_args(&FLAG_files);

  char* error = flag_parse_command_line(argc, argv);
  if (error) {
    flag_print_help(stderr, error);
    exit(1);
  }

  fprintf(stdout, "# number-of-lines=%d max-number=%d\n",
          FLAG_number_of_lines & 0xfffffff, FLAG_max_number & 0xfffffff);

  random_state_t state = random_state_for_test();

  for (int i = 0; i < FLAG_number_of_lines; i++) {
    uint64_t random_n = random_next_uint64_below(&state, FLAG_max_number);
    fprintf(stdout, "%d\n", random_n & 0xfffffff);
  }

  exit(0);
}
