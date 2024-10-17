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

#endif /* _DEBUG_PRINTER_H_ */

/**
 * @function buffer_append_dbg_parse_node
 *
 * Append the debugging version of a parse node to a buffer.
 */
void buffer_append_dbg_parse_node(cdl_printer_t* printer, parse_node_t* node) {
  switch (node->tag) {
  case PARSE_NODE_DECLARATIONS:
    buffer_append_dbg_declarations(printer, to_declarations_node(node));
    break;

  case PARSE_NODE_ENUM:
    buffer_append_dbg_enum(printer, to_enum_node(node));
    break;

  case PARSE_NODE_ENUM_ELEMENT:
    buffer_append_dbg_enum_element(printer, to_enum_element_node(node));
    break;

  case PARSE_NODE_STRUCT:
    buffer_append_dbg_struct_node(printer, to_struct_node(node));
    break;

  case PARSE_NODE_FIELD:
    buffer_append_dbg_field_node(printer, to_field_node(node));
    break;

  case PARSE_NODE_TYPE:
    buffer_append_dbg_type_node(printer, to_type_node(node));
    break;
    ;

  case PARSE_NODE_LITERAL:
    buffer_append_dbg_literal_node(printer, to_literal_node(node));
    break;

  case PARSE_NODE_FUNCTION:
    buffer_append_dbg_function_node(printer, to_function_node(node));
    break;

  case PARSE_NODE_FUNCTION_ARGUMENT:
    buffer_append_dbg_function_argument_node(printer,
                                             to_function_argument_node(node));
    break;

  case PARSE_NODE_BALANCED_CONSTRUCT:
    buffer_append_dbg_balanced_construct_node(printer,
                                              to_balanced_construct_node(node));
    break;

  case PARSE_NODE_TYPEDEF:
    buffer_append_dbg_typedef_node(printer, to_typedef_node(node));
    break;

  case PARSE_NODE_VARIABLE_DEFINITION:
    buffer_append_dbg_variable_definition_node(
        printer, to_variable_definition_node(node));
    break;

  case PARSE_NODE_ATTRIBUTE:
    buffer_append_dbg_attribute_node(printer, to_attribute_node(node));
    break;

  case PARSE_NODE_IDENTIFIER:
    buffer_append_dbg_identifier_node(printer, to_identifier_node(node));
    break;

  case PARSE_NODE_OPERATOR:
    buffer_append_dbg_operator_node(printer, to_operator_node(node));
    break;

  case PARSE_NODE_BREAK_STATEMENT:
    buffer_append_dbg_break_statement_node(printer,
                                           to_break_statement_node(node));
    break;

  case PARSE_NODE_CONTINUE_STATEMENT:
    buffer_append_dbg_continue_statement_node(printer,
                                              to_continue_statement_node(node));
    break;

  case PARSE_NODE_LABEL_STATEMENT:
    buffer_append_dbg_label_statement_node(printer,
                                           to_label_statement_node(node));
    break;

  case PARSE_NODE_CASE_LABEL:
    buffer_append_dbg_case_label_node(printer, to_case_label_node(node));
    break;

  case PARSE_NODE_DEFAULT_LABEL:
    buffer_append_dbg_default_label_node(printer, to_default_label_node(node));
    break;

  case PARSE_NODE_CALL:
    buffer_append_dbg_call_node(printer, to_call_node(node));
    break;

  case PARSE_NODE_BLOCK:
    buffer_append_dbg_block_node(printer, to_block_node(node));
    break;

  case PARSE_NODE_WHILE_STATEMENT:
    buffer_append_dbg_while_node(printer, to_while_statement_node(node));
    break;

  case PARSE_NODE_FOR_STATEMENT:
    buffer_append_dbg_for_node(printer, to_for_statement_node(node));
    break;

  case PARSE_NODE_DO_STATEMENT:
    buffer_append_dbg_do_node(printer, to_do_statement_node(node));
    break;

  case PARSE_NODE_IF_STATEMENT:
    buffer_append_dbg_if_node(printer, to_if_statement_node(node));
    break;

  case PARSE_NODE_EMPTY_STATEMENT:
    buffer_append_dbg_empty_statement_node(printer,
                                           to_empty_statement_node(node));
    break;

  case PARSE_NODE_RETURN_STATEMENT:
    buffer_append_dbg_return_statement_node(printer,
                                            to_return_statement_node(node));
    break;

  case PARSE_NODE_EXPRESSION_STATEMENT:
    buffer_append_dbg_expression_statement_node(
        printer, to_expression_statement_node(node));
    break;

  case PARSE_NODE_SWITCH_STATEMENT:
    buffer_append_dbg_switch_node(printer, to_switch_statement_node(node));
    break;

  default:
    log_fatal("No debug printer for %s", parse_node_type_to_string(node->tag));
    fatal_error(ERROR_ILLEGAL_STATE);
    break;
  }
}

/**
 * @function buffer_append_dbg_node_list
 *
 * Append the debugging version of a list of parse nodes to a buffer.
 */
void buffer_append_dbg_node_list(cdl_printer_t* printer, node_list_t list) {
  cdl_start_array(printer);
  uint64_t length = node_list_length(list);
  for (uint64_t i = 0; i < length; i++) {
    buffer_append_dbg_parse_node(printer, node_list_get(list, i));
  }
  cdl_end_array(printer);
}

/**
 * @function buffer_append_dbg_tokens
 *
 * Append the debugging version of a list of value_array of tokens
 * (not as common as node lists).
 */
void buffer_append_dbg_tokens(cdl_printer_t* printer, value_array_t* tokens,
                              char* field_name) {
  cdl_key(printer, field_name);
  cdl_start_array(printer);
  uint64_t length = tokens->length;
  for (uint64_t i = 0; i < length; i++) {
    token_t* token = value_array_get_ptr(tokens, i, typeof(token_t*));
    cdl_string(printer, token_to_string(token));
  }
  cdl_end_array(printer);
}


void buffer_append_dbg_declarations(cdl_printer_t* printer,
                                    declarations_node_t* node) {
  buffer_append_dbg_node_list(printer, node->declarations);
}

void buffer_append_dbg_enum(cdl_printer_t* printer, enum_node_t* node) {
  cdl_start_table(printer);
  cdl_key(printer, "tag");
  cdl_string(printer, "PARSE_NODE_ENUM");
  if (node->name != NULL) {
    cdl_key(printer, "name");
    cdl_string(printer, token_to_string(node->name));
  }
  cdl_key(printer, "elements");
  buffer_append_dbg_node_list(printer, node->elements);
  cdl_key(printer, "partial_definition");
  cdl_boolean(printer, node->partial_definition);
  cdl_end_table(printer);
}

void buffer_append_dbg_struct_node(cdl_printer_t* printer,
                                   struct_node_t* node) {
  cdl_start_table(printer);
  cdl_key(printer, "tag");
  cdl_string(printer, "PARSE_NODE_STRUCT");
  if (node->name != NULL) {
    cdl_key(printer, "name");
    cdl_string(printer, token_to_string(node->name));
  }
  cdl_key(printer, "partial_definition");
  cdl_boolean(printer, node->partial_definition);
  cdl_key(printer, "fields");
  buffer_append_dbg_node_list(printer, node->fields);
  cdl_end_table(printer);
}

void buffer_append_dbg_enum_element(cdl_printer_t* printer,
                                    enum_element_t* node) {
  cdl_start_table(printer);
  cdl_key(printer, "tag");
  cdl_string(printer, "PARSE_NODE_ENUM_ELEMENT");
  if (node->name != NULL) {
    cdl_key(printer, "name");
    cdl_string(printer, token_to_string(node->name));
  }
  if (node->value != NULL) {
    cdl_key(printer, "value");
    cdl_string(printer, token_to_string(node->value));
  }
  cdl_end_table(printer);
}

void buffer_append_dbg_field_node(cdl_printer_t* printer, field_node_t* node) {
  cdl_start_table(printer);
  cdl_key(printer, "tag");
  cdl_string(printer, "PARSE_NODE_FIELD");
  if (node->name != NULL) {
    cdl_key(printer, "name");
    cdl_string(printer, token_to_string(node->name));
  }
  if (node->type != NULL) {
    cdl_key(printer, "type");
    buffer_append_dbg_type_node(printer, node->type);
  }
  // TODO(jawilson): bit_field_width
  cdl_end_table(printer);
}

void buffer_append_dbg_type_node(cdl_printer_t* printer, type_node_t* node) {
  cdl_start_table(printer);
  cdl_key(printer, "tag");
  cdl_string(printer, "PARSE_NODE_TYPE");
  cdl_key(printer, "type_node_kind");
  cdl_string(printer, type_node_kind_to_string(node->type_node_kind));
  if (node->type_name != NULL) {
    cdl_key(printer, "type_name");
    cdl_string(printer, token_to_string(node->type_name));
  }
  if (node->user_type != NULL) {
    cdl_key(printer, "user_type");
    buffer_append_dbg_parse_node(printer, node->user_type);
  }
  cdl_key(printer, "type_args");
  buffer_append_dbg_node_list(printer, node->type_args);
  cdl_end_table(printer);
}

void buffer_append_dbg_literal_node(cdl_printer_t* printer,
                                    literal_node_t* node) {
  cdl_start_table(printer);
  cdl_key(printer, "tag");
  cdl_string(printer, "PARSE_NODE_LITERAL");
  if (node->token != NULL) {
    cdl_key(printer, "token");
    cdl_string(printer, token_to_string(node->token));
  }
  if (node->tokens != NULL) {
    buffer_append_dbg_tokens(printer, node->tokens, "tokens");
  }
  if (node->initializer_node != NULL) {
    cdl_key(printer, "initializer_node");
    buffer_append_dbg_parse_node(printer, node->initializer_node);
  }
  if (node->initializer_type != NULL) {
    cdl_key(printer, "initializer_type");
    buffer_append_dbg_parse_node(printer, node->initializer_type);
  }
  cdl_end_table(printer);
}

void buffer_append_dbg_function_node(cdl_printer_t* printer,
                                     function_node_t* node) {
  cdl_start_table(printer);
  cdl_key(printer, "tag");
  cdl_string(printer, "PARSE_NODE_FUNCTION");
  cdl_key(printer, "attributes");
  buffer_append_dbg_node_list(printer, node->attributes);

  if (node->storage_class_specifier != NULL) {
    cdl_key(printer, "storage_class_specifier");
    cdl_string(printer, token_to_string(node->storage_class_specifier));
  }

  // FIXME! use function_specifiers

  if (node->return_type != NULL) {
    cdl_key(printer, "return_type");
    buffer_append_dbg_type_node(printer, node->return_type);
  }

  if (node->function_name != NULL) {
    cdl_key(printer, "function_name");
    cdl_string(printer, token_to_string(node->function_name));
  }

  cdl_key(printer, "function_args");
  buffer_append_dbg_node_list(printer, node->function_args);
  if (node->body != NULL) {
    cdl_key(printer, "body");
    buffer_append_dbg_parse_node(printer, node->body);
  }
  cdl_end_table(printer);
}

void buffer_append_dbg_function_argument_node(cdl_printer_t* printer,
                                              function_argument_node_t* node) {
  cdl_start_table(printer);
  cdl_key(printer, "tag");
  cdl_string(printer, "PARSE_NODE_FUNCTION_ARGUEMENT");

  if (node->arg_type != NULL) {
    cdl_key(printer, "arg_type");
    buffer_append_dbg_type_node(printer, node->arg_type);
  }
  if (node->arg_name != NULL) {
    cdl_key(printer, "arg_name");
    cdl_string(printer, token_to_string(node->arg_name));
  }
  cdl_end_table(printer);
}

void buffer_append_dbg_balanced_construct_node(
    cdl_printer_t* printer, balanced_construct_node_t* node) {
  cdl_start_table(printer);
  cdl_key(printer, "tag");
  cdl_string(printer, "PARSE_NODE_BALANCED_CONSTRUCT");
  if (node->start_token != NULL) {
    cdl_key(printer, "start_token");
    cdl_string(printer, token_to_string(node->start_token));
  }
  if (node->end_token != NULL) {
    cdl_key(printer, "end_token");
    cdl_string(printer, token_to_string(node->end_token));
  }
  cdl_end_table(printer);
}

void buffer_append_dbg_typedef_node(cdl_printer_t* printer,
                                    typedef_node_t* node) {
  cdl_start_table(printer);
  cdl_key(printer, "tag");
  cdl_string(printer, "PARSE_NODE_TYPEDEF");
  if (node->name != NULL) {
    cdl_key(printer, "name");
    cdl_string(printer, token_to_string(node->name));
  }
  if (node->type_node != NULL) {
    cdl_key(printer, "type_node");
    buffer_append_dbg_type_node(printer, node->type_node);
  }
  cdl_end_table(printer);
}

void buffer_append_dbg_variable_definition_node(
    cdl_printer_t* printer, variable_definition_node_t* node) {
  cdl_start_table(printer);
  cdl_key(printer, "tag");
  cdl_string(printer, "PARSE_NODE_VARIABLE_DEFINITION");
  if (node->name != NULL) {
    cdl_key(printer, "name");
    cdl_string(printer, token_to_string(node->name));
  }
  if (node->type != NULL) {
    cdl_key(printer, "type");
    buffer_append_dbg_type_node(printer, node->type);
  }
  if (node->value != NULL) {
    cdl_key(printer, "value");
    buffer_append_dbg_parse_node(printer, node->value);
  }
  if (node->storage_class_specifier != NULL) {
    cdl_key(printer, "storage_class_specifier");
    cdl_string(printer, token_to_string(node->storage_class_specifier));
  }
  cdl_end_table(printer);
}

void buffer_append_dbg_attribute_node(cdl_printer_t* printer,
                                      attribute_node_t* node) {
  cdl_start_table(printer);
  cdl_key(printer, "tag");
  cdl_string(printer, "PARSE_NODE_ATTRIBUTE");
  if (node->inner_start_token != NULL) {
    cdl_key(printer, "inner_start_token");
    cdl_string(printer, token_to_string(node->inner_start_token));
  }
  if (node->inner_end_token != NULL) {
    cdl_key(printer, "inner_end_token");
    cdl_string(printer, token_to_string(node->inner_end_token));
  }
  cdl_end_table(printer);
}

/* ====================================================================== */
/* Statements */
/* ====================================================================== */

/**
 * @function buffer_append_dbg_empty_statement_node
 */
void buffer_append_dbg_empty_statement_node(cdl_printer_t* printer,
                                            empty_statement_node_t* node) {
  cdl_start_table(printer);
  cdl_key(printer, "tag");
  cdl_string(printer, "PARSE_NODE_EMPTY_STATEMENT");
  cdl_end_table(printer);
}

/**
 * @function buffer_append_dbg_block_node
 */
void buffer_append_dbg_block_node(cdl_printer_t* printer, block_node_t* node) {
  cdl_start_table(printer);
  cdl_key(printer, "tag");
  cdl_string(printer, "PARSE_NODE_BLOCK");
  cdl_key(printer, "statements");
  buffer_append_dbg_node_list(printer, node->statements);
  cdl_end_table(printer);
}

void buffer_append_dbg_if_node(cdl_printer_t* printer,
                               if_statement_node_t* node) {
  cdl_start_table(printer);
  cdl_key(printer, "tag");
  cdl_string(printer, "PARSE_NODE_IF_STATEMENT");
  if (node->if_condition != NULL) {
    cdl_key(printer, "if_condition");
    buffer_append_dbg_parse_node(printer, node->if_condition);
  }
  if (node->if_true != NULL) {
    cdl_key(printer, "if_true");
    buffer_append_dbg_parse_node(printer, node->if_true);
  }
  if (node->if_else != NULL) {
    cdl_key(printer, "if_else");
    buffer_append_dbg_parse_node(printer, node->if_else);
  }
  cdl_end_table(printer);
}


void buffer_append_dbg_while_node(cdl_printer_t* printer,
                                  while_statement_node_t* node) {
  cdl_start_table(printer);
  cdl_key(printer, "tag");
  cdl_string(printer, "PARSE_NODE_WHILE_STATEMENT");
  if (node->condition != NULL) {
    cdl_key(printer, "condition");
    buffer_append_dbg_parse_node(printer, node->condition);
  }
  if (node->body != NULL) {
    cdl_key(printer, "body");
    buffer_append_dbg_parse_node(printer, node->body);
  }
  cdl_end_table(printer);
}

void buffer_append_dbg_for_node(cdl_printer_t* printer,
                                for_statement_node_t* node) {
  cdl_start_table(printer);
  cdl_key(printer, "tag");
  cdl_string(printer, "PARSE_NODE_FOR_STATEMENT");
  if (node->for_init != NULL) {
    cdl_key(printer, "for_init");
    buffer_append_dbg_parse_node(printer, node->for_init);
  }
  if (node->for_test != NULL) {
    cdl_key(printer, "for_test");
    buffer_append_dbg_parse_node(printer, node->for_test);
  }
  if (node->for_increment != NULL) {
    cdl_key(printer, "for_increment");
    buffer_append_dbg_parse_node(printer, node->for_increment);
  }
  if (node->for_body != NULL) {
    cdl_key(printer, "for_body");
    buffer_append_dbg_parse_node(printer, node->for_body);
  }
  cdl_end_table(printer);
}

void buffer_append_dbg_do_node(cdl_printer_t* printer,
                               do_statement_node_t* node) {
  cdl_start_table(printer);
  cdl_key(printer, "tag");
  cdl_string(printer, "PARSE_NODE_DO_STATEMENT");
  if (node->body != NULL) {
    cdl_key(printer, "body");
    buffer_append_dbg_parse_node(printer, node->body);
  }
  if (node->condition != NULL) {
    cdl_key(printer, "condition");
    buffer_append_dbg_parse_node(printer, node->condition);
  }
  cdl_end_table(printer);
}

void buffer_append_dbg_break_statement_node(cdl_printer_t* printer,
                                            break_statement_node_t* node) {
  cdl_start_table(printer);
  cdl_key(printer, "tag");
  cdl_string(printer, "PARSE_NODE_BREAK_STATEMENT");
  if (node->break_keyword_token != NULL) {
    cdl_key(printer, "break_keyword_token");
    cdl_string(printer, token_to_string(node->break_keyword_token));
  }
  cdl_end_table(printer);
}

void buffer_append_dbg_continue_statement_node(
    cdl_printer_t* printer, continue_statement_node_t* node) {
  cdl_start_table(printer);
  cdl_key(printer, "tag");
  cdl_string(printer, "PARSE_NODE_CONTINUE_STATEMENT");
  if (node->continue_keyword_token != NULL) {
    cdl_key(printer, "continue_keyword_token");
    cdl_string(printer, token_to_string(node->continue_keyword_token));
  }
  cdl_end_table(printer);
}

void buffer_append_dbg_label_statement_node(cdl_printer_t* printer,
                                            label_statement_node_t* node) {
  cdl_start_table(printer);
  cdl_key(printer, "tag");
  cdl_string(printer, "PARSE_NODE_LABEL_STATEMENT");
  if (node->label != NULL) {
    cdl_key(printer, "label");
    cdl_string(printer, token_to_string(node->label));
  }
  cdl_end_table(printer);
}

void buffer_append_dbg_case_label_node(cdl_printer_t* printer,
                                       case_label_node_t* node) {
  cdl_start_table(printer);
  cdl_key(printer, "tag");
  cdl_string(printer, "PARSE_NODE_CASE_LABEL");
  if (node->expression != NULL) {
    cdl_key(printer, "expression");
    buffer_append_dbg_parse_node(printer, node->expression);
  }
  cdl_end_table(printer);
}

void buffer_append_dbg_default_label_node(cdl_printer_t* printer,
                                          default_label_node_t* node) {
  cdl_start_table(printer);
  cdl_key(printer, "tag");
  cdl_string(printer, "PARSE_NODE_DEFAULT_LABEL");
  cdl_end_table(printer);
}

void buffer_append_dbg_return_statement_node(cdl_printer_t* printer,
                                             return_statement_node_t* node) {
  cdl_start_table(printer);
  cdl_key(printer, "tag");
  cdl_string(printer, "PARSE_NODE_RETURN_STATEMENT");
  if (node->expression != NULL) {
    cdl_key(printer, "expression");
    buffer_append_dbg_parse_node(printer, node->expression);
  }
  cdl_end_table(printer);
}

void buffer_append_dbg_expression_statement_node(
    cdl_printer_t* printer, expression_statement_node_t* node) {
  cdl_start_table(printer);
  cdl_key(printer, "tag");
  cdl_string(printer, "PARSE_NODE_EXPRESSION_STATEMENT");
  if (node->expression != NULL) {
    cdl_key(printer, "expression");
    buffer_append_dbg_parse_node(printer, node->expression);
  }
  cdl_end_table(printer);
}

/* ====================================================================== */

void buffer_append_dbg_identifier_node(cdl_printer_t* printer,
                                       identifier_node_t* node) {
  cdl_start_table(printer);
  cdl_key(printer, "tag");
  cdl_string(printer, "PARSE_NODE_IDENTIFIER");
  cdl_key(printer, "token");
  cdl_string(printer, token_to_string(node->token));
  cdl_end_table(printer);
}

void buffer_append_dbg_operator_node(cdl_printer_t* printer,
                                     operator_node_t* node) {
  cdl_start_table(printer);
  cdl_key(printer, "tag");
  cdl_string(printer, "PARSE_NODE_OPERATOR");
  cdl_key(printer, "operator");
  cdl_string(printer, token_to_string(node->operator));
  if (node->left != NULL) {
    cdl_key(printer, "left");
    buffer_append_dbg_parse_node(printer, node->left);
  }
  if (node->right != NULL) {
    cdl_key(printer, "right");
    buffer_append_dbg_parse_node(printer, node->right);
  }
  cdl_end_table(printer);
}

void buffer_append_dbg_call_node(cdl_printer_t* printer, call_node_t* node) {
  cdl_start_table(printer);
  cdl_key(printer, "tag");
  cdl_string(printer, "PARSE_NODE_CALL");
  cdl_key(printer, "function");
  buffer_append_dbg_parse_node(printer, node->function);
  cdl_key(printer, "args");
  buffer_append_dbg_node_list(printer, node->args);
  cdl_end_table(printer);
}

void buffer_append_dbg_switch_node(cdl_printer_t* printer,
                                   switch_statement_node_t* node) {
  cdl_start_table(printer);
  cdl_key(printer, "tag");
  cdl_string(printer, "PARSE_NODE_SWITCH_STATEMENT");
  if (node->expression != NULL) {
    cdl_key(printer, "expression");
    buffer_append_dbg_parse_node(printer, node->expression);
  }
  if (node->block != NULL) {
    cdl_key(printer, "block");
    buffer_append_dbg_parse_node(printer, node->block);
  }
  cdl_end_table(printer);
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
