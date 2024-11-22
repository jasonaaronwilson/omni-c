#line 2 "to-string-helpers.c"
#ifndef _TO_STRING_HELPERS_H_
#define _TO_STRING_HELPERS_H_

#include "declaration-parser.h"
#include "parser.h"

/**
 * @file to-string-helpers.c
 *
 * One of the goals of Omni C is to automatically generate both enum
 * -> string and string -> enum functions. For now, we put the
 * hand-written versions here.
 */

#include "to-string-helpers.c.generated.h"

#endif /* _TO_STRING_HELPERS_H_ */

// I grabbed this from self.c and reformatted it.
char* parse_node_type_to_string(parse_node_type_t value) {
  switch (value) {
  case PARSE_NODE_UNKNOWN:
    return "PARSE_NODE_UNKNOWN";
  case PARSE_NODE_DECLARATIONS:
    return "PARSE_NODE_DECLARATIONS";
  case PARSE_NODE_ENUM:
    return "PARSE_NODE_ENUM";
  case PARSE_NODE_ENUM_ELEMENT:
    return "PARSE_NODE_ENUM_ELEMENT";
  case PARSE_NODE_FIELD:
    return "PARSE_NODE_FIELD";
  case PARSE_NODE_GLOBAL_FUNCTION:
    return "PARSE_NODE_GLOBAL_FUNCTION";
  case PARSE_NODE_VARIABLE_DEFINITION:
    return "PARSE_NODE_VARIABLE_DEFINITION";
  case PARSE_NODE_LIST_OF_NODES:
    return "PARSE_NODE_LIST_OF_NODES";
  case PARSE_NODE_STRUCT:
    return "PARSE_NODE_STRUCT";
  case PARSE_NODE_UNION:
    return "PARSE_NODE_UNION";
  case PARSE_NODE_TYPE:
    return "PARSE_NODE_TYPE";
  case PARSE_NODE_LITERAL:
    return "PARSE_NODE_LITERAL";
  case PARSE_NODE_FUNCTION:
    return "PARSE_NODE_FUNCTION";
  case PARSE_NODE_FUNCTION_ARGUMENT:
    return "PARSE_NODE_FUNCTION_ARGUMENT";
  case PARSE_NODE_TYPEDEF:
    return "PARSE_NODE_TYPEDEF";
  case PARSE_NODE_UNPARSED_EXPRESSION:
    return "PARSE_NODE_UNPARSED_EXPRESSION";
  case PARSE_NODE_ATTRIBUTE:
    return "PARSE_NODE_ATTRIBUTE";
  case PARSE_NODE_CPP_INCLUDE:
    return "PARSE_NODE_CPP_INCLUDE";
  case PARSE_NODE_CPP_DEFINE:
    return "PARSE_NODE_CPP_DEFINE";
  case PARSE_NODE_OPERATOR:
    return "PARSE_NODE_OPERATOR";
  case PARSE_NODE_IDENTIFIER:
    return "PARSE_NODE_IDENTIFIER";
  case PARSE_NODE_IF_STATEMENT:
    return "PARSE_NODE_IF_STATEMENT";
  case PARSE_NODE_FOR_STATEMENT:
    return "PARSE_NODE_FOR_STATEMENT";
  case PARSE_NODE_DO_STATEMENT:
    return "PARSE_NODE_DO_STATEMENT";
  case PARSE_NODE_WHILE_STATEMENT:
    return "PARSE_NODE_WHILE_STATEMENT";
  case PARSE_NODE_EMPTY_STATEMENT:
    return "PARSE_NODE_EMPTY_STATEMENT";
  case PARSE_NODE_BLOCK:
    return "PARSE_NODE_BLOCK";
  case PARSE_NODE_RETURN_STATEMENT:
    return "PARSE_NODE_RETURN_STATEMENT";
  case PARSE_NODE_SWITCH_STATEMENT:
    return "PARSE_NODE_SWITCH_STATEMENT";
  case PARSE_NODE_CASE_LABEL:
    return "PARSE_NODE_CASE_LABEL";
  case PARSE_NODE_DEFAULT_LABEL:
    return "PARSE_NODE_DEFAULT_LABEL";
  case PARSE_NODE_GOTO_STATEMENT:
    return "PARSE_NODE_GOTO_STATEMENT";
  case PARSE_NODE_BREAK_STATEMENT:
    return "PARSE_NODE_BREAK_STATEMENT";
  case PARSE_NODE_CONTINUE_STATEMENT:
    return "PARSE_NODE_CONTINUE_STATEMENT";
  case PARSE_NODE_LABEL_STATEMENT:
    return "PARSE_NODE_LABEL_STATEMENT";
  case PARSE_NODE_VARIABLE_STATEMENT:
    return "PARSE_NODE_VARIABLE_STATEMENT";
  case PARSE_NODE_EXPRESSION_STATEMENT:
    return "PARSE_NODE_EXPRESSION_STATEMENT";
  case PARSE_NODE_BALANCED_CONSTRUCT:
    return "PARSE_NODE_BALANCED_CONSTRUCT";
  case PARSE_NODE_CALL:
    return "PARSE_NODE_CALL";
  default:
    return "<<unknown-parse_node_type>>";
  }
}

char* type_node_kind_to_string(type_node_kind_t kind) {
  switch (kind) {
  default:
  case TYPE_NODE_KIND_UNKNOWN:
    return "TYPE_NODE_KIND_UNKNOWN";
  case TYPE_NODE_KIND_POINTER:
    return "TYPE_NODE_KIND_POINTER";
  case TYPE_NODE_KIND_ARRAY:
    return "TYPE_NODE_KIND_ARRAY";
  case TYPE_NODE_KIND_SIZED_ARRAY:
    return "TYPE_NODE_KIND_SIZED_ARRAY";
  case TYPE_NODE_KIND_VARIABLE_SIZED_ARRAY:
    return "TYPE_NODE_KIND_VARIABLE_SIZED_ARRAY";
  case TYPE_NODE_KIND_PRIMITIVE_TYPENAME:
    return "TYPE_NODE_KIND_PRIMITIVE_TYPENAME";
  case TYPE_NODE_KIND_TYPENAME:
    return "TYPE_NODE_KIND_TYPENAME";
  case TYPE_NODE_KIND_TYPE_EXPRESSION:
    return "TYPE_NODE_KIND_TYPE_EXPRESSION";
  case TYPE_NODE_KIND_TYPEOF:
    return "TYPE_NODE_KIND_TYPEOF";
  }
  return "**NOT REACHED**";
}

char* type_qualifier_to_string(type_qualifier_t value) {
  switch (value) {
  case TYPE_QUALIFIER_NONE:
    return "TYPE_QUALIFIER_NONE";
  case TYPE_QUALIFIER_CONST:
    return "TYPE_QUALIFIER_CONST";
  case TYPE_QUALIFIER_VOLATILE:
    return "TYPE_QUALIFIER_VOLATILE";
  case TYPE_QUALIFIER_RESTRICT:
    return "TYPE_QUALIFIER_RESTRICT";
  default:
    return "<<unknown-type_qualifier>>";
  }
}
