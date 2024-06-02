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

__attribute__((warn_unused_result)) buffer_t*
    buffer_append_field_node(buffer_t* buffer, field_node_t* node,
                             int indention_level);

__attribute__((warn_unused_result)) buffer_t*
    buffer_append_type_node(buffer_t* buffer, type_node_t* node,
                            int indention_level);

__attribute__((warn_unused_result)) buffer_t*
    buffer_append_literal_node(buffer_t* buffer, literal_node_t* node,
                               int indention_level);

__attribute__((warn_unused_result)) buffer_t*
    buffer_append_function_node(buffer_t* buffer, function_node_t* node,
                                int indention_level);

__attribute__((warn_unused_result)) buffer_t*
    buffer_append_function_argument_node(buffer_t* buffer,
                                         function_argument_node_t* node,
                                         int indention_level);

__attribute__((warn_unused_result)) buffer_t* buffer_append_function_body_node(
    buffer_t* buffer, function_body_node_t* node, int indention_level);

__attribute__((warn_unused_result)) buffer_t*
    buffer_append_typedef_node(buffer_t* buffer, typedef_node_t* node,
                               int indention_level);

__attribute__((warn_unused_result)) buffer_t*
    buffer_append_global_variable_node(buffer_t* buffer,
                                       global_variable_node_t* node,
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
    return buffer_append_declarations(buffer, to_declarations_node(node),
                                      indention_level);

  case PARSE_NODE_ENUM:
    return buffer_append_enum(buffer, to_enum_node(node), indention_level);

  case PARSE_NODE_ENUM_ELEMENT:
    return buffer_append_enum_element(buffer, to_enum_element_node(node),
                                      indention_level);

  case PARSE_NODE_STRUCT:
    return buffer_append_struct_node(buffer, to_struct_node(node),
                                     indention_level);

  case PARSE_NODE_FIELD:
    return buffer_append_field_node(buffer, to_field_node(node),
                                    indention_level);

  case PARSE_NODE_TYPE:
    return buffer_append_type_node(buffer, to_type_node(node), indention_level);

  case PARSE_NODE_LITERAL:
    return buffer_append_literal_node(buffer, to_literal_node(node),
                                      indention_level);

  case PARSE_NODE_FUNCTION:
    return buffer_append_function_node(buffer, to_function_node(node),
                                       indention_level);

  case PARSE_NODE_FUNCTION_ARGUMENT:
    return buffer_append_function_argument_node(
        buffer, to_function_argument_node(node), indention_level);

  case PARSE_NODE_FUNCTION_BODY:
    return buffer_append_function_body_node(buffer, to_function_body_node(node),
                                            indention_level);

  case PARSE_NODE_TYPEDEF:
    return buffer_append_typedef_node(buffer, to_typedef_node(node),
                                      indention_level);

  case PARSE_NODE_GLOBAL_VARIABLE_DEFINITION:
    return buffer_append_global_variable_node(
        buffer, to_global_variable_node(node), indention_level);

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

/**
 * @function buffer_append_tokens
 *
 * Append the debugging version of a list of value_array of tokens
 * (not as common as node lists).
 */
__attribute__((warn_unused_result)) buffer_t*
    buffer_append_tokens(buffer_t* buffer, value_array_t* tokens,
                         char* field_name, int indention_level) {
  uint64_t length = tokens->length;
  for (uint64_t i = 0; i < length; i++) {
    oc_token_t* token = cast(oc_token_t*, value_array_get(tokens, i).ptr);
    buffer = buffer_indent(buffer, indention_level);
    buffer = buffer_printf(buffer, "%s[%lld]: %s", field_name, i,
                           token_to_string(*token));
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

__attribute__((warn_unused_result)) buffer_t*
    buffer_append_field_node(buffer_t* buffer, field_node_t* node,
                             int indention_level) {
  buffer = buffer_indent(buffer, indention_level);
  buffer = buffer_printf(buffer, "tag: PARSE_NODE_FIELD\n");
  if (node->name != NULL) {
    buffer = buffer_indent(buffer, indention_level);
    buffer = buffer_append_string(buffer, "name: ");
    buffer = buffer_append_token_string(buffer, *(node->name));
    buffer = buffer_printf(buffer, "\n");
  }
  if (node->type != NULL) {
    buffer = buffer_indent(buffer, indention_level);
    buffer = buffer_append_string(buffer, "type:\n");
    buffer = buffer_append_type_node(buffer, node->type, indention_level + 1);
  }

  // TODO(jawilson): bit_field_width

  return buffer;
}

__attribute__((warn_unused_result)) buffer_t*
    buffer_append_type_node(buffer_t* buffer, type_node_t* node,
                            int indention_level) {
  buffer = buffer_indent(buffer, indention_level);
  buffer = buffer_printf(buffer, "tag: PARSE_NODE_TYPE\n");

  buffer = buffer_indent(buffer, indention_level);
  buffer = buffer_printf(buffer, "type_node_kind: %s\n",
                         type_node_kind_to_string(node->type_node_kind));

  if (node->type_name != NULL) {
    buffer = buffer_indent(buffer, indention_level);
    buffer = buffer_append_string(buffer, "type_name: ");
    buffer = buffer_append_token_string(buffer, *(node->type_name));
    buffer = buffer_printf(buffer, "\n");
  }
  buffer = buffer_append_node_list(buffer, node->type_args, "type_arg",
                                   indention_level);
  return buffer;
}

__attribute__((warn_unused_result)) buffer_t*
    buffer_append_literal_node(buffer_t* buffer, literal_node_t* node,
                               int indention_level) {
  buffer = buffer_indent(buffer, indention_level);
  buffer = buffer_printf(buffer, "tag: PARSE_NODE_LITERAL\n");
  buffer = buffer_indent(buffer, indention_level);
  if (node->token != NULL) {
    buffer
        = buffer_printf(buffer, "token: %s\n", token_to_string(*(node->token)));
  }
  if (node->tokens != NULL) {
    buffer
        = buffer_append_tokens(buffer, node->tokens, "tokens", indention_level);
  }
  return buffer;
}

__attribute__((warn_unused_result)) buffer_t*
    buffer_append_function_node(buffer_t* buffer, function_node_t* node,
                                int indention_level) {
  buffer = buffer_indent(buffer, indention_level);
  buffer = buffer_printf(buffer, "tag: PARSE_NODE_FUNCTION\n");
  buffer = buffer_append_node_list(buffer, node->function_args, "function_args",
                                   indention_level);
  if (node->return_type != NULL) {
    buffer = buffer_indent(buffer, indention_level);
    buffer = buffer_append_string(buffer, "return_type:\n");
    buffer = buffer_append_type_node(buffer, node->return_type,
                                     indention_level + 1);
  }

  if (node->body != NULL) {
    buffer
        = buffer_append_function_body_node(buffer, node->body, indention_level);
  }

  return buffer;
}

__attribute__((warn_unused_result)) buffer_t*
    buffer_append_function_argument_node(buffer_t* buffer,
                                         function_argument_node_t* node,
                                         int indention_level) {
  buffer = buffer_indent(buffer, indention_level);
  buffer = buffer_printf(buffer, "tag: PARSE_NODE_FUNCTION\n");
  if (node->arg_type != NULL) {
    buffer = buffer_indent(buffer, indention_level);
    buffer = buffer_append_string(buffer, "arg_type:\n");
    buffer
        = buffer_append_type_node(buffer, node->arg_type, indention_level + 1);
  }
  if (node->arg_name != NULL) {
    buffer = buffer_indent(buffer, indention_level);
    buffer = buffer_printf(buffer, "arg_name: %s\n",
                           token_to_string(*(node->arg_name)));
  }
  return buffer;
}

__attribute__((warn_unused_result)) buffer_t* buffer_append_function_body_node(
    buffer_t* buffer, function_body_node_t* node, int indention_level) {
  buffer = buffer_indent(buffer, indention_level);
  buffer = buffer_printf(buffer, "tag: PARSE_NODE_FUNCTION_BODY\n");
  if (node->open_brace_token != NULL) {
    buffer = buffer_indent(buffer, indention_level);
    buffer = buffer_printf(buffer, "open_brace_token: %s\n",
                           token_to_string(*(node->open_brace_token)));
  }
  if (node->close_brace_token != NULL) {
    buffer = buffer_indent(buffer, indention_level);
    buffer = buffer_printf(buffer, "close_brace_token: %s\n",
                           token_to_string(*(node->close_brace_token)));
  }
  return buffer;
}

__attribute__((warn_unused_result)) buffer_t*
    buffer_append_typedef_node(buffer_t* buffer, typedef_node_t* node,
                               int indention_level) {
  buffer = buffer_indent(buffer, indention_level);
  buffer = buffer_printf(buffer, "tag: PARSE_NODE_TYPEDEF\n");
  if (node->name != NULL) {
    buffer = buffer_indent(buffer, indention_level);
    buffer
        = buffer_printf(buffer, "name: %s\n", token_to_string(*(node->name)));
  }
  if (node->type_node != NULL) {
    buffer = buffer_indent(buffer, indention_level);
    buffer = buffer_append_string(buffer, "type_node:\n");
    buffer
        = buffer_append_type_node(buffer, node->type_node, indention_level + 1);
  }
  return buffer;
}

__attribute__((warn_unused_result)) buffer_t*
    buffer_append_global_variable_node(buffer_t* buffer,
                                       global_variable_node_t* node,
                                       int indention_level) {
  buffer = buffer_indent(buffer, indention_level);
  buffer
      = buffer_printf(buffer, "tag: PARSE_NODE_GLOBAL_VARIABLE_DEFINITION\n");
  if (node->name != NULL) {
    buffer = buffer_indent(buffer, indention_level);
    buffer
        = buffer_printf(buffer, "name: %s\n", token_to_string(*(node->name)));
  }
  if (node->type != NULL) {
    buffer = buffer_indent(buffer, indention_level);
    buffer = buffer_append_string(buffer, "type:\n");
    buffer = buffer_append_type_node(buffer, node->type, indention_level + 1);
  }
  return buffer;
}
