/**
 * @file sort-lines.c
 *
 * This program reads one or more files specified on the command line
 * and "cats" them together but in sorted order (with duplicate and
 * empty lines removed).
 *
 * This is similar to "sort" on Unix except:
 */

#include <stdlib.h>

#define C_ARMYKNIFE_LIB_IMPL
#include "../c-armyknife-lib.h"

int main(int argc, char** argv) {
  // Flag Parsing
  value_array_t* FLAG_files = NULL;

  flag_program_name(argv[0]);
  flag_description("Similar to 'sort' though duplicate 'lines' are elided.");
  flag_file_args(&FLAG_files);

  char* error = flag_parse_command_line(argc, argv);
  if (error) {
    flag_print_help(stderr, error);
    exit(1);
  }

  // The main program.

  string_tree_t* sorted = NULL;

  for (int i = 0; i < FLAG_files->length; i++) {
    char* file_name = value_array_get(FLAG_files, i).str;
    buffer_t* buffer = make_buffer(1);
    buffer = buffer_append_file_contents(buffer, file_name);
    value_array_t* lines = buffer_tokenize(buffer, "\n");
    for (int j = 0; j < lines->length; j++) {
      char* line = value_array_get(lines, j).str;
      sorted = string_tree_insert(sorted, line, boolean_to_value(true));
    }
  }

  // clang-format off
  string_tree_foreach(sorted, key, value_ignored, {
      fprintf(stdout, "%s\n", key);
    });
  // clang-format on

  exit(0);
}
