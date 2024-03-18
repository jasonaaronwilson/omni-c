/**
 * This is the parse tree after conversion from the similar parse tree
 * in TSNode's.
 */

#ifndef _OC_NODE_H_
#define _OC_NODE_H_

#include "../../../c-armyknife-lib/c-armyknife-lib.h"
#include <tree_sitter/api.h>

typedef enum {
  OC_NODE_ERROR,
  OC_NODE_ABSTRACT_ARRAY_DECLARATOR,
  OC_NODE_ABSTRACT_FUNCTION_DECLARATOR,
  OC_NODE_ABSTRACT_PARENTHESIZED_DECLARATOR,
  OC_NODE_ABSTRACT_POINTER_DECLARATOR,
  OC_NODE_ALIGNOF_EXPRESSION,
  OC_NODE_ARGUMENT_LIST,
  OC_NODE_ARRAY_DECLARATOR,
  OC_NODE_ASSIGNMENT_EXPRESSION,
  OC_NODE_ATTRIBUTE,
  OC_NODE_ATTRIBUTED_DECLARATOR,
  OC_NODE_ATTRIBUTED_STATEMENT,
  OC_NODE_ATTRIBUTE_DECLARATION,
  OC_NODE_ATTRIBUTE_SPECIFIER,
  OC_NODE_BINARY_EXPRESSION,
  OC_NODE_BITFIELD_CLAUSE,
  OC_NODE_BREAK_STATEMENT,
  OC_NODE_CALL_EXPRESSION,
  OC_NODE_CASE_STATEMENT,
  OC_NODE_CAST_EXPRESSION,
  OC_NODE_CHARACTER,
  OC_NODE_CHAR_LITERAL,
  OC_NODE_COMMA_EXPRESSION,
  OC_NODE_COMMENT,
  OC_NODE_COMPOUND_LITERAL_EXPRESSION,
  OC_NODE_COMPOUND_STATEMENT,
  OC_NODE_CONCATENATED_STRING,
  OC_NODE_CONDITIONAL_EXPRESSION,
  OC_NODE_CONTINUE_STATEMENT,
  OC_NODE_DECLARATION,
  OC_NODE_DECLARATION_LIST,
  OC_NODE_DO_STATEMENT,
  OC_NODE_ELSE_CLAUSE,
  OC_NODE_ENUMERATOR,
  OC_NODE_ENUMERATOR_LIST,
  OC_NODE_ENUM_SPECIFIER,
  OC_NODE_ESCAPE_SEQUENCE,
  OC_NODE_EXPRESSION_STATEMENT,
  OC_NODE_FALSE,
  OC_NODE_FIELD_DECLARATION,
  OC_NODE_FIELD_DECLARATION_LIST,
  OC_NODE_FIELD_DESIGNATOR,
  OC_NODE_FIELD_EXPRESSION,
  OC_NODE_FIELD_IDENTIFIER,
  OC_NODE_FOR_STATEMENT,
  OC_NODE_FUNCTION_DECLARATOR,
  OC_NODE_FUNCTION_DEFINITION,
  OC_NODE_GENERIC_EXPRESSION,
  OC_NODE_GOTO_STATEMENT,
  OC_NODE_IDENTIFIER,
  OC_NODE_IF_STATEMENT,
  OC_NODE_INITIALIZER_LIST,
  OC_NODE_INITIALIZER_PAIR,
  OC_NODE_INIT_DECLARATOR,
  OC_NODE_LABELED_STATEMENT,
  OC_NODE_LINKAGE_SPECIFICATION,
  OC_NODE_MACRO_TYPE_SPECIFIER,
  OC_NODE_MS_DECLSPEC_MODIFIER,
  OC_NODE_MS_POINTER_MODIFIER,
  OC_NODE_MS_RESTRICT_MODIFIER,
  OC_NODE_NULL,
  OC_NODE_NUMBER_LITERAL,
  OC_NODE_OFFSETOF_EXPRESSION,
  OC_NODE_PARAMETER_DECLARATION,
  OC_NODE_PARAMETER_LIST,
  OC_NODE_PARENTHESIZED_DECLARATOR,
  OC_NODE_PARENTHESIZED_EXPRESSION,
  OC_NODE_POINTER_DECLARATOR,
  OC_NODE_POINTER_EXPRESSION,
  OC_NODE_PREPROC_ARG,
  OC_NODE_PREPROC_CALL,
  OC_NODE_PREPROC_DEF,
  OC_NODE_PREPROC_DEFINED,
  OC_NODE_PREPROC_DIRECTIVE,
  OC_NODE_PREPROC_ELIF,
  OC_NODE_PREPROC_ELSE,
  OC_NODE_PREPROC_FUNCTION_DEF,
  OC_NODE_PREPROC_IF,
  OC_NODE_PREPROC_IFDEF,
  OC_NODE_PREPROC_INCLUDE,
  OC_NODE_PREPROC_PARAMS,
  OC_NODE_PRIMITIVE_TYPE,
  OC_NODE_RETURN_STATEMENT,
  OC_NODE_SIZED_TYPE_SPECIFIER,
  OC_NODE_SIZEOF_EXPRESSION,
  OC_NODE_STATEMENT_IDENTIFIER,
  OC_NODE_STORAGE_CLASS_SPECIFIER,
  OC_NODE_STRING_CONTENT,
  OC_NODE_STRING_LITERAL,
  OC_NODE_STRUCT_SPECIFIER,
  OC_NODE_SUBSCRIPT_DESIGNATOR,
  OC_NODE_SUBSCRIPT_EXPRESSION,
  OC_NODE_SWITCH_STATEMENT,
  OC_NODE_SYSTEM_LIB_STRING,
  OC_NODE_TRANSLATION_UNIT,
  OC_NODE_TRUE,
  OC_NODE_TYPE_DEFINITION,
  OC_NODE_TYPE_DESCRIPTOR,
  OC_NODE_TYPE_IDENTIFIER,
  OC_NODE_TYPE_QUALIFIER,
  OC_NODE_UNARY_EXPRESSION,
  OC_NODE_UNION_SPECIFIER,
  OC_NODE_UPDATE_EXPRESSION,
  OC_NODE_VARIADIC_PARAMETER,
  OC_NODE_WHILE_STATEMENT,

  // gnu_asm_expression
  // gnu_asm_output_operand_list
  // gnu_asm_input_operand_list
  // gnu_asm_input_operand
  // gnu_asm_input_operand
  // gnu_asm_expression
  // gnu_asm_output_operand_list
  // gnu_asm_output_operand
  // gnu_asm_clobber_list

} oc_node_tag_t;

struct oc_node_S {
  oc_node_tag_t tag;

  char* text; // only for leaf nodes like constants and identifiers

  value_array_t* children;

  // Additional fields are per tag and per analysis phase

  char* var_name; // an alpha renamed version of the variable name
                  // alpha renaming

  char* expr_type; // example: uint64 or string

  TSNode ts_node; // Unless this is generated code, we can access the
                  // original TSNode
};

typedef struct oc_node_S oc_node_t;

extern oc_node_t* ts_node_to_oc_node(TSNode ts_node, buffer_t* source_code);

__attribute__((warn_unused_result)) extern buffer_t*
    append_oc_node_text(buffer_t* buffer, oc_node_t* node);

__attribute__((warn_unused_result)) extern buffer_t*
    append_oc_node_tree(buffer_t* buffer, oc_node_t* node);

#endif /* _OC_NODE_H_ */

oc_node_tag_t ts_node_name_to_tag(const char* name) {

  switch (name[0]) {

    // The only capitalized node name appears to be ERROR
  case 'E':
    if (string_equal("ERROR", name)) {
      return OC_NODE_ERROR;
    }
    break;

  case 'a':
    if (string_equal("argument_list", name)) {
      return OC_NODE_ARGUMENT_LIST;
    }
    if (string_equal("assignment_expression", name)) {
      return OC_NODE_ASSIGNMENT_EXPRESSION;
    }
    if (string_equal("abstract_pointer_declarator", name)) {
      return OC_NODE_ABSTRACT_POINTER_DECLARATOR;
    }
    if (string_equal("abstract_array_declarator", name)) {
      return OC_NODE_ABSTRACT_ARRAY_DECLARATOR;
    }
    if (string_equal("array_declarator", name)) {
      return OC_NODE_ARRAY_DECLARATOR;
    }
    if (string_equal("abstract_function_declarator", name)) {
      return OC_NODE_ABSTRACT_FUNCTION_DECLARATOR;
    }
    if (string_equal("abstract_parenthesized_declarator", name)) {
      return OC_NODE_ABSTRACT_FUNCTION_DECLARATOR;
    }
    if (string_equal("alignof_expression", name)) {
      return OC_NODE_ALIGNOF_EXPRESSION;
    }
    if (string_equal("attribute_specifier", name)) {
      return OC_NODE_ATTRIBUTE_SPECIFIER;
    }
    if (string_equal("attribute", name)) {
      return OC_NODE_ATTRIBUTE;
    }
    if (string_equal("attribute_declaration", name)) {
      return OC_NODE_ATTRIBUTE_DECLARATION;
    }
    if (string_equal("attributed_declarator", name)) {
      return OC_NODE_ATTRIBUTED_DECLARATOR;
    }
    if (string_equal("attributed_statement", name)) {
      return OC_NODE_ATTRIBUTED_STATEMENT;
    }
    break;

  case 'b':
    if (string_equal("binary_expression", name)) {
      return OC_NODE_BINARY_EXPRESSION;
    }
    if (string_equal("break_statement", name)) {
      return OC_NODE_BREAK_STATEMENT;
    }
    if (string_equal("bitfield_clause", name)) {
      return OC_NODE_BITFIELD_CLAUSE;
    }
    break;

  case 'c':
    if (string_equal("call_expression", name)) {
      return OC_NODE_CALL_EXPRESSION;
    }
    if (string_equal("case_statement", name)) {
      return OC_NODE_CASE_STATEMENT;
    }
    if (string_equal("cast_expression", name)) {
      return OC_NODE_CAST_EXPRESSION;
    }
    if (string_equal("char_literal", name)) {
      return OC_NODE_CHAR_LITERAL;
    }
    if (string_equal("character", name)) {
      return OC_NODE_CHARACTER;
    }
    if (string_equal("comment", name)) {
      return OC_NODE_COMMENT;
    }
    if (string_equal("compound_statement", name)) {
      return OC_NODE_COMPOUND_STATEMENT;
    }
    if (string_equal("compound_literal_expression", name)) {
      return OC_NODE_COMPOUND_LITERAL_EXPRESSION;
    }
    if (string_equal("conditional_expression", name)) {
      return OC_NODE_CONDITIONAL_EXPRESSION;
    }
    if (string_equal("comma_expression", name)) {
      return OC_NODE_COMMA_EXPRESSION;
    }
    if (string_equal("compound_statement", name)) {
      return OC_NODE_COMPOUND_STATEMENT;
    }
    if (string_equal("concatenated_string", name)) {
      return OC_NODE_CONCATENATED_STRING;
    }
    if (string_equal("continue_statement", name)) {
      return OC_NODE_CONTINUE_STATEMENT;
    }
    break;

  case 'd':
    if (string_equal("declaration", name)) {
      return OC_NODE_DECLARATION;
    }
    if (string_equal("declaration_list", name)) {
      return OC_NODE_DECLARATION_LIST;
    }
    if (string_equal("do_statement", name)) {
      return OC_NODE_DO_STATEMENT;
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
    if (string_equal("enum_specifier", name)) {
      return OC_NODE_ENUM_SPECIFIER;
    }
    if (string_equal("enumerator", name)) {
      return OC_NODE_ENUMERATOR;
    }
    if (string_equal("enumerator_list", name)) {
      return OC_NODE_ENUMERATOR_LIST;
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
    if (string_equal("field_declaration", name)) {
      return OC_NODE_FIELD_DECLARATION;
    }
    if (string_equal("field_declaration_list", name)) {
      return OC_NODE_FIELD_DECLARATION_LIST;
    }
    if (string_equal("field_designator", name)) {
      return OC_NODE_FIELD_DESIGNATOR;
    }
    if (string_equal("for_statement", name)) {
      return OC_NODE_FOR_STATEMENT;
    }
    if (string_equal("function_declarator", name)) {
      return OC_NODE_FUNCTION_DECLARATOR;
    }
    break;

  case 'g':
    if (string_equal("goto_statement", name)) {
      return OC_NODE_GOTO_STATEMENT;
    }
    if (string_equal("generic_expression", name)) {
      return OC_NODE_GENERIC_EXPRESSION;
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
    if (string_equal("initializer_list", name)) {
      return OC_NODE_INITIALIZER_LIST;
    }
    if (string_equal("initializer_pair", name)) {
      return OC_NODE_INITIALIZER_PAIR;
    }
    break;

  case 'l':
    if (string_equal("labeled_statement", name)) {
      return OC_NODE_LABELED_STATEMENT;
    }
    if (string_equal("linkage_specification", name)) {
      return OC_NODE_LINKAGE_SPECIFICATION;
    }
    break;

  case 'm':
    if (string_equal("macro_type_specifier", name)) {
      return OC_NODE_MACRO_TYPE_SPECIFIER;
    }
    if (string_equal("ms_pointer_modifier", name)) {
      return OC_NODE_MS_POINTER_MODIFIER;
    }
    if (string_equal("ms_restrict_modifier", name)) {
      return OC_NODE_MS_RESTRICT_MODIFIER;
    }
    if (string_equal("ms_declspec_modifier", name)) {
      return OC_NODE_MS_DECLSPEC_MODIFIER;
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

  case 'o':
    if (string_equal("offsetof_expression", name)) {
      return OC_NODE_OFFSETOF_EXPRESSION;
    }
    break;

  case 'p':
    if (string_starts_with(name, "preproc")) {
      if (string_equal("preproc_include", name)) {
        return OC_NODE_PREPROC_INCLUDE;
      }
      if (string_equal("preproc_arg", name)) {
        return OC_NODE_PREPROC_ARG;
      }
      if (string_equal("preproc_call", name)) {
        return OC_NODE_PREPROC_CALL;
      }
      if (string_equal("preproc_def", name)) {
        return OC_NODE_PREPROC_DEF;
      }
      if (string_equal("preproc_directive", name)) {
        return OC_NODE_PREPROC_DIRECTIVE;
      }
      if (string_equal("preproc_ifdef", name)) {
        return OC_NODE_PREPROC_IFDEF;
      }
      if (string_equal("preproc_defined", name)) {
        return OC_NODE_PREPROC_DEFINED;
      }
      if (string_equal("preproc_if", name)) {
        return OC_NODE_PREPROC_IF;
      }
      if (string_equal("preproc_elif", name)) {
        return OC_NODE_PREPROC_ELIF;
      }
      if (string_equal("preproc_else", name)) {
        return OC_NODE_PREPROC_ELSE;
      }
      if (string_equal("preproc_function_def", name)) {
        return OC_NODE_PREPROC_FUNCTION_DEF;
      }
      if (string_equal("preproc_params", name)) {
        return OC_NODE_PREPROC_PARAMS;
      }
      break;
    }
    if (string_equal("parameter_declaration", name)) {
      return OC_NODE_PARAMETER_DECLARATION;
    }
    if (string_equal("parameter_list", name)) {
      return OC_NODE_PARAMETER_LIST;
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
    if (string_equal("pointer_expression", name)) {
      return OC_NODE_POINTER_EXPRESSION;
    }
    if (string_equal("parenthesized_declarator", name)) {
      return OC_NODE_PARENTHESIZED_DECLARATOR;
    }

    break;

  case 'r':
    if (string_equal("return_statement", name)) {
      return OC_NODE_RETURN_STATEMENT;
    }
    break;

  case 's':
    if (string_equal("subscript_expression", name)) {
      return OC_NODE_SUBSCRIPT_EXPRESSION;
    }
    if (string_equal("switch_statement", name)) {
      return OC_NODE_SWITCH_STATEMENT;
    }
    if (string_equal("struct_specifier", name)) {
      return OC_NODE_STRUCT_SPECIFIER;
    }
    if (string_equal("storage_class_specifier", name)) {
      return OC_NODE_STORAGE_CLASS_SPECIFIER;
    }
    if (string_equal("string_literal", name)) {
      return OC_NODE_STRING_LITERAL;
    }
    if (string_equal("string_content", name)) {
      return OC_NODE_STRING_CONTENT;
    }
    if (string_equal("system_lib_string", name)) {
      return OC_NODE_SYSTEM_LIB_STRING;
    }
    if (string_equal("sizeof_expression", name)) {
      return OC_NODE_SIZEOF_EXPRESSION;
    }
    if (string_equal("sized_type_specifier", name)) {
      return OC_NODE_SIZED_TYPE_SPECIFIER;
    }
    if (string_equal("statement_identifier", name)) {
      return OC_NODE_STATEMENT_IDENTIFIER;
    }
    if (string_equal("subscript_designator", name)) {
      return OC_NODE_SUBSCRIPT_DESIGNATOR;
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
    if (string_equal("type_definition", name)) {
      return OC_NODE_TYPE_DEFINITION;
    }
    if (string_equal("type_descriptor", name)) {
      return OC_NODE_TYPE_DESCRIPTOR;
    }
    if (string_equal("type_qualifier", name)) {
      return OC_NODE_TYPE_QUALIFIER;
    }
    break;

  case 'u':
    if (string_equal("update_expression", name)) {
      return OC_NODE_UPDATE_EXPRESSION;
    }
    if (string_equal("unary_expression", name)) {
      return OC_NODE_UNARY_EXPRESSION;
    }
    if (string_equal("union_specifier", name)) {
      return OC_NODE_UNION_SPECIFIER;
    }
    break;

  case 'v':
    if (string_equal("variadic_parameter", name)) {
      return OC_NODE_VARIADIC_PARAMETER;
    }
    break;

  case 'w':
    if (string_equal("while_statement", name)) {
      return OC_NODE_WHILE_STATEMENT;
    }
    break;
  }

  if (!string_starts_with(name, "gnu_asm")) {
    fprintf(stdout, "WARNING: unknown node named %s\n", name);
  }

  return OC_NODE_ERROR;
}


char* oc_node_tag_to_string(oc_node_tag_t value) {
  static char* array[] = {"OC_NODE_ERROR",
                          "OC_NODE_ABSTRACT_ARRAY_DECLARATOR",
                          "OC_NODE_ABSTRACT_FUNCTION_DECLARATOR",
                          "OC_NODE_ABSTRACT_PARENTHESIZED_DECLARATOR",
                          "OC_NODE_ABSTRACT_POINTER_DECLARATOR",
                          "OC_NODE_ALIGNOF_EXPRESSION",
                          "OC_NODE_ARGUMENT_LIST",
                          "OC_NODE_ARRAY_DECLARATOR",
                          "OC_NODE_ASSIGNMENT_EXPRESSION",
                          "OC_NODE_ATTRIBUTE",
                          "OC_NODE_ATTRIBUTED_DECLARATOR",
                          "OC_NODE_ATTRIBUTED_STATEMENT",
                          "OC_NODE_ATTRIBUTE_DECLARATION",
                          "OC_NODE_ATTRIBUTE_SPECIFIER",
                          "OC_NODE_BINARY_EXPRESSION",
                          "OC_NODE_BITFIELD_CLAUSE",
                          "OC_NODE_BREAK_STATEMENT",
                          "OC_NODE_CALL_EXPRESSION",
                          "OC_NODE_CASE_STATEMENT",
                          "OC_NODE_CAST_EXPRESSION",
                          "OC_NODE_CHARACTER",
                          "OC_NODE_CHAR_LITERAL",
                          "OC_NODE_COMMA_EXPRESSION",
                          "OC_NODE_COMMENT",
                          "OC_NODE_COMPOUND_LITERAL_EXPRESSION",
                          "OC_NODE_COMPOUND_STATEMENT",
                          "OC_NODE_CONCATENATED_STRING",
                          "OC_NODE_CONDITIONAL_EXPRESSION",
                          "OC_NODE_CONTINUE_STATEMENT",
                          "OC_NODE_DECLARATION",
                          "OC_NODE_DECLARATION_LIST",
                          "OC_NODE_DO_STATEMENT",
                          "OC_NODE_ELSE_CLAUSE",
                          "OC_NODE_ENUMERATOR",
                          "OC_NODE_ENUMERATOR_LIST",
                          "OC_NODE_ENUM_SPECIFIER",
                          "OC_NODE_ESCAPE_SEQUENCE",
                          "OC_NODE_EXPRESSION_STATEMENT",
                          "OC_NODE_FALSE",
                          "OC_NODE_FIELD_DECLARATION",
                          "OC_NODE_FIELD_DECLARATION_LIST",
                          "OC_NODE_FIELD_DESIGNATOR",
                          "OC_NODE_FIELD_EXPRESSION",
                          "OC_NODE_FIELD_IDENTIFIER",
                          "OC_NODE_FOR_STATEMENT",
                          "OC_NODE_FUNCTION_DECLARATOR",
                          "OC_NODE_FUNCTION_DEFINITION",
                          "OC_NODE_GENERIC_EXPRESSION",
                          "OC_NODE_GOTO_STATEMENT",
                          "OC_NODE_IDENTIFIER",
                          "OC_NODE_IF_STATEMENT",
                          "OC_NODE_INITIALIZER_LIST",
                          "OC_NODE_INITIALIZER_PAIR",
                          "OC_NODE_INIT_DECLARATOR",
                          "OC_NODE_LABELED_STATEMENT",
                          "OC_NODE_LINKAGE_SPECIFICATION",
                          "OC_NODE_MACRO_TYPE_SPECIFIER",
                          "OC_NODE_MS_DECLSPEC_MODIFIER",
                          "OC_NODE_MS_POINTER_MODIFIER",
                          "OC_NODE_MS_RESTRICT_MODIFIER",
                          "OC_NODE_NULL",
                          "OC_NODE_NUMBER_LITERAL",
                          "OC_NODE_OFFSETOF_EXPRESSION",
                          "OC_NODE_PARAMETER_DECLARATION",
                          "OC_NODE_PARAMETER_LIST",
                          "OC_NODE_PARENTHESIZED_DECLARATOR",
                          "OC_NODE_PARENTHESIZED_EXPRESSION",
                          "OC_NODE_POINTER_DECLARATOR",
                          "OC_NODE_POINTER_EXPRESSION",
                          "OC_NODE_PREPROC_ARG",
                          "OC_NODE_PREPROC_CALL",
                          "OC_NODE_PREPROC_DEF",
                          "OC_NODE_PREPROC_DEFINED",
                          "OC_NODE_PREPROC_DIRECTIVE",
                          "OC_NODE_PREPROC_ELIF",
                          "OC_NODE_PREPROC_ELSE",
                          "OC_NODE_PREPROC_FUNCTION_DEF",
                          "OC_NODE_PREPROC_IF",
                          "OC_NODE_PREPROC_IFDEF",
                          "OC_NODE_PREPROC_INCLUDE",
                          "OC_NODE_PREPROC_PARAMS",
                          "OC_NODE_PRIMITIVE_TYPE",
                          "OC_NODE_RETURN_STATEMENT",
                          "OC_NODE_SIZED_TYPE_SPECIFIER",
                          "OC_NODE_SIZEOF_EXPRESSION",
                          "OC_NODE_STATEMENT_IDENTIFIER",
                          "OC_NODE_STORAGE_CLASS_SPECIFIER",
                          "OC_NODE_STRING_CONTENT",
                          "OC_NODE_STRING_LITERAL",
                          "OC_NODE_STRUCT_SPECIFIER",
                          "OC_NODE_SUBSCRIPT_DESIGNATOR",
                          "OC_NODE_SUBSCRIPT_EXPRESSION",
                          "OC_NODE_SWITCH_STATEMENT",
                          "OC_NODE_SYSTEM_LIB_STRING",
                          "OC_NODE_TRANSLATION_UNIT",
                          "OC_NODE_TRUE",
                          "OC_NODE_TYPE_DEFINITION",
                          "OC_NODE_TYPE_DESCRIPTOR",
                          "OC_NODE_TYPE_IDENTIFIER",
                          "OC_NODE_TYPE_QUALIFIER",
                          "OC_NODE_UNARY_EXPRESSION",
                          "OC_NODE_UNION_SPECIFIER",
                          "OC_NODE_UPDATE_EXPRESSION",
                          "OC_NODE_VARIADIC_PARAMETER",
                          "OC_NODE_WHILE_STATEMENT"};

  if (value >= 0 && value < 105) {
    return array[value];
  } else {
    fatal_error(ERROR_ILLEGAL_ENUM_VALUE);
  }
}

/**
 * @function extract_oc_node_text
 *
 * This function decides when to extract the file contents
 * representing this node (according to ts_node_start_byte and
 * ts_node_end_byte). Generally we only do this for leaf nodes since
 * we intend to modify the tree (at least for the initial C++ and C
 * backends).
 */
void extract_oc_node_text(oc_node_t* oc_node, TSNode ts_node,
                          buffer_t* source_code) {
  if (ts_node_child_count(ts_node) == 0) {
    oc_node->text = buffer_c_substring(source_code, ts_node_start_byte(ts_node),
                                       ts_node_end_byte(ts_node));
  }
}

/* ============================================================ */

__attribute__((warn_unused_result)) buffer_t*
    append_oc_node_tree_impl(buffer_t* buffer, oc_node_t* node, int level);

/**
 * Append the tree representation of an oc_node_t* to the buffer.
 *
 * This will look something like this:
 *
 * OC_NODE_TRANSLATION_UNIT
 *     OC_NODE_PREPROC_INCLUDE
 *         OC_NODE_SYSTEM_LIB_STRING   «<stdio.h>»
 * (etc.)
 *
 * I think I got rid of the s-expression version too quickly but this
 * version is frankly easier to read in part because it (mostly) keeps
 * all parents to a single line, etc.
 *
 * I definitely want to add back in s-expressions and maybe even have
 * a fancy output HTML that is semi-interactive.
 */
__attribute__((warn_unused_result)) buffer_t*
    append_oc_node_tree(buffer_t* buffer, oc_node_t* node) {
  return append_oc_node_tree_impl(buffer, node, 0);
}

__attribute__((warn_unused_result)) buffer_t*
    append_oc_node_tree_impl(buffer_t* buffer, oc_node_t* node, int level) {
  buffer = buffer_append_repeated_byte(buffer, ' ', level * 4);
  buffer = buffer_append_string(buffer, oc_node_tag_to_string(node->tag));
  // TODO(jawilson): line numbers
  if (node->text) {
    buffer = buffer_append_string(buffer, "   ");
    buffer = buffer_append_string(buffer, "\u00AB");
    char* str = string_truncate(node->text, 40, "...");
    buffer = buffer_append_string(buffer, node->text);
    free_bytes(str);
    buffer = buffer_append_string(buffer, "\u00BB");
  }
  buffer = buffer_append_string(buffer, "\n");

  if (node->children) {
    for (int i = 0; i < node->children->length; i++) {
      oc_node_t* child = (oc_node_t*) value_array_get(node->children, i).ptr;
      buffer = append_oc_node_tree_impl(buffer, child, level + 1);
    }
  }

  return buffer;
}

/* ============================================================ */

/**
 * Recursively walks a ts_node and it's children and produces a
 * oc_node_t* from it.
 */
oc_node_t* ts_node_to_oc_node(TSNode ts_node, buffer_t* source_code) {
  oc_node_t* result = malloc_struct(oc_node_t);
  result->tag = ts_node_name_to_tag(ts_node_type(ts_node));
  extract_oc_node_text(result, ts_node, source_code);

  int n_children = ts_node_named_child_count(ts_node);
  result->children = n_children > 0 ? make_value_array(n_children) : NULL;
  for (int i = 0; i < n_children; i++) {
    value_array_add(result->children,
                    ptr_to_value(ts_node_to_oc_node(
                        ts_node_named_child(ts_node, i), source_code)));
  }
  return result;
}

char* oc_tag_prefix_text(oc_node_t* node) {
  switch (node->tag) {
  case OC_NODE_PREPROC_INCLUDE:
    return "#include";
    break;

  case OC_NODE_ELSE_CLAUSE:
    return "else";
    break;

  case OC_NODE_IF_STATEMENT:
    return "if";
    break;

  case OC_NODE_PARENTHESIZED_EXPRESSION:
  case OC_NODE_ARGUMENT_LIST:
  case OC_NODE_PARAMETER_LIST:
    return "(";
    break;

  case OC_NODE_COMPOUND_STATEMENT:
    return "\n{";
    break;

  case OC_NODE_NULL:
    return "NULL";
    break;

  case OC_NODE_STRING_CONTENT:
    return "\"";

  case OC_NODE_POINTER_EXPRESSION:
    return "(*";

  case OC_NODE_COMMENT:
  case OC_NODE_EXPRESSION_STATEMENT:
    return "\n";

  case OC_NODE_SYSTEM_LIB_STRING:
    return "<";

  case OC_NODE_FOR_STATEMENT:
    return "for (";

  case OC_NODE_TRANSLATION_UNIT:
  case OC_NODE_PARAMETER_DECLARATION:
  case OC_NODE_NUMBER_LITERAL:
  case OC_NODE_TYPE_IDENTIFIER:
  case OC_NODE_DECLARATION:
  case OC_NODE_STRING_LITERAL:
  case OC_NODE_FUNCTION_DECLARATOR:
  case OC_NODE_TYPE_DESCRIPTOR:
  case OC_NODE_CALL_EXPRESSION:
  case OC_NODE_INIT_DECLARATOR:
  case OC_NODE_POINTER_DECLARATOR:
  case OC_NODE_IDENTIFIER:
  case OC_NODE_PRIMITIVE_TYPE:
  case OC_NODE_TRUE:
    return "";
    break;
  }
  return string_printf("<%s>", oc_node_tag_to_string(node->tag));
}

char* oc_tag_suffix_text(oc_node_t* node) {
  switch (node->tag) {

  case OC_NODE_POINTER_DECLARATOR:
    return "*";

  case OC_NODE_PARENTHESIZED_EXPRESSION:
  case OC_NODE_ARGUMENT_LIST:
  case OC_NODE_PARAMETER_LIST:
  case OC_NODE_POINTER_EXPRESSION:
    return ")";
    break;

  case OC_NODE_COMPOUND_STATEMENT:
    return "}\n";
    break;

  case OC_NODE_EXPRESSION_STATEMENT:
    return ";\n";
    break;

  case OC_NODE_STRING_CONTENT:
    return "\"";

  case OC_NODE_SYSTEM_LIB_STRING:
    return ">";

  case OC_NODE_PREPROC_INCLUDE:
    return "\n";

    // maybe?
  case OC_NODE_FOR_STATEMENT:
    return ")";

  default:
    break;
  }

  return "";
}

/**
 * Append a textual representation of an oc_node_t* to the buffer.
 */
__attribute__((warn_unused_result)) buffer_t*
    append_oc_node_text(buffer_t* buffer, oc_node_t* node) {

  buffer = buffer_append_string(buffer, oc_tag_prefix_text(node));

  if (node->text != NULL) {
    buffer = buffer_append_string(buffer, "\u00AB");
    buffer = buffer_append_string(buffer, node->text);
    buffer = buffer_append_string(buffer, "\u00BB");
  }

  if (node->children) {
    for (int i = 0; i < node->children->length; i++) {
      buffer = buffer_append_string(buffer, " ");
      oc_node_t* child = (oc_node_t*) value_array_get(node->children, i).ptr;
      buffer = append_oc_node_text(buffer, child);
    }
  }

  buffer = buffer_append_string(buffer, oc_tag_suffix_text(node));
  return buffer;
}
