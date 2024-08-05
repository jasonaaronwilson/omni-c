/* Automatically extracted prototypes from debug-printer.c */

buffer_t* buffer_append_dbg_parse_node(buffer_t* buffer, parse_node_t* node, int indention_level);

buffer_t* buffer_append_dbg_node_list(buffer_t* buffer, node_list_t list, char* field_name, int indention_level);

buffer_t* buffer_append_dbg_tokens(buffer_t* buffer, value_array_t* tokens, char* field_name, int indention_level);

buffer_t* buffer_append_dbg_declarations(buffer_t* buffer, declarations_node_t* node, int indention_level);

buffer_t* buffer_append_dbg_enum(buffer_t* buffer, enum_node_t* node, int indention_level);

buffer_t* buffer_append_dbg_struct_node(buffer_t* buffer, struct_node_t* node, int indention_level);

buffer_t* buffer_append_dbg_enum_element(buffer_t* buffer, enum_element_t* node, int indention_level);

buffer_t* buffer_append_dbg_field_node(buffer_t* buffer, field_node_t* node, int indention_level);

buffer_t* buffer_append_dbg_type_node(buffer_t* buffer, type_node_t* node, int indention_level);

buffer_t* buffer_append_dbg_literal_node(buffer_t* buffer, literal_node_t* node, int indention_level);

buffer_t* buffer_append_dbg_function_node(buffer_t* buffer, function_node_t* node, int indention_level);

buffer_t* buffer_append_dbg_function_argument_node(buffer_t* buffer, function_argument_node_t* node, int indention_level);

buffer_t* buffer_append_dbg_function_body_node(buffer_t* buffer, function_body_node_t* node, int indention_level);

buffer_t* buffer_append_dbg_typedef_node(buffer_t* buffer, typedef_node_t* node, int indention_level);

buffer_t* buffer_append_dbg_global_variable_node(buffer_t* buffer, global_variable_node_t* node, int indention_level);

buffer_t* buffer_append_dbg_attribute_node(buffer_t* buffer, attribute_node_t* node, int indention_level);

void debug_append_tokens(buffer_t* buffer, value_array_t* tokens);

buffer_t* buffer_append_dbg_identifier_node(buffer_t* buffer, identifier_node_t* node, int indention_level);

buffer_t* buffer_append_dbg_operator_node(buffer_t* buffer, operator_node_t* node, int indention_level);

buffer_t* buffer_append_dbg_break_statement_node(buffer_t* buffer, break_statement_node_t* node, int indention_level);

