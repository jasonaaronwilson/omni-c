/* Automatically extracted prototypes from statement-parser.c */

pstatus_t parse_expression(pstate_t* pstate);

pstatus_t parse_statement(pstate_t* pstate);

pstatus_t parse_block(pstate_t* pstate);

pstatus_t parse_return_statement(pstate_t* pstate);

pstatus_t parse_if_statement(pstate_t* pstate);

pstatus_t parse_while_statement(pstate_t* pstate);

pstatus_t parse_do_statement(pstate_t* pstate);

pstatus_t parse_for_statement(pstate_t* pstate);

pstatus_t parse_switch_statement(pstate_t* pstate);

pstatus_t parse_case_label(pstate_t* pstate);

pstatus_t parse_expression_statement(pstate_t* pstate);

pstatus_t parse_goto_statement(pstate_t* pstate);

pstatus_t parse_break_statement(pstate_t* pstate);

pstatus_t parse_continue_statement(pstate_t* pstate);

pstatus_t parse_label_statement(pstate_t* pstate);

pstatus_t parse_variable_statement(pstate_t* pstate);

pstatus_t parse_empty_statement(pstate_t* pstate);

pstatus_t parse_default_label(pstate_t* pstate);

block_node_t* make_block_node();

break_statement_node_t* make_break_statement(token_t* break_keyword_token);

for_statement_node_t* make_for_statement(parse_node_t* for_init, parse_node_t* for_test, parse_node_t* for_increment, parse_node_t* for_body);

if_statement_node_t* make_if_statement(parse_node_t* if_condition, parse_node_t* if_true, parse_node_t* if_else);

expression_statement_node_t* make_expression_statement_node(parse_node_t* expression);

return_statement_node_t* make_return_statement(parse_node_t* expression);

while_statement_node_t* make_while_statement(parse_node_t* condition, parse_node_t* body);

do_statement_node_t* make_do_statement(parse_node_t* body, parse_node_t* condition);

switch_statement_node_t* make_switch_statement(parse_node_t* expression, parse_node_t* block);

case_label_node_t* make_case_label(parse_node_t* expression);

