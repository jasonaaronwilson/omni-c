/* Automatically extracted prototypes from declaration-parser.c */

parse_result_t parse_declarations(value_array_t* tokens, uint64_t position);

pstatus_t parse_declaration(pstate_t* pstate);

pstatus_t parse_enum_node_declaration(pstate_t* pstate);

pstatus_t parse_structure_node_declaration(pstate_t* pstate);

pstatus_t parse_structure_node(pstate_t* pstate);

pstatus_t parse_field_node(pstate_t* pstate, boolean_t allow_field_width);

pstatus_t parse_union_node_declaration(pstate_t* pstate);

pstatus_t parse_union_node(pstate_t* pstate);

pstatus_t parse_attribute_node(pstate_t* pstate);

pstatus_t parse_function_node(pstate_t* pstate);

pstatus_t parse_function_argument_node(pstate_t* pstate);

pstatus_t parse_function_body_node(pstate_t* pstate);

canonical_type_result_t make_type_token_result(char* str, int consumed_tokens);

canonical_type_result_t parse_canonical_type(pstate_t* pstate);

pstatus_t parse_user_type_node(pstate_t* pstate);

pstatus_t parse_type_node(pstate_t* pstate);

pstatus_t parse_enum_node(pstate_t* pstate);

pstatus_t parse_enum_element_node(pstate_t* pstate);

pstatus_t parse_typedef_node(pstate_t* pstate);

pstatus_t parse_global_variable_node(pstate_t* pstate);

pstatus_t parse_literal_node(pstate_t* pstate);

pstatus_t parse_balanced_construct(pstate_t* pstate);

