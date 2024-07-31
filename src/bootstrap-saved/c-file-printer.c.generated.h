/* Automatically extracted prototypes from header-file-printer.c */

buffer_t* buffer_append_parse_node(buffer_t* buffer, parse_node_t* node);

buffer_t* buffer_append_c_function_node_prefix(buffer_t* buffer, function_node_t* node);

buffer_t* buffer_append_c_function_node_prototype(buffer_t* buffer, function_node_t* node);

buffer_t* buffer_append_function_body_node(buffer_t* buffer, function_body_node_t* node);

buffer_t* buffer_append_c_function_node_and_body(buffer_t* buffer, function_node_t* node);

buffer_t* buffer_append_c_function_argument_node(buffer_t* buffer, function_argument_node_t* node);

buffer_t* buffer_append_c_type_node(buffer_t* buffer, type_node_t* node);

buffer_t* buffer_append_c_attribute_node(buffer_t* buffer, attribute_node_t* node);

buffer_t* buffer_append_c_raw_token_span(buffer_t* buffer, oc_token_t* start_token, oc_token_t* end_token);

buffer_t* buffer_append_enum_node(buffer_t* buffer, enum_node_t* node);

buffer_t* buffer_append_enum_element(buffer_t* buffer, enum_element_t* node);

buffer_t* buffer_append_enum_to_string(buffer_t* buffer, enum_node_t* node, char* to_string_fn_prefix, char* type_string);

buffer_t* buffer_append_string_to_enum(buffer_t* buffer, enum_node_t* node, char* to_string_fn_prefix, char* type_string);

buffer_t* buffer_append_field_node(buffer_t* buffer, field_node_t* node);

buffer_t* buffer_append_struct_node(buffer_t* buffer, struct_node_t* node);

buffer_t* buffer_append_typedef_node(buffer_t* buffer, typedef_node_t* node);

buffer_t* buffer_append_cpp_include_node(buffer_t* buffer, cpp_include_node_t* node);

buffer_t* buffer_append_cpp_define_node(buffer_t* buffer, cpp_define_node_t* node);

buffer_t* buffer_append_global_variable_node(buffer_t* buffer, global_variable_node_t* node, boolean_t is_library);

buffer_t* buffer_append_literal_node(buffer_t* buffer, literal_node_t* node);

