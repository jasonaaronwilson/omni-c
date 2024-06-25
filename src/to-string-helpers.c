#line 2 "to-string-helpers.c"
#ifndef _TO_STRING_HELPERS_H_
#define _TO_STRING_HELPERS_H_

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

char* parse_node_type_to_string(parse_node_type_t type) {
  switch (type) {
  default:
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
  case PARSE_NODE_GLOBAL_VARIABLE:
    return "PARSE_NODE_GLOBAL_VARIABLE";
  case PARSE_NODE_LIST_OF_NODES:
    return "PARSE_NODE_LIST_OF_NODES";
  case PARSE_NODE_STRUCT:
    return "PARSE_NODE_STRUCT";
  case PARSE_NODE_UNION:
    return "PARSE_NODE_UNION";
  case PARSE_NODE_TYPE:
    return "PARSE_NODE_TYPE";
  }
  return "**NOT REACHED**";
}

char* parse_error_code_to_string(parse_error_code_t error_code) {
  switch (error_code) {
  default:
  case PARSE_ERROR_UNKNOWN:
    return "PARSE_ERROR_UNKNOWN";
  case PARSE_ERROR_COMMA_OR_EQUAL_SIGN_EXPECTED:
    return "PARSE_ERROR_COMMA_OR_EQUAL_SIGN_EXPECTED";
  case PARSE_ERROR_EXPECTED_FIELD_WIDTH_OR_SEMICOLON:
    return "PARSE_ERROR_EXPECTED_FIELD_WIDTH_OR_SEMICOLON";
  case PARSE_ERROR_EXPECTED_SEMICOLON:
    return "PARSE_ERROR_EXPECTED_SEMICOLON";
  case PARSE_ERROR_IDENTIFIER_EXPECTED:
    return "PARSE_ERROR_IDENTIFIER_EXPECTED";
  case PARSE_ERROR_INTEGER_LITERAL_EXPECTED:
    return "PARSE_ERROR_INTEGER_LITERAL_EXPECTED";
  case PARSE_ERROR_OPEN_BRACE_EXPECTED:
    return "PARSE_ERROR_OPEN_BRACE_EXPECTED";
  case PARSE_ERROR_UNRECOGNIZED_TOP_LEVEL_DECLARATION:
    return "PARSE_ERROR_UNRECOGNIZED_TOP_LEVEL_DECLARATION";
  }
  return "**NOT REACHED**";
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
  }
  return "**NOT REACHED**";
}
