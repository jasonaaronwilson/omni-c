#line 2 "debug-printer.c"
#ifndef _DEBUG_PRINTER_H_
#define _DEBUG_PRINTER_H_

#include "lexer.h"
#include "parser.h"
#include <c-armyknife-lib.h>
#include <ctype.h>

/**
 * @file debug-printer.c
 *
 * A node debug "printer" (appending to a buffer).
 *
 * Normally I would use "s-expressions" but since I don't have an
 * s-expression pretty-printer, I thought trying something else might
 * be interesting. (We did something a bit similar with TSNode's when
 * we were using tree-sitter).
 */

__attribute__((warn_unused_result)) buffer_t*
    buffer_append_parse_node(buffer_t* buffer, parse_node_t* node,
                             int indention_level);

#endif /* _DEBUG_PRINTER_H_ */


/* ====================================================================== */
/* "Local" Forward Declarations */
/* ====================================================================== */

__attribute__((warn_unused_result)) static inline buffer_t*
    buffer_indent(buffer_t* buffer, int indention_level) {
  return buffer_append_repeated_byte(buffer, ' ', indention_level * 4);
}

__attribute__((warn_unused_result)) buffer_t*
    buffer_append_declarations(buffer_t* buffer, declarations_t* node,
                               int indention_level);

__attribute__((warn_unused_result)) buffer_t*
    buffer_append_enum(buffer_t* buffer, enum_node_t* node,
                       int indention_level);

__attribute__((warn_unused_result)) buffer_t*
    buffer_append_enum_element(buffer_t* buffer, enum_element_t* node,
                               int indention_level);

__attribute__((warn_unused_result)) buffer_t*
    buffer_append_struct_node(buffer_t* buffer, struct_node_t* node,
                              int indention_level);

/**
 * @function buffer_append_parse_node
 *
 * Append the debugging version of a parse node to a buffer.
 */
__attribute__((warn_unused_result)) buffer_t*
    buffer_append_parse_node(buffer_t* buffer, parse_node_t* node,
                             int indention_level) {
  switch (node->tag) {
  case PARSE_NODE_DECLARATIONS:
    return buffer_append_declarations(buffer, to_declarations(node),
                                      indention_level);

  case PARSE_NODE_ENUM:
    return buffer_append_enum(buffer, to_enum_node(node), indention_level);

  case PARSE_NODE_ENUM_ELEMENT:
    return buffer_append_enum_element(buffer, to_enum_element(node),
                                      indention_level);

  case PARSE_NODE_STRUCT:
    return buffer_append_struct_node(buffer, to_struct_node(node),
                                     indention_level);

  default:
    break;
  }
  fatal_error(ERROR_ILLEGAL_STATE);
}

/**
 * @function buffer_append_node_list
 *
 * Append the debugging version of a list of parse nodes to a buffer.
 */
__attribute__((warn_unused_result)) buffer_t*
    buffer_append_node_list(buffer_t* buffer, node_list_t list,
                            char* field_name, int indention_level) {
  uint64_t length = node_list_length(list);
  for (uint64_t i = 0; i < length; i++) {
    parse_node_t* node = node_list_get(list, i);
    buffer = buffer_indent(buffer, indention_level);
    buffer = buffer_printf(buffer, "%s[%d]:\n", field_name, i & 0xffffffff);
    buffer = buffer_append_parse_node(buffer, node, indention_level + 1);
  }
  return buffer;
}

__attribute__((warn_unused_result)) buffer_t*
    buffer_append_declarations(buffer_t* buffer, declarations_t* node,
                               int indention_level) {
  return buffer_append_node_list(buffer, node->declarations, "declaration",
                                 indention_level);
}

__attribute__((warn_unused_result)) buffer_t*
    buffer_append_enum(buffer_t* buffer, enum_node_t* node,
                       int indention_level) {
  buffer = buffer_indent(buffer, indention_level);
  buffer = buffer_printf(buffer, "tag: PARSE_NODE_ENUM\n");
  buffer = buffer_indent(buffer, indention_level);
  buffer = buffer_printf(buffer, "name: %s\n", token_to_string(*(node->name)));
  buffer = buffer_append_node_list(buffer, node->elements, "element",
                                   indention_level);
  return buffer;
}

__attribute__((warn_unused_result)) buffer_t*
    buffer_append_struct_node(buffer_t* buffer, struct_node_t* node,
                              int indention_level) {
  buffer = buffer_indent(buffer, indention_level);
  buffer = buffer_printf(buffer, "tag: PARSE_NODE_STRUCT\n");
  buffer = buffer_indent(buffer, indention_level);
  buffer = buffer_printf(buffer, "name: %s\n", token_to_string(*(node->name)));
  buffer
      = buffer_append_node_list(buffer, node->fields, "field", indention_level);
  return buffer;
}

__attribute__((warn_unused_result)) buffer_t*
    buffer_append_enum_element(buffer_t* buffer, enum_element_t* node,
                               int indention_level) {
  buffer = buffer_indent(buffer, indention_level);
  buffer = buffer_printf(buffer, "tag: PARSE_NODE_ENUM_ELEMENT\n");

  if (node->name != NULL) {
    buffer = buffer_indent(buffer, indention_level);
    buffer = buffer_append_string(buffer, "name: ");
    buffer = buffer_append_token_string(buffer, *(node->name));
    buffer = buffer_printf(buffer, "\n");
  }

  if (node->value != NULL) {
    buffer = buffer_indent(buffer, indention_level);
    buffer = buffer_append_string(buffer, "value: ");
    buffer = buffer_append_token_string(buffer, *(node->value));
    buffer = buffer_printf(buffer, "\n");
  }

  return buffer;
}
