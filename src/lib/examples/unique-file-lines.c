/**
 * @file unique-file-lines.c
 *
 * This program reads one or more files specified on the command line
 * and "cats" them together but with duplicate (and empty) lines
 * removed (in some ways very similar to the histogram program we will
 * eventually provide).
 *
 * This is similar to using cat + sort + uniq on Unix except:
 *
 * 1. The input remains in the orginal order
 *
 * 2. While I'm not sure how much memory *sort* uses on your flavor of
 * unix, uniq only has to keep two lines in memory at once, since it
 * only compares adjacent lines, so it's unfair to only compare us to
 * "uniq" without also comparing us to "sort". For this program,
 * "unique-file-lines", you can assume we use at least as much memory
 * as would be needed to keep all of the input files in
 * memory. (Future versions might have flags like --adaptive and
 * --bloom-filter-size=XXXX).
 *
 * Besides potentially being useful, this tool stresses a bunch of
 * parts of c-arymknife-lib. Since this program can easily be written
 * in other languages, it provides some potential for benchmarking.
 *
 * As alluded to aboe, while not currently implemented, probabilistic
 * approaches could be much more space efficient (and potentially more
 * performant). For example, with a decent quality 64bit hash, every
 * "seen" line could be represented in about 8 bytes and would thus
 * have a different "set" abstraction than what our library provides
 *
 * (The problem is that with some small probability, a non duplicate
 * line will be determined to be a duplicate.)
 */

#include <stdlib.h>

#define C_ARMYKNIFE_LIB_IMPL
#include "../c-armyknife-lib.h"

// result is a hashtable of strings -> true
string_hashtable_t* initial_seen_hashtable() {
  string_hashtable_t* seen = make_string_hashtable(128 * 1024);
  return seen;
}

void process_lines(buffer_t* buffer, string_hashtable_t** seen,
                   string_tree_t** seen_tree, boolean_t use_tree) {
  value_array_t* lines = buffer_tokenize(buffer, "\n");
  for (int j = 0; j < lines->length; j++) {
    char* line = value_array_get(lines, j).str;
    if (use_tree) {
      value_result_t find_result = string_tree_find(*seen_tree, line);
      if (!is_ok(find_result)) {
        *seen_tree
            = string_tree_insert(*seen_tree, line, boolean_to_value(true));
        fprintf(stdout, "%s\n", line);
      }
    } else {
      value_result_t find_result = string_ht_find(*seen, line);
      if (!is_ok(find_result)) {
        *seen = string_ht_insert(*seen, line, boolean_to_value(true));
        fprintf(stdout, "%s\n", line);
      }
    }
  }
}

int main(int argc, char** argv) {
  // Flag Parsing

  boolean_t FLAG_use_tree = false;
  value_array_t* FLAG_files = NULL;

  flag_program_name(argv[0]);
  flag_description("Similar to 'cat' but duplicate 'lines' are elided.");
  // TODO(jawilson): add --limit-uniqueness-to-each-file
  flag_boolean("--use-tree", &FLAG_use_tree);
  flag_description("When true, use a tree instead of a hashtable");
  flag_file_args(&FLAG_files);

  char* error = flag_parse_command_line(argc, argv);

  if (error) {
    flag_print_help(stderr, error);
    exit(1);
  }

  string_hashtable_t* seen = initial_seen_hashtable();
  string_tree_t* seen_tree = NULL;

  // This is about the worst way we could write this. So?
  buffer_t* buffer = make_buffer(1);
  for (int i = 0; i < FLAG_files->length; i++) {
    char* file_name = value_array_get(FLAG_files, i).str;
    buffer = buffer_append_file_contents(buffer, file_name);
    process_lines(buffer, &seen, &seen_tree, FLAG_use_tree);
    buffer_clear(buffer);
  }
  if (FLAG_files->length == 0) {
    buffer = buffer_append_all(buffer, stdin);
    process_lines(buffer, &seen, &seen_tree, FLAG_use_tree);
  }

  exit(0);
}
