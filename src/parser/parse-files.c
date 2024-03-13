/**
 * This file contains the entry point to the parser.
 *
 * While this version is currently single-threaded, it should be made
 * multi-threaded (for which a work-queue of some sort seems the most
 * general abstracttion).
 */

#ifndef _PARSE_FILES_H_
#define _PARSE_FILES_H_

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <tree_sitter/api.h>

#include <c-armyknife-lib.h>
#include "../common/oc-file.h"

extern value_array_t* parse_files(value_array_t* files);

#endif /* _PARSE_FILES_H_ */

TSLanguage* tree_sitter_c();

oc_file_t* read_and_parse_file(TSParser* parser, char* file);

value_array_t* parse_files(value_array_t* files) {
  fprintf(stderr, "Parsing %d files...\n", files->length);

  TSParser* parser = ts_parser_new();
  ts_parser_set_language(parser, tree_sitter_c());

  value_array_t* result = make_value_array(files->length);
  for (int i = 0; i < files->length; i++) {
    char* file_name = value_array_get(files, i).str;
    fprintf(stderr, "Parsing %s\n", file_name);
    value_array_add(result, ptr_to_value(read_and_parse_file(parser, file_name)));
  }

  ts_parser_delete(parser);

  return result;
}

oc_file_t* read_and_parse_file(TSParser* parser, char* file_name) {

  buffer_t* buffer = make_buffer(1024);
  buffer = buffer_append_file_contents(buffer, file_name);

  // TODO(jawilson): we should be able to read directrly from out
  // buffer which avoids an entire copy.
  char* str = buffer_to_c_string(buffer);
  int length = buffer->length;
  // free(buffer);

  // Build a syntax tree based on source code stored in a string.
  TSTree* tree = ts_parser_parse_string(parser, NULL, str, length);

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

  /// We might eventually need this.
  /// ts_tree_delete(tree);

  return result;
}
