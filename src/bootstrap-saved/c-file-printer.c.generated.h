/* Automatically extracted prototypes from c-file-printer.c */

printer_t* append_parse_node(printer_t* printer, parse_node_t* node);

printer_t* append_c_function_node_prefix(printer_t* printer, function_node_t* node);

printer_t* append_c_function_node_prototype(printer_t* printer, function_node_t* node);

printer_t* append_balanced_construct_node(printer_t* printer, balanced_construct_node_t* node);

printer_t* append_c_function_node_and_body(printer_t* printer, function_node_t* node);

printer_t* append_c_function_argument_node(printer_t* printer, function_argument_node_t* node);

printer_t* append_c_type_node(printer_t* printer, type_node_t* node);

printer_t* append_c_attribute_node(printer_t* printer, attribute_node_t* node);

printer_t* append_c_raw_token_span(printer_t* printer, token_t* start_token, token_t* end_token);

printer_t* append_enum_node(printer_t* printer, enum_node_t* node);

printer_t* append_enum_element(printer_t* printer, enum_element_t* node);

printer_t* append_enum_to_string(printer_t* printer, enum_node_t* node, char* to_string_fn_prefix, char* type_string);

printer_t* append_string_to_enum(printer_t* printer, enum_node_t* node, char* to_string_fn_prefix, char* type_string);

printer_t* append_field_node(printer_t* printer, field_node_t* node);

printer_t* append_struct_node(printer_t* printer, struct_node_t* node);

printer_t* append_typedef_node(printer_t* printer, typedef_node_t* node);

printer_t* append_cpp_include_node(printer_t* printer, cpp_include_node_t* node);

printer_t* append_cpp_define_node(printer_t* printer, cpp_define_node_t* node);

printer_t* append_variable_definition_node(printer_t* printer, variable_definition_node_t* node, boolean_t is_library);

printer_t* append_literal_node(printer_t* printer, literal_node_t* node);

printer_t* append_identifier_node(printer_t* printer, identifier_node_t* node);

printer_t* append_empty_statement_node(printer_t* printer, empty_statement_node_t* node);

printer_t* append_break_statement_node(printer_t* printer, break_statement_node_t* node);

printer_t* append_continue_statement_node(printer_t* printer, continue_statement_node_t* node);

printer_t* append_label_statement_node(printer_t* printer, label_statement_node_t* node);

printer_t* append_case_label_node(printer_t* printer, case_label_node_t* node);

printer_t* append_default_label_node(printer_t* printer, default_label_node_t* node);

printer_t* append_expression_statement_node(printer_t* printer, expression_statement_node_t* node);

printer_t* append_block_node(printer_t* printer, block_node_t* node);

printer_t* append_if_statement_node(printer_t* printer, if_statement_node_t* node);

printer_t* append_while_statement_node(printer_t* printer, while_statement_node_t* node);

printer_t* append_switch_statement_node(printer_t* printer, switch_statement_node_t* node);

printer_t* append_for_statement_node(printer_t* printer, for_statement_node_t* node);

printer_t* append_do_statement_node(printer_t* printer, do_statement_node_t* node);

printer_t* append_return_statement_node(printer_t* printer, return_statement_node_t* node);

buffer_t* buffer_append_enum_metadata(buffer_t* buffer, enum_node_t* node, char* fn_prefix, char* type_string);

