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
 * nodes in a suitable format for a C "header" file (which means they
 * will be potentially be the same as for a normal non-header file
 * which we will sort out later).
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

#include "header-file-printer.c.generated.h"

#endif /* _HEADER_FILE_PRINTER_H_ */

__attribute__((warn_unused_result)) buffer_t*
    buffer_append_c_function_node_prototype(buffer_t* buffer,
                                            function_node_t* node) {

  for (int i = 0; i < node_list_length(node->attributes); i++) {
    buffer = buffer_append_c_attribute_node(
        buffer, to_attribute_node(node_list_get(node->attributes, i)));
    buffer = buffer_printf(buffer, " ");
  }

  // What happend to static/extern and inline?

  buffer = buffer_append_c_type_node(buffer, node->return_type);
  buffer
      = buffer_printf(buffer, " %s(", token_to_string(*(node->function_name)));

  for (int i = 0; i < node_list_length(node->function_args); i++) {
    if (i > 0) {
      buffer = buffer_printf(buffer, ", ");
    }
    function_argument_node_t* arg_node
        = to_function_argument_node(node_list_get(node->function_args, i));
    buffer = buffer_append_c_function_argument_node(buffer, arg_node);
  }

  buffer = buffer_printf(buffer, ");\n");

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
  buffer = buffer_append_c_type_node(buffer, node->arg_type);
  if (node->arg_name != NULL) {
    buffer = buffer_printf(buffer, " %s", token_to_string(*(node->arg_name)));
  }
  return buffer;
}

__attribute__((warn_unused_result)) buffer_t*
    buffer_append_c_type_node(buffer_t* buffer, type_node_t* node) {

  switch (node->type_node_kind) {
  case TYPE_NODE_KIND_POINTER:
    buffer = buffer_append_c_type_node(
        buffer, to_type_node(node_list_get(node->type_args, 0)));
    buffer = buffer_printf(buffer, "*");
    break;

  case TYPE_NODE_KIND_PRIMITIVE_TYPENAME:
  case TYPE_NODE_KIND_TYPENAME:
    if (node->type_name != NULL) {
      buffer = buffer_append_token_string(buffer, *(node->type_name));
    }
    break;

  default:
    log_fatal("type_node_kind is not expected.");
    fatal_error(ERROR_ILLEGAL_STATE);
    break;
  }

  return buffer;
}

__attribute__((warn_unused_result)) buffer_t*
    buffer_append_c_attribute_node(buffer_t* buffer, attribute_node_t* node) {
  buffer = buffer_printf(buffer, "__attribute__((");

  // Since parser.c doesn't fully parse attributes because we were
  // lazy and don't need to care yet, we simply need to emit a
  // sub-string of the two tokens we were smart enough to retain. We
  // may end up with non-canonicalized whitespace, comments and stuff
  // like that between them, i.e., rather than just putting everything
  // on a single line which we were doing up until now, we are at the
  // mercy of the input more-so than usually permissive parser.
  //
  // We can hopefully reach a demo state where technically we don't
  // need to write function prototypes for our own code anymore and
  // the code is still available as ANSI C when hanlding our own
  // source code for development and boot-strapping.
  buffer = buffer_append_c_raw_token_span(buffer, node->inner_start_token,
                                          node->inner_end_token);


  buffer = buffer_printf(buffer, "))");
  return buffer;
}

//
// Any call to this means that we haven't explicitly "parsed" the
// input yet and are passing a RAW span to the C compiler potentially
// without having completely signed off on it completely (though the
// region will be lexicially consistent with our lexer).
//
__attribute__((warn_unused_result)) buffer_t*
    buffer_append_c_raw_token_span(buffer_t* buffer, oc_token_t* start_token,
                                   oc_token_t* end_token) {
  if (start_token->buffer != end_token->buffer) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  buffer = buffer_append_sub_buffer(buffer, start_token->start, end_token->end,
                                    start_token->buffer);
  return buffer;
}

__attribute__((warn_unused_result)) buffer_t*
    buffer_append_enum_node(buffer_t* buffer, enum_node_t* node) {

  buffer = buffer_printf(buffer, "enum ");
  if (node->name != NULL) {
    buffer = buffer_printf(buffer, "%s\n", token_to_string(*(node->name)));
  }
  buffer = buffer_printf(buffer, "{\n");

  for (int i = 0; i < node_list_length(node->elements); i++) {
    buffer = buffer_printf(buffer, "    ");
    buffer = buffer_append_enum_element(
        buffer, to_enum_element_node(node_list_get(node->elements, i)));
    buffer = buffer_printf(buffer, ",\n");
  }
  buffer = buffer_printf(buffer, "}");

  return buffer;
}

__attribute__((warn_unused_result)) buffer_t*
    buffer_append_enum_element(buffer_t* buffer, enum_element_t* node) {

  buffer = buffer_append_token_string(buffer, *(node->name));
  if (node->value != NULL) {
    buffer = buffer_printf(buffer, " = ");
    buffer = buffer_append_token_string(buffer, *(node->value));
  }

  return buffer;
}


__attribute__((warn_unused_result)) buffer_t*
    buffer_append_enum_to_string(buffer_t* buffer, enum_node_t* node,
                                 char* to_string_fn_prefix, char* type_string) {
  buffer
      = buffer_printf(buffer, "static inline char* %s_to_string(%s value) {\n",
                      to_string_fn_prefix, type_string);
  buffer = buffer_printf(buffer, "  switch (value) {\n");

  for (int i = 0; i < node_list_length(node->elements); i++) {
    enum_element_t* element
        = to_enum_element_node(node_list_get(node->elements, i));
    buffer = buffer_printf(buffer, "    case ");
    buffer = buffer_append_token_string(buffer, *(element->name));
    buffer = buffer_printf(buffer, ":\n      return \"");
    buffer = buffer_append_token_string(buffer, *(element->name));
    buffer = buffer_printf(buffer, "\";\n");
  }
  buffer = buffer_printf(buffer, "    default:\n");
  buffer = buffer_printf(buffer, "      return \"<<unknown-%s>>\";\n",
                         to_string_fn_prefix);

  buffer = buffer_printf(buffer, "  }\n");
  buffer = buffer_printf(buffer, "}\n\n");

  return buffer;
}

__attribute__((warn_unused_result)) buffer_t*
    buffer_append_string_to_enum(buffer_t* buffer, enum_node_t* node,
                                 char* to_string_fn_prefix, char* type_string) {
  buffer = buffer_printf(buffer,
                         "static inline %s string_to_string(char* value) {\n",
                         type_string, to_string_fn_prefix);

  for (int i = 0; i < node_list_length(node->elements); i++) {
    enum_element_t* element
        = to_enum_element_node(node_list_get(node->elements, i));
    buffer = buffer_printf(buffer, "  if (strcmp(value, \"");
    buffer = buffer_append_token_string(buffer, *(element->name));
    buffer = buffer_printf(buffer, "\") == 0) {");
    buffer = buffer_printf(buffer, "\n    return ");
    buffer = buffer_append_token_string(buffer, *(element->name));
    buffer = buffer_printf(buffer, ";\n  }\n");
  }
  buffer = buffer_printf(buffer, "  return 0;\n");
  buffer = buffer_printf(buffer, "}\n\n");

  return buffer;
}
