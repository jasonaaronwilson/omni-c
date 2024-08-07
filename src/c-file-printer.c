#line 2 "c-file-printer.c"
#ifndef _C_FILE_PRINTER_H_
#define _C_FILE_PRINTER_H_

#include "declaration-parser.h"
#include "lexer.h"
#include "parser.h"
#include <c-armyknife-lib.h>
#include <ctype.h>

/**
 * @file c-file-printer.c
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

#include "c-file-printer.c.generated.h"

#endif /* _C_FILE_PRINTER_H_ */

/**
 * @function buffer_append_dbg_parse_node
 *
 * Append the debugging version of a parse node to a buffer.
 */
buffer_t* buffer_append_parse_node(buffer_t* buffer, parse_node_t* node) {
  switch (node->tag) {

    /*
  case PARSE_NODE_DECLARATIONS:
    return buffer_append_dbg_declarations(buffer, to_declarations_node(node),
                                          indention_level);
    */

  case PARSE_NODE_ENUM:
    return buffer_append_enum_node(buffer, to_enum_node(node));

    /*
  case PARSE_NODE_ENUM_ELEMENT:
    return buffer_append_dbg_enum_element(buffer, to_enum_element_node(node),
                                          indention_level);
    */

  case PARSE_NODE_STRUCT:
    return buffer_append_struct_node(buffer, to_struct_node(node));

    /*
  case PARSE_NODE_FIELD:
    return buffer_append_dbg_field_node(buffer, to_field_node(node),
                                        indention_level);

  case PARSE_NODE_TYPE:
    return buffer_append_dbg_type_node(buffer, to_type_node(node),
                                       indention_level);
    */

  case PARSE_NODE_LITERAL:
    return buffer_append_literal_node(buffer, to_literal_node(node));

    /*

  case PARSE_NODE_FUNCTION:
    return buffer_append_dbg_function_node(buffer, to_function_node(node),
                                           indention_level);

  case PARSE_NODE_FUNCTION_ARGUMENT:
    return buffer_append_dbg_function_argument_node(
        buffer, to_function_argument_node(node), indention_level);

  case PARSE_NODE_TYPEDEF:
    return buffer_append_dbg_typedef_node(buffer, to_typedef_node(node),
                                          indention_level);
    */

  case PARSE_NODE_FUNCTION_BODY:
    return buffer_append_function_body_node(buffer,
                                            to_function_body_node(node));

    // Always assuming this is a library is problematic...
  case PARSE_NODE_GLOBAL_VARIABLE_DEFINITION:
    return buffer_append_global_variable_node(
        buffer, to_global_variable_node(node), true);

    /*
  case PARSE_NODE_ATTRIBUTE:
    return buffer_append_dbg_attribute_node(buffer, to_attribute_node(node),
                                            indention_level);
    */

  default:
    break;
  }
  fatal_error(ERROR_ILLEGAL_STATE);
}

/**
 * Not a full function prototypes since the semi-colon is missing but
 * otherwise the same.
 */
buffer_t* buffer_append_c_function_node_prefix(buffer_t* buffer,
                                               function_node_t* node) {

  for (int i = 0; i < node_list_length(node->attributes); i++) {
    buffer_append_c_attribute_node(
        buffer, to_attribute_node(node_list_get(node->attributes, i)));
    buffer_printf(buffer, " ");
  }

  if (node->storage_class_specifier != NULL) {
    buffer_append_token_string(buffer, node->storage_class_specifier);
    buffer_append_string(buffer, " ");
  }

  if (node->function_specifier != NULL) {
    buffer_append_token_string(buffer, node->function_specifier);
    buffer_append_string(buffer, " ");
  }

  buffer_append_c_type_node(buffer, node->return_type);
  buffer_printf(buffer, " %s(", token_to_string(node->function_name));

  for (int i = 0; i < node_list_length(node->function_args); i++) {
    if (i > 0) {
      buffer_printf(buffer, ", ");
    }
    function_argument_node_t* arg_node
        = to_function_argument_node(node_list_get(node->function_args, i));
    buffer_append_c_function_argument_node(buffer, arg_node);
  }
  buffer_append_string(buffer, ")");
  return buffer;
}

buffer_t* buffer_append_c_function_node_prototype(buffer_t* buffer,
                                                  function_node_t* node) {
  buffer_append_c_function_node_prefix(buffer, node);
  buffer_printf(buffer, ";\n");
  return buffer;
}

buffer_t* buffer_append_function_body_node(buffer_t* buffer,
                                           function_body_node_t* node) {
  uint64_t start = node->open_brace_token->start;
  uint64_t end = node->close_brace_token->end;
  buffer_append_sub_buffer(buffer, start, end, node->open_brace_token->buffer);
  return buffer;
}

buffer_t* buffer_append_c_function_node_and_body(buffer_t* buffer,
                                                 function_node_t* node) {
  buffer_append_c_function_node_prefix(buffer, node);
  buffer_append_function_body_node(buffer, node->body);
  buffer_append_string(buffer, "\n");
  return buffer;
}

buffer_t*
    buffer_append_c_function_argument_node(buffer_t* buffer,
                                           function_argument_node_t* node) {
  buffer_append_c_type_node(buffer, node->arg_type);
  if (node->arg_name != NULL) {
    buffer_printf(buffer, " %s", token_to_string(node->arg_name));
  }
  return buffer;
}

buffer_t* buffer_append_c_type_node(buffer_t* buffer, type_node_t* node) {

  switch (node->type_node_kind) {
  case TYPE_NODE_KIND_POINTER:
    buffer_append_c_type_node(buffer,
                              to_type_node(node_list_get(node->type_args, 0)));
    buffer_printf(buffer, "*");
    break;

  case TYPE_NODE_KIND_PRIMITIVE_TYPENAME:
  case TYPE_NODE_KIND_TYPENAME:
    if (node->type_name != NULL) {
      buffer_append_token_string(buffer, node->type_name);
    }
    break;

  case TYPE_NODE_KIND_TYPE_EXPRESSION:
    buffer_append_parse_node(buffer, node->user_type);
    break;

  default:
    log_fatal("type_node_kind is not expected.");
    fatal_error(ERROR_ILLEGAL_STATE);
    break;
  }

  return buffer;
}

buffer_t* buffer_append_c_attribute_node(buffer_t* buffer,
                                         attribute_node_t* node) {
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
  buffer_append_c_raw_token_span(buffer, node->inner_start_token,
                                 node->inner_end_token);
  return buffer;
}

//
// Any call to this means that we haven't explicitly "parsed" the
// input yet and are passing a RAW span to the C compiler potentially
// without having completely signed off on it completely (though the
// region will be lexicially consistent with our lexer).
//
buffer_t* buffer_append_c_raw_token_span(buffer_t* buffer, token_t* start_token,
                                         token_t* end_token) {
  if (start_token->buffer != end_token->buffer) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  buffer_append_sub_buffer(buffer, start_token->start, end_token->end,
                           start_token->buffer);
  return buffer;
}

buffer_t* buffer_append_enum_node(buffer_t* buffer, enum_node_t* node) {

  buffer_printf(buffer, "enum ");
  if (node->name != NULL) {
    buffer_printf(buffer, "%s\n", token_to_string(node->name));
  }
  if (node->partial_definition) {
    return buffer;
  }

  buffer_printf(buffer, "{\n");

  for (int i = 0; i < node_list_length(node->elements); i++) {
    buffer_printf(buffer, "    ");
    buffer_append_enum_element(
        buffer, to_enum_element_node(node_list_get(node->elements, i)));
    buffer_printf(buffer, ",\n");
  }
  buffer_printf(buffer, "}");

  return buffer;
}

buffer_t* buffer_append_enum_element(buffer_t* buffer, enum_element_t* node) {

  buffer_append_token_string(buffer, node->name);
  if (node->value != NULL) {
    buffer_printf(buffer, " = ");
    buffer_append_token_string(buffer, node->value);
  }

  return buffer;
}

/**
 * @function buffer_append_enum_to_string
 *
 * Adds the source code to a function like input_mode_to_string (from
 * the enum input_mode_t).
 */
buffer_t* buffer_append_enum_to_string(buffer_t* buffer, enum_node_t* node,
                                       char* to_string_fn_prefix,
                                       char* type_string) {
  buffer_printf(buffer, "char* %s_to_string(%s value) {\n", to_string_fn_prefix,
                type_string);
  buffer_printf(buffer, "  switch (value) {\n");

  for (int i = 0; i < node_list_length(node->elements); i++) {
    enum_element_t* element
        = to_enum_element_node(node_list_get(node->elements, i));
    buffer_printf(buffer, "    case ");
    buffer_append_token_string(buffer, element->name);
    buffer_printf(buffer, ":\n      return \"");
    buffer_append_token_string(buffer, element->name);
    buffer_printf(buffer, "\";\n");
  }
  buffer_printf(buffer, "    default:\n");
  buffer_printf(buffer, "      return \"<<unknown-%s>>\";\n",
                to_string_fn_prefix);

  buffer_printf(buffer, "  }\n");
  buffer_printf(buffer, "}\n\n");

  return buffer;
}

/**
 * @function buffer_append_string_to_enum
 *
 * Adds the source code to a function like string_to_input_mode (from
 * the enum input_mode_t).
 */
buffer_t* buffer_append_string_to_enum(buffer_t* buffer, enum_node_t* node,
                                       char* to_string_fn_prefix,
                                       char* type_string) {
  buffer_printf(buffer, "%s string_to_%s(char* value) {\n", type_string,
                to_string_fn_prefix);

  for (int i = 0; i < node_list_length(node->elements); i++) {
    enum_element_t* element
        = to_enum_element_node(node_list_get(node->elements, i));
    buffer_printf(buffer, "  if (strcmp(value, \"");
    buffer_append_token_string(buffer, element->name);
    buffer_printf(buffer, "\") == 0) {");
    buffer_printf(buffer, "\n    return ");
    buffer_append_token_string(buffer, element->name);
    buffer_printf(buffer, ";\n  }\n");
  }
  buffer_printf(buffer, "  return 0;\n");
  buffer_printf(buffer, "}\n\n");

  return buffer;
}

buffer_t* buffer_append_field_node(buffer_t* buffer, field_node_t* node) {
  buffer_append_c_type_node(buffer, node->type);
  buffer_printf(buffer, " ");
  buffer_append_token_string(buffer, node->name);
  // TODO(jawilson): bits...
  return buffer;
}

/**
 * @function buffer_append_struct_node
 */
buffer_t* buffer_append_struct_node(buffer_t* buffer, struct_node_t* node) {

  buffer_append_string(buffer, "struct ");
  if (node->name != NULL) {
    buffer_printf(buffer, "%s", token_to_string(node->name));
  }

  if (!node->partial_definition) {
    buffer_printf(buffer, " {\n");
    for (int i = 0; i < node_list_length(node->fields); i++) {
      buffer_printf(buffer, "    ");
      buffer_append_field_node(buffer,
                               to_field_node(node_list_get(node->fields, i)));
      buffer_printf(buffer, ";\n");
    }
    buffer_printf(buffer, "}");
  }

  return buffer;
}

/**
 * @function buffer_append_typedef_node
 */
buffer_t* buffer_append_typedef_node(buffer_t* buffer, typedef_node_t* node) {
  buffer_append_string(buffer, "typedef ");
  buffer_append_c_type_node(buffer, node->type_node);
  buffer_append_string(buffer, " ");
  buffer_printf(buffer, "%s", token_to_string(node->name));
  buffer_append_string(buffer, ";\n");
  return buffer;
}

/**
 * @function buffer_append_cpp_include_node
 */
buffer_t* buffer_append_cpp_include_node(buffer_t* buffer,
                                         cpp_include_node_t* node) {
  buffer_append_string(buffer, node->text);
  return buffer;
}

/**
 * @function buffer_append_cpp_define_node
 */
buffer_t* buffer_append_cpp_define_node(buffer_t* buffer,
                                        cpp_define_node_t* node) {
  buffer_append_string(buffer, node->text);
  return buffer;
}

/**
 * @function buffer_append_global_variable_node
 */
buffer_t* buffer_append_global_variable_node(buffer_t* buffer,
                                             global_variable_node_t* node,
                                             boolean_t is_library) {
  boolean_t is_header_file = !is_library;
  if (node->storage_class_specifier != NULL) {
    buffer_append_token_string(buffer, node->storage_class_specifier);
    buffer_append_string(buffer, " ");
  } else if (is_header_file) {
    buffer_append_string(buffer, "extern ");
  }

  buffer_append_c_type_node(buffer, node->type);
  buffer_append_string(buffer, " ");
  buffer_append_token_string(buffer, node->name);
  if (node->suffixes) {
    for (int i = 0; i < node->suffixes->length; i++) {
      buffer_append_parse_node(
          buffer, value_array_get_ptr(node->suffixes, i, parse_node_t*));
    }
  }
  if (is_library && node->value != NULL) {
    buffer_append_string(buffer, " = ");
    buffer_append_parse_node(buffer, node->value);
  }
  buffer_append_string(buffer, ";");
  return buffer;
}

/**
 * @function buffer_append_literal_node
 */
buffer_t* buffer_append_literal_node(buffer_t* buffer, literal_node_t* node) {
  if (node->token != NULL) {
    buffer_append_token_string(buffer, node->token);
  } else if (node->initializer_node != NULL) {
    buffer_append_function_body_node(buffer, node->initializer_node);
  } else if (node->tokens != NULL && node->tokens->length > 0) {
    for (uint64_t i = 0; i < node->tokens->length; i++) {
      buffer_append_string(buffer, " ");
      token_t* token = value_array_get_ptr(node->tokens, i, token_t*);
      buffer_append_token_string(buffer, token);
    }
  } else {
    buffer_append_string(buffer, "FIXME");
  }
  return buffer;
}

/* ====================================================================== */

buffer_t* buffer_append_enum_metadata(buffer_t* buffer, enum_node_t* node,
                                      char* fn_prefix, char* type_string) {
  // clang-format off
  static char* code_template =
    "enum_metadata_t* ${fn_prefix}_metadata() {\n"
    "${element_constructions}"
    "    static enum_metadata_t enum_metadata_result = (enum_metadata_t) {\n"
    "        .name = \"${enum_name}\",\n"
    "        .elements = ${previous_var_address}\n"
    "    };\n"
    "    return &enum_metadata_result;\n"
    "}\n\n";

  static char* field_template =
    "    static enum_element_metadata_t ${var_id} = (enum_element_metadata_t) {\n"
    "        .next = ${previous_var_address},\n"
    "        .name = \"${element_name}\",\n"
    "        .value = ${element_name}\n"
    "    };\n";
  // clang-format on

  buffer_t* element_constructions = make_buffer(128);
  buffer_t* buf = make_buffer(128);

  char* previous_var_address = "NULL";

  // TODO(jawilson): do in reverse order though no one should
  // technically care...
  for (int i = 0; i < node_list_length(node->elements); i++) {
    enum_element_t* element
        = to_enum_element_node(node_list_get(node->elements, i));
    char* var_id = string_printf("var_%d", i);
    char* element_name = token_to_string(element->name);

    buffer_clear(buf);
    buffer_append_string(buf, field_template);
    buffer_replace_all(buf, "${var_id}", var_id);
    buffer_replace_all(buf, "${previous_var_address}", previous_var_address);
    buffer_replace_all(buf, "${element_name}", element_name);

    buffer_append_buffer(element_constructions, buf);
    previous_var_address = string_printf("&%s", var_id);
  }

  buffer_t* code = buffer_append_string(make_buffer(128), code_template);
  buffer_replace_all(code, "${fn_prefix}", fn_prefix);
  buffer_replace_all(code, "${enum_name}", type_string);
  buffer_replace_all(code, "${previous_var_address}", previous_var_address);
  buffer_replace_all(code, "${element_constructions}",
                     buffer_to_c_string(element_constructions));

  return buffer_append_buffer(buffer, code);
}
