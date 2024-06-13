#line 2 "header-file-printer.c"
#ifndef _HEADER_FILE_PRINTER_H_
#define _HEADER_FILE_PRINTER_H_

#include "lexer.h"
#include "parser.h"
#include <c-armyknife-lib.h>
#include <ctype.h>

/**
 * @file header-file-printer.c
 *
 * This file provides primitives to "print" out certain kinds of parse
 * nodes in a suitable format for a C "header" file.
 *
 * This will eventually support one of our immediate goals which is
 * that no one should ever have to write a header file themselves even
 * if they could care less about any advanced feature of omni-c
 * (namely, overloaded functions, C++ style templates, automatic code
 * generation for things like enum->string and string->enum), let
 * alone the reasons I started this project.
 *
 * The primitives are not smart enough to make decisions about the
 * order things should appear in the header file, etc.
 */

__attribute__((warn_unused_result)) buffer_t*
    buffer_append_c_function_node_prototype(buffer_t* buffer,
                                            function_node_t* node);

__attribute__((warn_unused_result)) buffer_t*
    buffer_append_c_function_argument_node(buffer_t* buffer,
                                           function_argument_node_t* node);

__attribute__((warn_unused_result)) buffer_t*
    buffer_append_c_type_node(buffer_t* buffer, type_node_t* node);

#endif /* _HEADER_FILE_PRINTER_H_ */

__attribute__((warn_unused_result)) buffer_t*
    buffer_append_c_function_node_prototype(buffer_t* buffer,
                                            function_node_t* node) {
  buffer = buffer_append_c_type_node(buffer, node->return_type);
  buffer = buffer_printf(buffer, "%s();\n", token_to_string(*(node->function_name)));
  /*
  buffer = buffer_indent(buffer, indention_level);
  buffer = buffer_printf(buffer, "tag: PARSE_NODE_FUNCTION\n");
  buffer = buffer_append_dbg_node_list(buffer, node->function_args,
  "function_args", indention_level); if (node->return_type != NULL) { buffer =
  buffer_indent(buffer, indention_level); buffer = buffer_append_string(buffer,
  "return_type:\n"); buffer = buffer_append_dbg_type_node(buffer,
  node->return_type, indention_level + 1);
  }
  if (node->storage_class_specifier != NULL) {
    buffer = buffer_indent(buffer, indention_level);
    buffer = buffer_printf(buffer, "storage_class_specifier: %s\n",
                           token_to_string(*(node->storage_class_specifier)));
  }
  if (node->body != NULL) {
    buffer
        = buffer_append_dbg_function_body_node(buffer, node->body,
  indention_level);
  }
  */

  return buffer;
}

__attribute__((warn_unused_result)) buffer_t*
    buffer_append_c_function_argument_node(buffer_t* buffer,
                                           function_argument_node_t* node) {
  /*
  buffer = buffer_indent(buffer, indention_level);
  buffer = buffer_printf(buffer, "tag: PARSE_NODE_FUNCTION\n");
  if (node->arg_type != NULL) {
    buffer = buffer_indent(buffer, indention_level);
    buffer = buffer_append_string(buffer, "arg_type:\n");
    buffer
        = buffer_append_dbg_type_node(buffer, node->arg_type, indention_level +
  1);
  }
  if (node->arg_name != NULL) {
    buffer = buffer_indent(buffer, indention_level);
    buffer = buffer_printf(buffer, "arg_name: %s\n",
                           token_to_string(*(node->arg_name)));
  }
  */
  return buffer;
}

__attribute__((warn_unused_result)) buffer_t*
    buffer_append_c_type_node(buffer_t* buffer, type_node_t* node) {
  // Nowhere near complete.
  if (node->type_name != NULL) {
    buffer = buffer_append_token_string(buffer, *(node->type_name));
    buffer = buffer_printf(buffer, " ");
  }
  return buffer;
}
