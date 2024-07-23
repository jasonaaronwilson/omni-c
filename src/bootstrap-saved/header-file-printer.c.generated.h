/* Automatically extracted prototypes from header-file-printer.c */

buffer_t* buffer_append_c_function_node_prototype(buffer_t* buffer, function_node_t* node);

buffer_t* buffer_append_c_function_argument_node(buffer_t* buffer, function_argument_node_t* node);

buffer_t* buffer_append_c_type_node(buffer_t* buffer, type_node_t* node);

buffer_t* buffer_append_c_attribute_node(buffer_t* buffer, attribute_node_t* node);

buffer_t* buffer_append_c_raw_token_span(buffer_t* buffer, oc_token_t* start_token, oc_token_t* end_token);

buffer_t* buffer_append_enum_node(buffer_t* buffer, enum_node_t* node);

buffer_t* buffer_append_enum_element(buffer_t* buffer, enum_element_t* node);

buffer_t* buffer_append_enum_to_string(buffer_t* buffer, enum_node_t* node, char* to_string_fn_prefix, char* type_string);

buffer_t* buffer_append_string_to_enum(buffer_t* buffer, enum_node_t* node, char* to_string_fn_prefix, char* type_string);

