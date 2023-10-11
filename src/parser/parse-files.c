#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <tree_sitter/api.h>

#include "../../../c-armyknife-lib/c-armyknife-lib.h"
#include "../common/oc-file.h"

TSLanguage *tree_sitter_c();

oc_file_t* read_and_parse_file(TSParser *parser, char* file);

ptr_array_t* parse_files(ptr_array_t* files) {
  TSParser *parser = ts_parser_new();
  ts_parser_set_language(parser, tree_sitter_c());

  ptr_array_t* result = make_ptr_array(files->length);
  for (int i = 0; i < files->length; i++) {
    ptr_array_add(result, read_and_parse_file(parser, ptr_array_get(files, i)));
  }

  ts_parser_delete(parser);
  
  return result;
}

oc_file_t* read_and_parse_file(TSParser *parser, char* file_name) {

  buffer_t* buffer = make_buffer(1024);
  buffer = buffer_append_file_contents(buffer, file_name);
  char* str = buffer_to_c_string(buffer);

  // Build a syntax tree based on source code stored in a string.
  TSTree *tree = ts_parser_parse_string(
    parser,
    NULL,
    str,
    buffer->length
  );

  free(str);

  /// // Get the root node of the syntax tree.
  /// TSNode root_node = ts_tree_root_node(tree);
  ///
  /// // Print the syntax tree as an S-expression.
  /// char *string = ts_node_string(root_node);
  /// printf("%s\n", string);

  oc_file_t* result = malloc_struct(oc_file_t);
  result->tag = STD_C_SOURCE_FILE;
  result->file_name = file_name;
  result->data = buffer;
  result->tree = tree;

  /// // Free all of the heap-allocated memory.
  /// free(buffer);
  /// ts_tree_delete(tree);

  return result;
}

/*

int main(int argc, char**argv) {
  command_line_parse_result_t args_and_files = parse_command_line(argc, argv);

  for (int i = 0; i < args_and_files.files->length; i++) {
    reference_t file = array_get_reference(args_and_files.files, i);
    process_file(dereference_char_ptr(file));
  }

  return 0;
}

*/
