/* Automatically extracted prototypes from debug-printer.c */

void buffer_append_dbg_parse_node(cdl_printer_t* printer, parse_node_t* node);

void buffer_append_dbg_node_list(cdl_printer_t* printer, node_list_t list);

void buffer_append_dbg_tokens(cdl_printer_t* printer, value_array_t* tokens, char* field_name);

void buffer_append_dbg_declarations(cdl_printer_t* printer, declarations_node_t* node);

void buffer_append_dbg_enum(cdl_printer_t* printer, enum_node_t* node);

void buffer_append_dbg_struct_node(cdl_printer_t* printer, struct_node_t* node);

void buffer_append_dbg_enum_element(cdl_printer_t* printer, enum_element_t* node);

void buffer_append_dbg_field_node(cdl_printer_t* printer, field_node_t* node);

void buffer_append_dbg_type_node(cdl_printer_t* printer, type_node_t* node);

void buffer_append_dbg_literal_node(cdl_printer_t* printer, literal_node_t* node);

void buffer_append_dbg_function_node(cdl_printer_t* printer, function_node_t* node);

void buffer_append_dbg_function_argument_node(cdl_printer_t* printer, function_argument_node_t* node);

void buffer_append_dbg_balanced_construct_node(cdl_printer_t* printer, balanced_construct_node_t* node);

void buffer_append_dbg_typedef_node(cdl_printer_t* printer, typedef_node_t* node);

void buffer_append_dbg_variable_definition_node(cdl_printer_t* printer, variable_definition_node_t* node);

void buffer_append_dbg_attribute_node(cdl_printer_t* printer, attribute_node_t* node);

void buffer_append_dbg_identifier_node(cdl_printer_t* printer, identifier_node_t* node);

void buffer_append_dbg_operator_node(cdl_printer_t* printer, operator_node_t* node);

void buffer_append_dbg_break_statement_node(cdl_printer_t* printer, break_statement_node_t* node);

void debug_append_tokens(buffer_t* buffer, value_array_t* tokens);

