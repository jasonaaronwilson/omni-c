/* Automatically extracted prototypes from declaration-parser.c */

parse_result_t parse_declarations(value_array_t* tokens, uint64_t position);

parse_result_t parse_declaration(value_array_t* tokens, uint64_t position);

parse_result_t parse_structure_node(value_array_t* tokens, uint64_t position);

parse_result_t parse_field_node(value_array_t* tokens, uint64_t position);

parse_result_t parse_attribute_node(value_array_t* tokens, uint64_t position);

parse_result_t parse_function_node(value_array_t* tokens, uint64_t position);

parse_result_t parse_function_argument_node(value_array_t* tokens, uint64_t position);

parse_result_t parse_function_body_node(value_array_t* tokens, uint64_t position);

canonical_type_result_t make_type_token_result(char* str, int consumed_tokens);

canonical_type_result_t parse_canonical_type(value_array_t* tokens, uint64_t position);

parse_result_t parse_user_type_node(value_array_t* tokens, uint64_t position);

parse_result_t parse_type_node(value_array_t* tokens, uint64_t position);

parse_result_t parse_enum_node(value_array_t* tokens, uint64_t position);

parse_result_t parse_enum_element_node(value_array_t* tokens, uint64_t position);

parse_result_t parse_typedef_node(value_array_t* tokens, uint64_t position);

parse_result_t parse_global_variable_node(value_array_t* tokens, uint64_t position);

parse_result_t parse_literal_node(value_array_t* tokens, uint64_t position);

