/**
 * @file line-histogram.c
 *
 * This program reads one or more files specified on the command line
 * and counts the number of occurrences of each line (except empty
 * ones).
 */

#include <stdlib.h>

#define C_ARMYKNIFE_LIB_IMPL
#include "../c-armyknife-lib.h"

// result is a hashtable of string -> uint64_t
string_hashtable_t* initial_counts_hashtable() {
  // The hashtable will resize as necessary but we still want a
  // reasonable initial size.
  string_hashtable_t* counts = make_string_hashtable(4096);
  return counts;
}

int main(int argc, char** argv) {
  // Flag Parsing

  boolean_t FLAG_use_tree = false;
  value_array_t* FLAG_files = NULL;

  flag_program_name(argv[0]);
  flag_description("Creates a histogram of lines and their counts");
  flag_boolean("--use-tree", &FLAG_use_tree);
  flag_description("When true, use a tree instead of a hashtable");
  flag_file_args(&FLAG_files);

  char* error = flag_parse_command_line(argc, argv);
  if (error) {
    flag_print_help(stderr, error);
    exit(1);
  }

  // The main program.

  boolean_t use_tree = FLAG_use_tree;
  value_array_t* files = FLAG_files;

  string_hashtable_t* counts = initial_counts_hashtable();
  string_tree_t* counts_tree = NULL;

  for (int i = 0; i < files->length; i++) {
    char* file_name = value_array_get(files, i).str;
    buffer_t* buffer = make_buffer(1);
    buffer = buffer_append_file_contents(buffer, file_name);
    value_array_t* lines = buffer_tokenize(buffer, "\n");
    for (int j = 0; j < lines->length; j++) {
      char* line = value_array_get(lines, j).str;
      if (use_tree) {
        value_result_t find_result = string_tree_find(counts_tree, line);
        if (is_ok(find_result)) {
          counts_tree = string_tree_insert(counts_tree, line,
                                           u64_to_value(find_result.u64 + 1));
        } else {
          counts_tree = string_tree_insert(counts_tree, line, u64_to_value(1));
        }
      } else {
        value_result_t find_result = string_ht_find(counts, line);
        if (is_ok(find_result)) {
          counts = string_ht_insert(counts, line,
                                    u64_to_value(find_result.u64 + 1));
        } else {
          counts = string_ht_insert(counts, line, u64_to_value(1));
        }
      }
    }
  }

  if (use_tree) {
    // clang-format off
    string_tree_foreach(counts_tree, key, value, {
	fprintf(stdout, "%d:%s\n", value.u64 & 0xfffffff, key);
      });
    // clang-format on
  } else {
    // clang-format off
    string_ht_foreach(counts, key, value, {
	fprintf(stdout, "%d:%s\n", value.u64 & 0xfffffff, key);
      });
    // clang-format on
  }

  exit(0);
}
