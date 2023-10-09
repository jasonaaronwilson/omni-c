#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <tree_sitter/api.h>

#define C_ARMYKNIFE_LIB_IMPL
#include "../../../c-armyknife-lib/c-armyknife-lib.h"

TSLanguage *tree_sitter_json();

void process_file(char* file) {
  // Create a parser.
  TSParser *parser = ts_parser_new();

  // Set the parser's language (JSON in this case).
  ts_parser_set_language(parser, tree_sitter_c());

  buffer_t* buffer = make_buffer(1024);
  buffer = buffer_append_file_contents(buffer, file);
  char* str = buffer_to_c_string(buffer);

  // Build a syntax tree based on source code stored in a string.
  TSTree *tree = ts_parser_parse_string(
    parser,
    NULL,
    str,
    buffer->length
  );

  // Get the root node of the syntax tree.
  TSNode root_node = ts_tree_root_node(tree);

  // Print the syntax tree as an S-expression.
  char *string = ts_node_string(root_node);
  printf("%s\n", string);

  // Free all of the heap-allocated memory.
  free(buffer);
  free(str);
  free(string);
  ts_tree_delete(tree);
  ts_parser_delete(parser);
}

int main(int argc, char**argv) {
  command_line_parse_result_t args_and_files = parse_command_line(argc, argv);

  for (int i = 0; i < args_and_files.files->length; i++) {
    reference_t file = array_get_reference(args_and_files.files, i);
    process_file(dereference_char_ptr(file));
  }

  return 0;
}
