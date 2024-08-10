/* Automatically extracted prototypes from pstate.c */

pstatus_t pstate_error(pstate_t* pstate, uint64_t saved_position, parse_error_code_t parse_error_code);

pstate_t* pstate_ignore_error(pstate_t* pstate);

pstatus_t pstate_propagate_error(pstate_t* pstate, uint64_t saved_position);

pstatus_t pstate_set_result_token(pstate_t* pstate, token_t* token);

pstatus_t pstate_set_result_node(pstate_t* pstate, parse_node_t* node);

token_t* pstate_get_result_token(pstate_t* pstate);

parse_node_t* pstate_get_result_node(pstate_t* pstate);

parse_node_t* pstate_get_optional_result_node(pstate_t* pstate);

token_t* pstate_peek(pstate_t* pstate, int offset);

token_t* pstate_advance(pstate_t* pstate);

boolean_t pstate_match_token_string(pstate_t* pstate, char* token_string);

pstatus_t pstate_expect_token_string(pstate_t* pstate, char* token_string);

pstatus_t pstate_expect_token_type(pstate_t* pstate, token_type_t token_type);

