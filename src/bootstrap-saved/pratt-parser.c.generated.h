/* Automatically extracted prototypes from pratt-parser.c */

parse_result_t pratt_parse_expression(value_array_t* tokens, uint64_t position, int precedence);

pratt_parser_instruction_t get_prefix_instruction(oc_token_t* token);

pratt_parser_instruction_t get_infix_instruction(oc_token_t* token);

parse_result_t pratt_handle_instruction(pratt_parser_instruction_t instruction, value_array_t* tokens, uint64_t position, parse_node_t* left);

