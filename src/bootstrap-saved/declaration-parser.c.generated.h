/* Automatically extracted prototypes from declaration-parser.c */

parse_result_t parse_declarations(value_array_t* tokens, uint64_t position, boolean_t use_statement_parser);

pstatus_t parse_declaration(pstate_t* pstate);

pstatus_t parse_enum_node_declaration(pstate_t* pstate);

pstatus_t parse_structure_node_declaration(pstate_t* pstate);

pstatus_t parse_union_node_declaration(pstate_t* pstate);

pstatus_t parse_attribute_node(pstate_t* pstate);

pstatus_t parse_function_node(pstate_t* pstate);

pstatus_t parse_function_argument_node(pstate_t* pstate);

pstatus_t parse_function_body_node(pstate_t* pstate);

pstatus_t parse_typedef_node(pstate_t* pstate);

