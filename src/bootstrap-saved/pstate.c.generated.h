/* Automatically extracted prototypes from pstate.c */

void ensure_empty_result_state(pstate_t* pstate);

pstate_t* pstate_ignore_error(pstate_t* pstate);

pstatus_t pstate_propagate_error(pstate_t* pstate, uint64_t saved_position);

pstatus_t pstate_token_result(pstate_t* pstate, token_t* token);

pstatus_t pstate_node_result(pstate_t* pstate, parse_node_t* node);

token_t* pstate_token(pstate_t* pstate);

token_t* pstate_peek(pstate_t* pstate, int offset);

token_t* pstate_advance(pstate_t* pstate);

boolean_t pstate_match_token_string(pstate_t* pstate, char* token_string);

pstatus_t pstate_expect_token_string(pstate_t* pstate, char* token_string);

