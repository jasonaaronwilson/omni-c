#line 2 "debug-printer.c"
#ifndef _DEBUG_PRINTER_H_
#define _DEBUG_PRINTER_H_

#include <c-armyknife-lib.h>
#include <ctype.h>

#include "declaration-parser.h"
#include "lexer.h"
#include "parser.h"
#include "pratt-parser.h"
#include "statement-parser.h"
#include "type-parser.h"
#include "user-type-parser.h"
#include "variable-definition-parser.h"

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

#include "debug-printer.c.generated.h"

static inline buffer_t* buffer_indent(buffer_t* buffer, int indention_level) {
  return buffer_append_repeated_byte(buffer, ' ', indention_level * 4);
}

#endif /* _DEBUG_PRINTER_H_ */

/**
 * @function buffer_append_dbg_parse_node
 *
 * Append the debugging version of a parse node to a buffer.
 */
buffer_t* buffer_append_dbg_parse_node(buffer_t* buffer, parse_node_t* node,
                                       int indention_level) {
  switch (node->tag) {
  case PARSE_NODE_DECLARATIONS:
    return buffer_append_dbg_declarations(buffer, to_declarations_node(node),
                                          indention_level);

  case PARSE_NODE_ENUM:
    return buffer_append_dbg_enum(buffer, to_enum_node(node), indention_level);

  case PARSE_NODE_ENUM_ELEMENT:
    return buffer_append_dbg_enum_element(buffer, to_enum_element_node(node),
                                          indention_level);

  case PARSE_NODE_STRUCT:
    return buffer_append_dbg_struct_node(buffer, to_struct_node(node),
                                         indention_level);

  case PARSE_NODE_FIELD:
    return buffer_append_dbg_field_node(buffer, to_field_node(node),
                                        indention_level);

  case PARSE_NODE_TYPE:
    return buffer_append_dbg_type_node(buffer, to_type_node(node),
                                       indention_level);

  case PARSE_NODE_LITERAL:
    return buffer_append_dbg_literal_node(buffer, to_literal_node(node),
                                          indention_level);

  case PARSE_NODE_FUNCTION:
    return buffer_append_dbg_function_node(buffer, to_function_node(node),
                                           indention_level);

  case PARSE_NODE_FUNCTION_ARGUMENT:
    return buffer_append_dbg_function_argument_node(
        buffer, to_function_argument_node(node), indention_level);

  case PARSE_NODE_BALANCED_CONSTRUCT:
    return buffer_append_dbg_balanced_construct_node(
        buffer, to_balanced_construct_node(node), indention_level);

  case PARSE_NODE_TYPEDEF:
    return buffer_append_dbg_typedef_node(buffer, to_typedef_node(node),
                                          indention_level);

  case PARSE_NODE_VARIABLE_DEFINITION:
    return buffer_append_dbg_variable_definition_node(
        buffer, to_variable_definition_node(node), indention_level);

  case PARSE_NODE_ATTRIBUTE:
    return buffer_append_dbg_attribute_node(buffer, to_attribute_node(node),
                                            indention_level);

  case PARSE_NODE_IDENTIFIER:
    return buffer_append_dbg_identifier_node(buffer, to_identifier_node(node),
                                             indention_level);

  case PARSE_NODE_OPERATOR:
    return buffer_append_dbg_operator_node(buffer, to_operator_node(node),
                                           indention_level);

  case PARSE_NODE_BREAK_STATEMENT:
    return buffer_append_dbg_break_statement_node(
        buffer, to_break_statement_node(node), indention_level);

  default:
    break;
  }
  fatal_error(ERROR_ILLEGAL_STATE);
}

/**
 * @function buffer_append_dbg_node_list
 *
 * Append the debugging version of a list of parse nodes to a buffer.
 */
buffer_t* buffer_append_dbg_node_list(buffer_t* buffer, node_list_t list,
                                      char* field_name, int indention_level) {
  uint64_t length = node_list_length(list);
  for (uint64_t i = 0; i < length; i++) {
    parse_node_t* node = node_list_get(list, i);
    buffer = buffer_indent(buffer, indention_level);
    buffer = buffer_printf(buffer, "%s[%d]:\n", field_name, i & 0xffffffff);
    buffer = buffer_append_dbg_parse_node(buffer, node, indention_level + 1);
  }
  return buffer;
}

/**
 * @function buffer_append_dbg_tokens
 *
 * Append the debugging version of a list of value_array of tokens
 * (not as common as node lists).
 */
buffer_t* buffer_append_dbg_tokens(buffer_t* buffer, value_array_t* tokens,
                                   char* field_name, int indention_level) {
  uint64_t length = tokens->length;
  for (uint64_t i = 0; i < length; i++) {
    token_t* token = value_array_get_ptr(tokens, i, token_t*);
    buffer = buffer_indent(buffer, indention_level);
    buffer = buffer_printf(buffer, "%s[%lld]: %s\n", field_name, i,
                           token_to_string(token));
  }
  return buffer;
}


buffer_t* buffer_append_dbg_declarations(buffer_t* buffer,
                                         declarations_node_t* node,
                                         int indention_level) {
  return buffer_append_dbg_node_list(buffer, node->declarations, "declaration",
                                     indention_level);
}

buffer_t* buffer_append_dbg_enum(buffer_t* buffer, enum_node_t* node,
                                 int indention_level) {
  buffer_indent(buffer, indention_level);
  buffer_printf(buffer, "tag: PARSE_NODE_ENUM\n");
  if (node->name != NULL) {
    buffer_indent(buffer, indention_level);
    buffer_printf(buffer, "name: %s\n", token_to_string(node->name));
  }
  buffer_append_dbg_node_list(buffer, node->elements, "element",
                              indention_level);
  if (node->partial_definition) {
    buffer_indent(buffer, indention_level);
    buffer_printf(buffer, "partial_definition: true\n");
  }
  return buffer;
}

buffer_t* buffer_append_dbg_struct_node(buffer_t* buffer, struct_node_t* node,
                                        int indention_level) {
  buffer = buffer_indent(buffer, indention_level);
  buffer = buffer_printf(buffer, "tag: PARSE_NODE_STRUCT\n");
  if (node->name != NULL) {
    buffer = buffer_indent(buffer, indention_level);
    buffer = buffer_printf(buffer, "name: %s\n", token_to_string(node->name));
  }
  buffer = buffer_indent(buffer, indention_level);
  buffer = buffer_printf(buffer, "partial_definition: %s\n",
                         node->partial_definition ? "true" : "false");
  buffer = buffer_append_dbg_node_list(buffer, node->fields, "field",
                                       indention_level);
  return buffer;
}

buffer_t* buffer_append_dbg_enum_element(buffer_t* buffer, enum_element_t* node,
                                         int indention_level) {
  buffer = buffer_indent(buffer, indention_level);
  buffer = buffer_printf(buffer, "tag: PARSE_NODE_ENUM_ELEMENT\n");

  if (node->name != NULL) {
    buffer = buffer_indent(buffer, indention_level);
    buffer = buffer_append_string(buffer, "name: ");
    buffer = buffer_append_token_string(buffer, node->name);
    buffer = buffer_printf(buffer, "\n");
  }

  if (node->value != NULL) {
    buffer = buffer_indent(buffer, indention_level);
    buffer = buffer_append_string(buffer, "value: ");
    buffer = buffer_append_token_string(buffer, node->value);
    buffer = buffer_printf(buffer, "\n");
  }

  return buffer;
}

buffer_t* buffer_append_dbg_field_node(buffer_t* buffer, field_node_t* node,
                                       int indention_level) {
  buffer = buffer_indent(buffer, indention_level);
  buffer = buffer_printf(buffer, "tag: PARSE_NODE_FIELD\n");
  if (node->name != NULL) {
    buffer = buffer_indent(buffer, indention_level);
    buffer = buffer_append_string(buffer, "name: ");
    buffer = buffer_append_token_string(buffer, node->name);
    buffer = buffer_printf(buffer, "\n");
  }
  if (node->type != NULL) {
    buffer = buffer_indent(buffer, indention_level);
    buffer = buffer_append_string(buffer, "type:\n");
    buffer
        = buffer_append_dbg_type_node(buffer, node->type, indention_level + 1);
  }

  // TODO(jawilson): bit_field_width

  return buffer;
}

buffer_t* buffer_append_dbg_type_node(buffer_t* buffer, type_node_t* node,
                                      int indention_level) {
  buffer = buffer_indent(buffer, indention_level);
  buffer = buffer_printf(buffer, "tag: PARSE_NODE_TYPE\n");

  buffer = buffer_indent(buffer, indention_level);
  buffer = buffer_printf(buffer, "type_node_kind: %s\n",
                         type_node_kind_to_string(node->type_node_kind));

  if (node->type_name != NULL) {
    buffer = buffer_indent(buffer, indention_level);
    buffer = buffer_append_string(buffer, "type_name: ");
    buffer = buffer_append_token_string(buffer, node->type_name);
    buffer = buffer_printf(buffer, "\n");
  }
  if (node->user_type != NULL) {
    buffer = buffer_indent(buffer, indention_level);
    buffer = buffer_append_string(buffer, "user_type:\n");
    buffer = buffer_append_dbg_parse_node(buffer, node->user_type,
                                          indention_level + 1);
  }
  buffer = buffer_append_dbg_node_list(buffer, node->type_args, "type_arg",
                                       indention_level);
  return buffer;
}

buffer_t* buffer_append_dbg_literal_node(buffer_t* buffer, literal_node_t* node,
                                         int indention_level) {
  buffer = buffer_indent(buffer, indention_level);
  buffer = buffer_printf(buffer, "tag: PARSE_NODE_LITERAL\n");
  if (node->token != NULL) {
    buffer = buffer_indent(buffer, indention_level);
    buffer = buffer_printf(buffer, "token: %s\n", token_to_string(node->token));
  }
  if (node->tokens != NULL) {
    buffer = buffer_append_dbg_tokens(buffer, node->tokens, "tokens",
                                      indention_level);
  }
  return buffer;
}

buffer_t* buffer_append_dbg_function_node(buffer_t* buffer,
                                          function_node_t* node,
                                          int indention_level) {
  buffer = buffer_indent(buffer, indention_level);
  buffer = buffer_printf(buffer, "tag: PARSE_NODE_FUNCTION\n");

  buffer = buffer_append_dbg_node_list(buffer, node->attributes, "attribute",
                                       indention_level);

  if (node->storage_class_specifier != NULL) {
    buffer = buffer_indent(buffer, indention_level);
    buffer = buffer_printf(buffer, "storage_class_specifier: %s\n",
                           token_to_string(node->storage_class_specifier));
  }

  // FIXME! use function_specifiers
  /*
  if (node->function_specifier != NULL) {
    buffer = buffer_indent(buffer, indention_level);
    buffer = buffer_printf(buffer, "function_specifier: %s\n",
                           token_to_string(node->function_specifier));
  }
  */

  if (node->return_type != NULL) {
    buffer = buffer_indent(buffer, indention_level);
    buffer = buffer_append_string(buffer, "return_type:\n");
    buffer = buffer_append_dbg_type_node(buffer, node->return_type,
                                         indention_level + 1);
  }

  if (node->function_name != NULL) {
    buffer = buffer_indent(buffer, indention_level);
    buffer = buffer_printf(buffer, "function_name: %s\n",
                           token_to_string(node->function_name));
  }

  buffer = buffer_append_dbg_node_list(buffer, node->function_args,
                                       "function_args", indention_level);
  if (node->body != NULL) {
    buffer = buffer_append_dbg_parse_node(buffer, node->body, indention_level);
  }

  return buffer;
}

buffer_t* buffer_append_dbg_function_argument_node(
    buffer_t* buffer, function_argument_node_t* node, int indention_level) {
  buffer = buffer_indent(buffer, indention_level);
  buffer = buffer_printf(buffer, "tag: PARSE_NODE_FUNCTION\n");
  if (node->arg_type != NULL) {
    buffer = buffer_indent(buffer, indention_level);
    buffer = buffer_append_string(buffer, "arg_type:\n");
    buffer = buffer_append_dbg_type_node(buffer, node->arg_type,
                                         indention_level + 1);
  }
  if (node->arg_name != NULL) {
    buffer = buffer_indent(buffer, indention_level);
    buffer = buffer_printf(buffer, "arg_name: %s\n",
                           token_to_string(node->arg_name));
  }
  return buffer;
}

buffer_t* buffer_append_dbg_balanced_construct_node(
    buffer_t* buffer, balanced_construct_node_t* node, int indention_level) {
  buffer = buffer_indent(buffer, indention_level);
  buffer = buffer_printf(buffer, "tag: PARSE_NODE_BALANCED_CONSTRUCT\n");
  if (node->start_token != NULL) {
    buffer = buffer_indent(buffer, indention_level);
    buffer = buffer_printf(buffer, "start_token: %s\n",
                           token_to_string(node->start_token));
  }
  if (node->end_token != NULL) {
    buffer = buffer_indent(buffer, indention_level);
    buffer = buffer_printf(buffer, "end_token: %s\n",
                           token_to_string(node->end_token));
  }
  return buffer;
}

buffer_t* buffer_append_dbg_typedef_node(buffer_t* buffer, typedef_node_t* node,
                                         int indention_level) {
  buffer = buffer_indent(buffer, indention_level);
  buffer = buffer_printf(buffer, "tag: PARSE_NODE_TYPEDEF\n");
  if (node->name != NULL) {
    buffer = buffer_indent(buffer, indention_level);
    buffer = buffer_printf(buffer, "name: %s\n", token_to_string(node->name));
  }
  if (node->type_node != NULL) {
    buffer = buffer_indent(buffer, indention_level);
    buffer = buffer_append_string(buffer, "type_node:\n");
    buffer = buffer_append_dbg_type_node(buffer, node->type_node,
                                         indention_level + 1);
  }
  return buffer;
}

buffer_t* buffer_append_dbg_variable_definition_node(
    buffer_t* buffer, variable_definition_node_t* node, int indention_level) {
  buffer = buffer_indent(buffer, indention_level);
  buffer = buffer_printf(buffer, "tag: PARSE_NODE_VARIABLE_DEFINITION\n");
  if (node->name != NULL) {
    buffer = buffer_indent(buffer, indention_level);
    buffer = buffer_printf(buffer, "name: %s\n", token_to_string(node->name));
  }
  if (node->type != NULL) {
    buffer = buffer_indent(buffer, indention_level);
    buffer = buffer_append_string(buffer, "type:\n");
    buffer
        = buffer_append_dbg_type_node(buffer, node->type, indention_level + 1);
  }
  if (node->value != NULL) {
    buffer = buffer_indent(buffer, indention_level);
    buffer = buffer_append_string(buffer, "value:\n");
    buffer = buffer_append_dbg_parse_node(buffer, node->value,
                                          indention_level + 1);
  }
  if (node->storage_class_specifier != NULL) {
    buffer = buffer_indent(buffer, indention_level);
    buffer = buffer_printf(buffer, "storage_class_specifier: %s\n",
                           token_to_string(node->storage_class_specifier));
  }

  return buffer;
}

buffer_t* buffer_append_dbg_attribute_node(buffer_t* buffer,
                                           attribute_node_t* node,
                                           int indention_level) {
  buffer = buffer_indent(buffer, indention_level);
  buffer = buffer_printf(buffer, "tag: PARSE_NODE_ATTRIBUTE\n");
  if (node->inner_start_token != NULL) {
    buffer = buffer_indent(buffer, indention_level);
    buffer = buffer_printf(buffer, "inner_start_token: %s\n",
                           token_to_string(node->inner_start_token));
  }
  if (node->inner_end_token != NULL) {
    buffer = buffer_indent(buffer, indention_level);
    buffer = buffer_printf(buffer, "inner_end_token: %s\n",
                           token_to_string(node->inner_end_token));
  }
  return buffer;
}

/* ====================================================================== */

/**
 * @function debug_append_tokens
 *
 * This function simply appends the contents of all tokens
 * together. This is useful for debugging source to source
 * transformations based on modiying the token stream before before
 * (like replacing nullptr with NULL).
 */
void debug_append_tokens(buffer_t* buffer, value_array_t* tokens) {
  for (int i = 0; i < tokens->length; i++) {
    token_t* token = token_at(tokens, i);
    buffer_append_sub_buffer(buffer, token->start, token->end, token->buffer);
  }
}

buffer_t* buffer_append_dbg_identifier_node(buffer_t* buffer,
                                            identifier_node_t* node,
                                            int indention_level) {
  buffer_indent(buffer, indention_level);
  buffer_printf(buffer, "tag: PARSE_NODE_TYPEDEF\n");
  buffer_indent(buffer, indention_level);
  buffer_printf(buffer, "name: %s\n", token_to_string(node->token));
  return buffer;
}

buffer_t* buffer_append_dbg_operator_node(buffer_t* buffer,
                                          operator_node_t* node,
                                          int indention_level) {
  buffer_indent(buffer, indention_level);
  buffer_printf(buffer, "tag: PARSE_NODE_OPERATOR\n");
  buffer_indent(buffer, indention_level);
  buffer_printf(buffer, "operator: %s\n", token_to_string(node->operator));

  if (node->left != NULL) {
    buffer_indent(buffer, indention_level);
    buffer_append_string(buffer, "left:\n");
    buffer_append_dbg_parse_node(buffer, node->left, indention_level + 1);
  }

  if (node->right != NULL) {
    buffer_indent(buffer, indention_level);
    buffer_append_string(buffer, "right:\n");
    buffer_append_dbg_parse_node(buffer, node->right, indention_level + 1);
  }

  return buffer;
}

buffer_t* buffer_append_dbg_break_statement_node(buffer_t* buffer,
                                                 break_statement_node_t* node,
                                                 int indention_level) {
  buffer_indent(buffer, indention_level);
  buffer_printf(buffer, "tag: PARSE_NODE_BREAK_STATEMENT\n");
  buffer_indent(buffer, indention_level);
  if (node->break_keyword_token != NULL) {
    buffer_printf(buffer, "break_keyword_token: %s",
                  token_to_string(node->break_keyword_token));
  }
  return buffer;
}
