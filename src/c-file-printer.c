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

/**
 * @function append_parse_node
 *
 * Append the debugging version of a parse node to a printer.
 */
printer_t* append_parse_node(printer_t* printer, parse_node_t* node) {
  switch (node->tag) {

  case PARSE_NODE_ENUM:
    return append_enum_node(printer, to_enum_node(node));

  case PARSE_NODE_STRUCT:
  case PARSE_NODE_UNION:
    return append_struct_node(printer, to_struct_node(node));

  case PARSE_NODE_TYPE:
    return append_type_node(printer, to_type_node(node));

  case PARSE_NODE_LITERAL:
    return append_literal_node(printer, to_literal_node(node));

  case PARSE_NODE_IDENTIFIER:
    return append_identifier_node(printer, to_identifier_node(node));

    /*
  case PARSE_NODE_FUNCTION:
    return append_function_node(printer, to_function_node(node));
    */

  case PARSE_NODE_TYPEDEF:
    return append_typedef_node(printer, to_typedef_node(node));

  case PARSE_NODE_BLOCK:
    return append_block_node(printer, to_block_node(node));

  case PARSE_NODE_BALANCED_CONSTRUCT:
    return append_balanced_construct_node(printer,
                                          to_balanced_construct_node(node));

    // TODO(jawilson): Always assuming this is a library is
    // problematic... We should be able to put that information into
    // the printer itself.
  case PARSE_NODE_VARIABLE_DEFINITION:
    return append_variable_definition_node(
        printer, to_variable_definition_node(node), true);

  case PARSE_NODE_WHILE_STATEMENT:
    return append_while_statement_node(printer, to_while_statement_node(node));

  case PARSE_NODE_FOR_STATEMENT:
    return append_for_statement_node(printer, to_for_statement_node(node));

  case PARSE_NODE_DO_STATEMENT:
    return append_do_statement_node(printer, to_do_statement_node(node));

  case PARSE_NODE_EMPTY_STATEMENT:
    return append_empty_statement_node(printer, to_empty_statement_node(node));

  case PARSE_NODE_IF_STATEMENT:
    return append_if_statement_node(printer, to_if_statement_node(node));

  case PARSE_NODE_RETURN_STATEMENT:
    return append_return_statement_node(printer,
                                        to_return_statement_node(node));

  case PARSE_NODE_BREAK_STATEMENT:
    return append_break_statement_node(printer, to_break_statement_node(node));

  case PARSE_NODE_CONTINUE_STATEMENT:
    return append_continue_statement_node(printer,
                                          to_continue_statement_node(node));

  case PARSE_NODE_LABEL_STATEMENT:
    return append_label_statement_node(printer, to_label_statement_node(node));

  case PARSE_NODE_CASE_LABEL:
    return append_case_label_node(printer, to_case_label_node(node));

  case PARSE_NODE_DEFAULT_LABEL:
    return append_default_label_node(printer, to_default_label_node(node));

  case PARSE_NODE_EXPRESSION_STATEMENT:
    return append_expression_statement_node(printer,
                                            to_expression_statement_node(node));

  case PARSE_NODE_SWITCH_STATEMENT:
    return append_switch_statement_node(printer,
                                        to_switch_statement_node(node));

  case PARSE_NODE_OPERATOR:
    return append_operator_node(printer, to_operator_node(node));

  case PARSE_NODE_CONDITIONAL:
    return append_conditional_node(printer, to_conditional_node(node));

  case PARSE_NODE_CALL:
    return append_call_node(printer, to_call_node(node));

  default:
    break;
  }
  log_fatal("No C file appender for %s", parse_node_type_to_string(node->tag));
  fatal_error(ERROR_ILLEGAL_STATE);
}

/**
 * @function append_c_function_node_prefix
 *
 * Almost a full function prototypes (but the semi-colon is
 * missing). This is used to print out prototypes or before printing
 * out a full function body.
 */
printer_t* append_c_function_node_prefix(printer_t* printer,
                                         function_node_t* node) {

  for (int i = 0; i < node_list_length(node->attributes); i++) {
    append_c_attribute_node(
        printer, to_attribute_node(node_list_get(node->attributes, i)));
    append_string(printer, " ");
  }

  if (node->storage_class_specifier != NULL) {
    append_token(printer, node->storage_class_specifier);
    append_string(printer, " ");
  }

  for (int i = 0; i < token_list_length(node->function_specifiers); i++) {
    append_token(printer, token_list_get(node->function_specifiers, i));
    append_string(printer, " ");
  }

  append_type_node(printer, node->return_type);
  printer_space(printer);
  append_token(printer, node->function_name);
  append_string(printer, "(");

  for (int i = 0; i < node_list_length(node->function_args); i++) {
    if (i > 0) {
      append_string(printer, ", ");
    }
    function_argument_node_t* arg_node
        = to_function_argument_node(node_list_get(node->function_args, i));
    append_c_function_argument_node(printer, arg_node);
  }
  append_string(printer, ")");
  return printer;
}

printer_t* append_c_function_node_prototype(printer_t* printer,
                                            function_node_t* node) {
  append_c_function_node_prefix(printer, node);
  append_string(printer, ";\n");
  return printer;
}

printer_t* append_balanced_construct_node(printer_t* printer,
                                          balanced_construct_node_t* node) {
  uint64_t start = node->start_token->start;
  uint64_t end = node->end_token->end;
  buffer_append_sub_buffer(printer->buffer, start, end,
                           node->start_token->buffer);
  return printer;
}

printer_t* append_c_function_node_and_body(printer_t* printer,
                                           function_node_t* node) {
  if (printer->output_line_directives) {
    append_line_directive(printer, node->function_name);
  }
  append_c_function_node_prefix(printer, node);
  append_parse_node(printer, node->body);
  printer_newline(printer);
  return printer;
}

printer_t* append_c_function_argument_node(printer_t* printer,
                                           function_argument_node_t* node) {
  if (node->is_var_args) {
    append_string(printer, "...");
  } else {
    append_type_node(printer, node->arg_type);
    if (node->arg_name != NULL) {
      printer_space(printer);
      append_token(printer, node->arg_name);
    }
  }
  return printer;
}

/**
 * @function append_type_node
 *
 * Append the C version of a type node to a printer.
 */
printer_t* append_type_node(printer_t* printer, type_node_t* node) {

  if ((node->qualifiers & TYPE_QUALIFIER_CONST) == TYPE_QUALIFIER_CONST) {
    append_string(printer, "const ");
  }
  if ((node->qualifiers & TYPE_QUALIFIER_VOLATILE) == TYPE_QUALIFIER_VOLATILE) {
    append_string(printer, "volatile ");
  }
  if ((node->qualifiers & TYPE_QUALIFIER_RESTRICT) == TYPE_QUALIFIER_RESTRICT) {
    append_string(printer, "restrict ");
  }

  switch (node->type_node_kind) {
  case TYPE_NODE_KIND_POINTER:
    append_type_node(printer, to_type_node(node_list_get(node->type_args, 0)));
    append_string(printer, "*");
    break;

  case TYPE_NODE_KIND_PRIMITIVE_TYPENAME:
  case TYPE_NODE_KIND_TYPENAME:
    if (node->type_name != NULL) {
      append_token(printer, node->type_name);
    }
    break;

  case TYPE_NODE_KIND_TYPE_EXPRESSION:
    if (token_matches(node->type_name, "fn_t")) {
      append_fn_type_node(printer, node);
    } else {
      // I'm curious what happens here. I recently though this was
      // dead without running unit tests.
      //
      // fatal_error(ERROR_ILLEGAL_STATE);
      append_parse_node(printer, node->user_type);
    }
    break;

  case TYPE_NODE_KIND_ARRAY:
    append_string(printer, "typeof(");
    append_type_node(printer, to_type_node(node_list_get(node->type_args, 0)));
    append_string(printer, "[])");
    break;

  case TYPE_NODE_KIND_TYPEOF:
    append_string(printer, "typeof(");
    append_type_node(printer, to_type_node(node->user_type));
    append_string(printer, ")");
    break;

  default:
    log_fatal("type_node_kind is not expected %s",
              type_node_kind_to_string(node->type_node_kind));
    fatal_error(ERROR_ILLEGAL_STATE);
    break;
  }

  return printer;
}

/**
 * @function append_fn_type_node
 *
 * Append the C version of a function pointer. This version assumes
 * that the fn_t macro is available which depends on typeof which is
 * fairly recent C as of 2024.
 */
printer_t* append_fn_type_node(printer_t* printer, type_node_t* node) {
  append_token(printer, node->type_name);
  append_string(printer, "(");
  for (int i = 0; i < node_list_length(node->type_args); i++) {
    if (i > 0) {
      append_string(printer, ", ");
    }
    append_parse_node(printer, node_list_get(node->type_args, i));
  }
  append_string(printer, ")");
  return printer;
}

printer_t* append_c_attribute_node(printer_t* printer, attribute_node_t* node) {
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
  append_c_raw_token_span(printer, node->inner_start_token,
                          node->inner_end_token);
  return printer;
}

//
// Any call to this means that we haven't explicitly "parsed" the
// input yet and are passing a RAW span to the C compiler potentially
// without having completely signed off on it completely (though the
// region will be lexicially consistent with our lexer).
//
printer_t* append_c_raw_token_span(printer_t* printer, token_t* start_token,
                                   token_t* end_token) {
  if (start_token->buffer != end_token->buffer) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  buffer_append_sub_buffer(printer->buffer, start_token->start, end_token->end,
                           start_token->buffer);
  return printer;
}

printer_t* append_enum_node(printer_t* printer, enum_node_t* node) {

  append_string(printer, "enum ");
  if (node->name != NULL) {
    append_token(printer, node->name);
    printer_newline(printer);
  }
  if (node->partial_definition) {
    return printer;
  }

  append_string(printer, "{\n");
  printer_increase_indent(printer);

  for (int i = 0; i < node_list_length(node->elements); i++) {
    printer_indent(printer);
    append_enum_element(printer,
                        to_enum_element_node(node_list_get(node->elements, i)));
    append_string(printer, ",\n");
  }
  append_string(printer, "}");
  printer_decrease_indent(printer);

  return printer;
}

printer_t* append_enum_element(printer_t* printer, enum_element_t* node) {
  append_token(printer, node->name);
  if (node->value_expr != NULL) {
    append_string(printer, " = ");
    append_parse_node(printer, node->value_expr);
  }
  return printer;
}

/**
 * @function append_enum_to_string
 *
 * Adds the source code to a function like input_mode_to_string (from
 * the enum input_mode_t).
 */
printer_t* append_enum_to_string(printer_t* printer, enum_node_t* node,
                                 char* to_string_fn_prefix, char* type_string) {
  append_string(printer, "char* ");
  append_string(printer, to_string_fn_prefix);
  append_string(printer, "_to_string(");
  append_string(printer, type_string);
  append_string(printer, " value) {\n");
  printer_increase_indent(printer);
  printer_indent(printer);
  append_string(printer, "switch (value) {\n");
  printer_indent(printer);

  for (int i = 0; i < node_list_length(node->elements); i++) {
    enum_element_t* element
        = to_enum_element_node(node_list_get(node->elements, i));
    printer_indent(printer);
    append_string(printer, "case ");
    append_token(printer, element->name);
    append_string(printer, ":\n");
    printer_increase_indent(printer);
    printer_indent(printer);
    append_string(printer, "return \"");
    append_token(printer, element->name);
    append_string(printer, "\";\n");
    printer_decrease_indent(printer);
  }

  printer_indent(printer);
  append_string(printer, "default:\n");

  printer_increase_indent(printer);
  printer_indent(printer);
  append_string(printer, "return \"<<unknown-");
  append_string(printer, to_string_fn_prefix);
  append_string(printer, ">>\";\n");
  printer_decrease_indent(printer);
  printer_indent(printer);
  append_string(printer, "}\n");
  printer_decrease_indent(printer);
  append_string(printer, "}\n\n");

  return printer;
}

/**
 * @function append_string_to_enum
 *
 * Adds the source code to a function like string_to_input_mode (from
 * the enum input_mode_t).
 */
printer_t* append_string_to_enum(printer_t* printer, enum_node_t* node,
                                 char* to_string_fn_prefix, char* type_string) {
  append_string(printer, type_string);
  append_string(printer, " string_to_");
  append_string(printer, to_string_fn_prefix);
  append_string(printer, "(char* value) {\n");
  printer_increase_indent(printer);

  for (int i = 0; i < node_list_length(node->elements); i++) {
    enum_element_t* element
        = to_enum_element_node(node_list_get(node->elements, i));
    printer_indent(printer);
    append_string(printer, "if (strcmp(value, \"");
    append_token(printer, element->name);
    append_string(printer, "\") == 0) {\n");
    printer_increase_indent(printer);
    printer_indent(printer);
    append_string(printer, "return ");
    append_token(printer, element->name);
    append_string(printer, ";\n");
    printer_decrease_indent(printer);
    printer_indent(printer);
    append_string(printer, "}\n");
  }
  printer_indent(printer);
  append_string(printer, "return 0;\n");
  printer_decrease_indent(printer);
  append_string(printer, "}\n\n");

  return printer;
}

printer_t* append_field_node(printer_t* printer, field_node_t* node) {
  append_type_node(printer, node->type);
  append_string(printer, " ");
  if (node->name != NULL) {
    append_token(printer, node->name);
  }
  if (node->suffixes) {
    for (int i = 0; i < node->suffixes->length; i++) {
      append_parse_node(printer, value_array_get_ptr(node->suffixes, i,
                                                     typeof(parse_node_t*)));
    }
  }
  // TODO(jawilson): If the field has a bit-width, we need to print
  // that out as well.
  return printer;
}

/**
 * @function append_struct_node
 */
printer_t* append_struct_node(printer_t* printer, struct_node_t* node) {
  append_string(printer, node->tag == PARSE_NODE_UNION ? "union " : "struct ");
  if (node->name != NULL) {
    append_token(printer, node->name);
  }

  if (!node->partial_definition) {
    append_string(printer, " {\n");
    printer_increase_indent(printer);
    for (int i = 0; i < node_list_length(node->fields); i++) {
      printer_indent(printer);
      append_field_node(printer, to_field_node(node_list_get(node->fields, i)));
      append_string(printer, ";\n");
    }
    printer_decrease_indent(printer);
    append_string(printer, "}");
  }

  return printer;
}

/**
 * @function append_typedef_node
 */
printer_t* append_typedef_node(printer_t* printer, typedef_node_t* node) {
  append_string(printer, "typedef ");
  append_type_node(printer, node->type_node);
  append_string(printer, " ");
  append_token(printer, node->name);
  append_string(printer, ";\n");
  return printer;
}

/**
 * @function append_cpp_include_node
 */
printer_t* append_cpp_include_node(printer_t* printer,
                                   cpp_include_node_t* node) {
  append_string(printer, node->text);
  return printer;
}

/**
 * @function append_cpp_define_node
 */
printer_t* append_cpp_define_node(printer_t* printer, cpp_define_node_t* node) {
  append_string(printer, node->text);
  return printer;
}

/**
 * @function append_variable_definition_node
 */
printer_t* append_variable_definition_node(printer_t* printer,
                                           variable_definition_node_t* node,
                                           boolean_t is_library) {
  printer_indent(printer);
  boolean_t is_header_file = !is_library;
  if (node->storage_class_specifier != NULL) {
    append_token(printer, node->storage_class_specifier);
    append_string(printer, " ");
  } else if (is_header_file) {
    append_string(printer, "extern ");
  }

  append_type_node(printer, node->type);
  append_string(printer, " ");
  append_token(printer, node->name);
  if (node->suffixes) {
    for (int i = 0; i < node->suffixes->length; i++) {
      append_parse_node(printer, value_array_get_ptr(node->suffixes, i,
                                                     typeof(parse_node_t*)));
    }
  }
  if (is_library && node->value != NULL) {
    append_string(printer, " = ");
    append_parse_node(printer, node->value);
  }
  append_string(printer, ";\n");
  return printer;
}

/**
 * @function append_literal_node
 */
printer_t* append_literal_node(printer_t* printer, literal_node_t* node) {
  if (node->token != NULL) {
    append_token(printer, node->token);
  } else if (node->initializer_node != NULL) {
    if (node->initializer_type != NULL) {
      append_string(printer, "((");
      append_parse_node(printer, node->initializer_type);
      append_string(printer, ") ");
    }
    append_balanced_construct_node(
        printer, to_balanced_construct_node(node->initializer_node));
    if (node->initializer_type != NULL) {
      append_string(printer, ")");
    }
  } else if (node->tokens != NULL && node->tokens->length > 0) {
    for (uint64_t i = 0; i < node->tokens->length; i++) {
      if (i > 0) {
        append_string(printer, " ");
      }
      token_t* token = value_array_get_ptr(node->tokens, i, typeof(token_t*));
      append_token(printer, token);
    }
  } else {
    append_string(printer, "FIXME");
  }
  return printer;
}

/**
 * @function append_literal_node
 */
printer_t* append_identifier_node(printer_t* printer, identifier_node_t* node) {
  if (node->token == NULL) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  if (token_matches(node->token, "nullptr")) {
    append_string(printer, "((void *)0)");
  } else {
    append_token(printer, node->token);
  }
  return printer;
}

/* ====================================================================== *
 * Statements                                                             *
 * ====================================================================== */

/**
 * @function append_empty_statement_node
 */
printer_t* append_empty_statement_node(printer_t* printer,
                                       empty_statement_node_t* node) {
  printer_indent(printer);
  append_string(printer, ";\n");
  return printer;
}

/**
 * @function append_break_statement_node
 */
printer_t* append_break_statement_node(printer_t* printer,
                                       break_statement_node_t* node) {
  printer_indent(printer);
  append_string(printer, "break;\n");
  return printer;
}

/**
 * @function append_continue_statement_node
 */
printer_t* append_continue_statement_node(printer_t* printer,
                                          continue_statement_node_t* node) {
  printer_indent(printer);
  append_string(printer, "continue;\n");
  return printer;
}

/**
 * @function append_label_statement_node
 */
printer_t* append_label_statement_node(printer_t* printer,
                                       label_statement_node_t* node) {
  printer_indent(printer);
  append_token(printer, node->label);
  append_string(printer, ":\n");
  return printer;
}

/**
 * @function append_case_label_node
 */
printer_t* append_case_label_node(printer_t* printer, case_label_node_t* node) {
  printer_indent(printer);
  append_string(printer, "case ");
  append_parse_node(printer, node->expression);
  append_string(printer, ":\n");
  return printer;
}

/**
 * @function append_label_statement_node
 */
printer_t* append_default_label_node(printer_t* printer,
                                     default_label_node_t* node) {
  printer_indent(printer);
  append_string(printer, "default:\n");
  return printer;
}

/**
 * @function append_expression_statement_node
 */
printer_t* append_expression_statement_node(printer_t* printer,
                                            expression_statement_node_t* node) {
  printer_indent(printer);
  append_parse_node(printer, node->expression);
  append_string(printer, ";\n");
  return printer;
}

/**
 * @function append_block_node
 */
printer_t* append_block_node(printer_t* printer, block_node_t* node) {
  printer_indent(printer);
  append_string(printer, "{\n");
  printer_increase_indent(printer);
  uint64_t length = node_list_length(node->statements);
  for (uint64_t i = 0; i < length; i++) {
    append_parse_node(printer, node_list_get(node->statements, i));
  }
  printer_decrease_indent(printer);
  printer_indent(printer);
  append_string(printer, "}\n");
  return printer;
}

/**
 * @function append_if_statement_node
 */
printer_t* append_if_statement_node(printer_t* printer,
                                    if_statement_node_t* node) {
  printer_indent(printer);
  append_string(printer, "if (");
  append_parse_node(printer, node->if_condition);
  append_string(printer, ")\n");
  append_parse_node(printer, node->if_true);
  if (node->if_else) {
    printer_indent(printer);
    append_string(printer, "else\n");
    append_parse_node(printer, node->if_else);
  }
  return printer;
}

/**
 * @function append_while_statement_node
 */
printer_t* append_while_statement_node(printer_t* printer,
                                       while_statement_node_t* node) {
  printer_indent(printer);
  append_string(printer, "while (");
  append_parse_node(printer, node->condition);
  append_string(printer, ")\n");
  append_parse_node(printer, node->body);
  return printer;
}

/**
 * @function append_switch_statement_node
 */
printer_t* append_switch_statement_node(printer_t* printer,
                                        switch_statement_node_t* node) {
  printer_indent(printer);
  append_string(printer, "switch (");
  append_parse_node(printer, node->expression);
  append_string(printer, ")\n");
  append_parse_node(printer, node->block);
  return printer;
}

/**
 * @function append_for_statement_node
 */
printer_t* append_for_statement_node(printer_t* printer,
                                     for_statement_node_t* node) {
  printer_indent(printer);

  append_string(printer, "for (\n");
  printer_increase_indent(printer);

  // for_init
  if (node->for_init != NULL) {
    append_parse_node(printer, node->for_init);
  } else {
    printer_indent(printer);
    append_string(printer, ";\n");
  }

  // for_test
  printer_indent(printer);
  if (node->for_test != NULL) {
    append_parse_node(printer, node->for_test);
  }
  append_string(printer, ";");
  printer_newline(printer);

  // for_increment
  printer_indent(printer);
  if (node->for_increment != NULL) {
    append_parse_node(printer, node->for_increment);
  }
  append_string(printer, ")\n");
  printer_decrease_indent(printer);
  append_parse_node(printer, node->for_body);
  return printer;
}

/**
 * @function append_do_statement_node
 */
printer_t* append_do_statement_node(printer_t* printer,
                                    do_statement_node_t* node) {
  printer_indent(printer);
  append_string(printer, "do");
  append_parse_node(printer, node->body);
  append_string(printer, "while (");
  append_parse_node(printer, node->condition);
  append_string(printer, ");\n");
  return printer;
}


/**
 * @function append_return_statement_node
 */
printer_t* append_return_statement_node(printer_t* printer,
                                        return_statement_node_t* node) {
  printer_indent(printer);
  append_string(printer, "return");
  if (node->expression != NULL) {
    append_string(printer, " ");
    append_parse_node(printer, node->expression);
  }
  append_string(printer, ";\n");
  return printer;
}

/**
 * @function append_operator_node
 */
printer_t* append_operator_node(printer_t* printer, operator_node_t* node) {
  if (token_matches(node->operator, "cast")) {
    append_string(printer, "(/*CAST*/(");
    append_parse_node(printer, node->left);
    append_string(printer, ") ");
    append_parse_node(printer, node->right);
    append_string(printer, ")");
    return printer;
  }

  if (token_matches(node->operator, "block_expr")) {
    return append_parse_node(printer, node->left);
  }

  if (token_matches(node->operator, "typeof")) {
    // TODO(jawilson): make sure left is actually a type.
    append_string(printer, "typeof(");
    append_parse_node(printer, node->left);
    append_string(printer, ")");
    return printer;
  }

  append_string(printer, "(");
  if (node->left != NULL) {
    append_parse_node(printer, node->left);
  }
  append_token(printer, node->operator);
  if (token_matches(node->operator, "sizeof")) {
    append_string(printer, "(");
  }
  if (node->right != NULL) {
    append_parse_node(printer, node->right);
  }
  if (token_matches(node->operator, "sizeof")) {
    append_string(printer, ")");
  } else if (token_matches(node->operator, "[")) {
    append_string(printer, "]");
  }
  append_string(printer, ")");
  return printer;
}

/**
 * @function append_conditional_node
 */
printer_t* append_conditional_node(printer_t* printer,
                                   conditional_node_t* node) {
  append_string(printer, "(");
  if (node->condition != NULL) {
    append_parse_node(printer, node->condition);
  }
  append_string(printer, " ? ");
  if (node->expr_if_true != NULL) {
    append_parse_node(printer, node->expr_if_true);
  }
  append_string(printer, " : ");
  if (node->expr_if_false != NULL) {
    append_parse_node(printer, node->expr_if_false);
  }
  append_string(printer, ")");
  return printer;
}

/**
 * @function append_call_node
 */
printer_t* append_call_node(printer_t* printer, call_node_t* node) {
  append_parse_node(printer, node->function);
  append_string(printer, "(");
  for (int i = 0; i < node_list_length(node->args); i++) {
    if (i > 0) {
      append_string(printer, ", ");
    }
    append_parse_node(printer, node_list_get(node->args, i));
  }
  append_string(printer, ")");
  return printer;
}

/* ====================================================================== */

// This doesn't really belong here...

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

printer_t* append_line_directive(printer_t* printer, token_t* token) {
  buffer_printf(printer->buffer, "\n# %d \"%s\"\n", token->line_number, "fixme.c");
  return printer;
}
