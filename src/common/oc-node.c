/**
 * This is the parse tree after conversion from the similar parse tree
 * in TSNode's.
 */

#ifndef _OC_NODE_H_
#define _OC_NODE_H_

#include "../../../c-armyknife-lib/c-armyknife-lib.h"
#include <tree_sitter/api.h>

typedef enum {
  OC_NODE_UNKNOWN,

  OC_NODE_ARGUMENT_LIST,
  OC_NODE_ASSIGNMENT_EXPRESSION,
  OC_NODE_BINARY_EXPRESSION,
  OC_NODE_CALL_EXPRESSION,
  OC_NODE_COMMENT,
  OC_NODE_COMPOUND_STATEMENT,
  OC_NODE_DECLARATION,
  OC_NODE_ELSE_CLAUSE,
  OC_NODE_ESCAPE_SEQUENCE,
  OC_NODE_EXPRESSION_STATEMENT,
  OC_NODE_FALSE,
  OC_NODE_FIELD_EXPRESSION,
  OC_NODE_FIELD_IDENTIFIER,
  OC_NODE_FOR_STATEMENT,
  OC_NODE_FUNCTION_DECLARATOR,
  OC_NODE_FUNCTION_DEFINITION,
  OC_NODE_IDENTIFIER,
  OC_NODE_IF_STATEMENT,
  OC_NODE_INIT_DECLARATOR,
  OC_NODE_NULL,
  OC_NODE_NUMBER_LITERAL,
  OC_NODE_PARAMETER_DECLARATION,
  OC_NODE_PARAMETER_LIST,
  OC_NODE_PARENTHESIZED_EXPRESSION,
  OC_NODE_POINTER_DECLARATOR,
  OC_NODE_PREPROC_INCLUDE,
  OC_NODE_PRIMITIVE_TYPE,
  OC_NODE_STRING_CONTENT,
  OC_NODE_STRING_LITERAL,
  OC_NODE_SYSTEM_LIB_STRING,
  OC_NODE_TRANSLATION_UNIT,
  OC_NODE_TRUE,
  OC_NODE_TYPE_IDENTIFIER,
  OC_NODE_UPDATE_EXPRESSION,

  OC_NODE__LAST__
} oc_node_tag_t;

struct oc_node_S {
  oc_node_tag_t tag;

  char* text; // only for leaf nodes like constants and identifiers

  ptr_array_t* children;

  // Additional fields are per tag and per analysis phase

  char* var_name; // an alpha renamed version of the variable name
                  // alpha renaming

  char* expr_type; // example: uint64 or string

  TSNode ts_node; // Unless this is generated code, we can access the
                  // original TSNode
};

typedef struct oc_node_S oc_node_t;

extern oc_node_t* ts_node_to_oc_node(TSNode ts_node, buffer_t* source_code);
extern buffer_t* append_oc_node_text(buffer_t* buffer, oc_node_t* node);

#endif /* _OC_NODE_H_ */

oc_node_tag_t ts_node_name_to_tag(const char* name) {

  switch (name[0]) {
  case 'a':
    if (string_equal("argument_list", name)) {
      return OC_NODE_ARGUMENT_LIST;
    }
    if (string_equal("assignment_expression", name)) {
      return OC_NODE_ASSIGNMENT_EXPRESSION;
    }
    break;

  case 'b':
    if (string_equal("binary_expression", name)) {
      return OC_NODE_BINARY_EXPRESSION;
    }
    break;

  case 'c':
    if (string_equal("comment", name)) {
      return OC_NODE_COMMENT;
    }
    if (string_equal("compound_statement", name)) {
      return OC_NODE_COMPOUND_STATEMENT;
    }
    if (string_equal("call_expression", name)) {
      return OC_NODE_CALL_EXPRESSION;
    }
    break;

  case 'd':
    if (string_equal("declaration", name)) {
      return OC_NODE_DECLARATION;
    }
    break;

  case 'e':
    if (string_equal("expression_statement", name)) {
      return OC_NODE_EXPRESSION_STATEMENT;
    }
    if (string_equal("escape_sequence", name)) {
      return OC_NODE_ESCAPE_SEQUENCE;
    }
    if (string_equal("else_clause", name)) {
      return OC_NODE_ELSE_CLAUSE;
    }
    break;

  case 'f':
    if (string_equal("function_definition", name)) {
      return OC_NODE_FUNCTION_DECLARATOR;
    }
    if (string_equal("false", name)) {
      return OC_NODE_FALSE;
    }
    if (string_equal("field_expression", name)) {
      return OC_NODE_FIELD_EXPRESSION;
    }
    if (string_equal("field_identifier", name)) {
      return OC_NODE_FIELD_IDENTIFIER;
    }
    if (string_equal("for_statement", name)) {
      return OC_NODE_FOR_STATEMENT;
    }
    if (string_equal("function_declarator", name)) {
      return OC_NODE_FUNCTION_DECLARATOR;
    }
    break;

  case 'i':
    if (string_equal("if_statement", name)) {
      return OC_NODE_IF_STATEMENT;
    }
    if (string_equal("identifier", name)) {
      return OC_NODE_IDENTIFIER;
    }
    if (string_equal("init_declarator", name)) {
      return OC_NODE_INIT_DECLARATOR;
    }
    break;

  case 'n':
    if (string_equal("null", name)) {
      return OC_NODE_NULL;
    }
    if (string_equal("number_literal", name)) {
      return OC_NODE_NUMBER_LITERAL;
    }
    break;

  case 'p':
    if (string_equal("parameter_declaration", name)) {
      return OC_NODE_PARAMETER_DECLARATION;
    }
    if (string_equal("parameter_list", name)) {
      return OC_NODE_PARAMETER_LIST;
    }
    if (string_equal("preproc_include", name)) {
      return OC_NODE_PREPROC_INCLUDE;
    }
    if (string_equal("primitive_type", name)) {
      return OC_NODE_PRIMITIVE_TYPE;
    }
    if (string_equal("parenthesized_expression", name)) {
      return OC_NODE_PARENTHESIZED_EXPRESSION;
    }
    if (string_equal("pointer_declarator", name)) {
      return OC_NODE_POINTER_DECLARATOR;
    }
    break;

  case 's':
    if (string_equal("string_literal", name)) {
      return OC_NODE_STRING_LITERAL;
    }
    if (string_equal("string_content", name)) {
      return OC_NODE_STRING_CONTENT;
    }
    if (string_equal("system_lib_string", name)) {
      return OC_NODE_SYSTEM_LIB_STRING;
    }
    break;

  case 't':
    if (string_equal("translation_unit", name)) {
      return OC_NODE_TRANSLATION_UNIT;
    }
    if (string_equal("true", name)) {
      return OC_NODE_TRUE;
    }
    if (string_equal("type_identifier", name)) {
      return OC_NODE_TYPE_IDENTIFIER;
    }
    break;

  case 'u':
    if (string_equal("update_expression", name)) {
      return OC_NODE_UPDATE_EXPRESSION;
    }
    break;
  }

  fprintf(stderr, "WARNING: unknown node named %s\n", name);
  return OC_NODE_UNKNOWN;
}

char* oc_node_tag_to_string(oc_node_tag_t tag) {
  switch (tag) {
  default:
    return "OC_NODE_UNKNOWN";
  }
}

void extract_oc_node_text(oc_node_t* oc_node, TSNode ts_node,
                          buffer_t* source_code) {
  if (ts_node_child_count(ts_node) == 0) {
    oc_node->text = buffer_c_substring(source_code, ts_node_start_byte(ts_node),
                                       ts_node_end_byte(ts_node));
  }
}

/**
 * Recursively walks a ts_node and it's children and produces a
 * oc_node_t* from it.
 */
oc_node_t* ts_node_to_oc_node(TSNode ts_node, buffer_t* source_code) {
  oc_node_t* result = malloc_struct(oc_node_t);
  result->tag = ts_node_name_to_tag(ts_node_type(ts_node));
  extract_oc_node_text(result, ts_node, source_code);

  int n_children = ts_node_named_child_count(ts_node);
  result->children = n_children > 0 ? make_ptr_array(n_children) : NULL;
  for (int i = 0; i < n_children; i++) {
    ptr_array_add(
        result->children,
        ts_node_to_oc_node(ts_node_named_child(ts_node, i), source_code));
  }
  return result;
}

/**
 * Append a textual representation of an oc_node_t* to the buffer.
 */
buffer_t* append_oc_node_text(buffer_t* buffer, oc_node_t* node) {
  buffer = buffer_append_string(buffer, "(");
  buffer = buffer_append_string(buffer, oc_node_tag_to_string(node->tag));

  if (node->children) {
    for (int i = 0; i < node->children->length; i++) {
      buffer = buffer_append_string(buffer, " ");
      oc_node_t* child = (oc_node_t*) ptr_array_get(node->children, i);
      buffer = append_oc_node_text(buffer, child);
    }
  }

  buffer = buffer_append_string(buffer, ")");
}
