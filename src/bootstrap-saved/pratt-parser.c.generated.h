/* Automatically extracted prototypes from pratt-parser.c */

pstatus_t pratt_parse_expression(pstate_t* pstate, int precedence);

pstatus_t pratt_handle_instruction(pstate_t* pstate, pratt_parser_instruction_t instruction, parse_node_t* left);

pratt_parser_instruction_t get_prefix_instruction(token_t* token);

pratt_parser_instruction_t get_infix_instruction(token_t* token);

associativity_t precedence_to_associativity(precedence_t precedence);

