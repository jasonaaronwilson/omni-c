#line 2 "statement-parser.c"
#ifndef _STATEMNET_PARSER_H_
#define _STATEMNET_PARSER_H_

#include "parser.h"
#include "pratt-parser.h"
#include "pstate.h"

/**
 * @structure if_statement_node_t
 */
typedef struct {
  parse_node_type_t tag;
  parse_node_t* if_condition;
  parse_node_t* if_true;
  parse_node_t* if_else;
} if_statement_node_t;

/**
 * @structure for_statement_node_t
 */
typedef struct {
  parse_node_type_t tag;
  parse_node_t* for_init;
  parse_node_t* for_test;
  parse_node_t* for_increment;
  parse_node_t* for_body;
} for_statement_node_t;

/**
 * @structure do_statement_node_t
 */
typedef struct {
  parse_node_type_t tag;
  parse_node_t* body;
  parse_node_t* condition;
} do_statement_node_t;

/**
 * @structure while_statement_node_t
 */
typedef struct {
  parse_node_type_t tag;
  parse_node_t* condition;
  parse_node_t* body;
} while_statement_node_t;

/**
 * @structure empty_statement_node_t
 */
typedef struct {
  parse_node_type_t tag;
} empty_statement_node_t;

/**
 * @structure block_node_t
 */
typedef struct {
  parse_node_type_t tag;
  value_array_t* statements;
} block_node_t;

/**
 * @structure return_statement_node_t
 */
typedef struct {
  parse_node_type_t tag;
  parse_node_t* expression;
} return_statement_node_t;

/**
 * @structure switch_statement_node_t
 */
typedef struct {
  parse_node_type_t tag;
  parse_node_t* expression;
  parse_node_t* block;
} switch_statement_node_t;

/**
 * @structure case_label_node_t
 */
typedef struct {
  parse_node_type_t tag;
  parse_node_t* expression;
} case_label_node_t;

/**
 * @structure default_label_node_t
 */
typedef struct {
  parse_node_type_t tag;
} _node_t;

/**
 * @structure goto_statement_node_t
 */
typedef struct {
  parse_node_type_t tag;
  token_t* label;
} goto_statement_node_t;

/**
 * @structure break_statement_node_t
 */
typedef struct {
  parse_node_type_t tag;
  token_t* break_keyword_token;
} break_statement_node_t;

/**
 * @structure continue_statement_node_t
 */
typedef struct {
  parse_node_type_t tag;
} continue_statement_node_t;

/**
 * @structure label_node_t
 */
typedef struct {
  parse_node_type_t tag;
  token_t* label;
} label_node_t;

/**
 * @structure variable_statement_node_t
 */
typedef struct {
  parse_node_type_t tag;
  // type
  // name
  parse_node_t* expression;
} variable_statement_node_t;

/**
 * @structure expression_statement_node_t
 */
typedef struct {
  parse_node_type_t tag;
  parse_node_t* expression;
} expression_statement_node_t;

// Should come after any includes that provide types.

#include "statement-parser.c.generated.h"

#endif /* _STATEMENT_PARSER_H_ */

pstatus_t parse_expression(pstate_t* pstate) {
  parse_result_t result
      = pratt_parse_expression(pstate->tokens, pstate->position, 0);
  if (!is_valid_result(result)) {
    pstate_error(pstate, pstate->position, result.parse_error.parse_error_code);
  }
  pstate->position = result.next_token_position;
  return pstate_set_result_node(pstate, result.node);
}

/**
 * @parse parse_statement
 *
 * Parse the next statement and return true leaving a node in pstate
 * or else return false and leave an eror in pstate.
 *
 * Like the rest of the parser, we don't necessarily only parse legal
 * programs (but when we do parse a legal program the parse tree will
 * obviously be correct). Additional error checking must be performed
 * on the parse tree in order to determine legality. For example we
 * would accept a case label outside of a switch statement (or one
 * with a non constant expression), but converting this to C would
 * cause a C compiler error.
 */
pstatus_t parse_statement(pstate_t* pstate) {
  uint64_t saved_position = pstate->position;
  if (parse_block(pstate) || parse_break_statement(pstate_ignore_error(pstate))
      || parse_return_statement(pstate_ignore_error(pstate))
      || parse_if_statement(pstate_ignore_error(pstate))
      || parse_while_statement(pstate_ignore_error(pstate))
      || parse_do_statement(pstate_ignore_error(pstate))
      || parse_for_statement(pstate_ignore_error(pstate))
      || parse_switch_statement(pstate_ignore_error(pstate))
      || parse_case_label(pstate_ignore_error(pstate))
      || parse_default_label(pstate_ignore_error(pstate))
      || parse_continue_statement(pstate_ignore_error(pstate))
      || parse_goto_statement(pstate_ignore_error(pstate))
      || parse_label_statement(pstate_ignore_error(pstate))
      || parse_variable_statement(pstate_ignore_error(pstate))
      || parse_expression_statement(pstate_ignore_error(pstate))
      || parse_empty_statement(pstate_ignore_error(pstate))) {
    return true;
  }
  return pstate_error(pstate, saved_position, PARSE_ERROR_EXPECTED_STATEMENT);
}

/**
 * @parse parse_block
 */
pstatus_t parse_block(pstate_t* pstate) {
  uint64_t saved_position = pstate->position;
  if (!pstate_expect_token_string(pstate, "{")) {
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_node_t* result = to_node(make_block_node());
  while (parse_statement(pstate)) {
    // TODO(jawilson) saved individual statements
  }
  if (!pstate_expect_token_string(pstate, "}")) {
    return pstate_propagate_error(pstate, saved_position);
  }
  return pstate_set_result_node(pstate, result);
}

/**
 * @parse parse_return_statement
 */
pstatus_t parse_return_statement(pstate_t* pstate) {
  uint64_t saved_position = pstate->position;
  if (!pstate_expect_token_string(pstate, "return")) {
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_expression(pstate);
  parse_node_t* expr = pstate_get_optional_result_node(pstate);
  if (!pstate_expect_token_string(pstate, ";")) {
    return pstate_propagate_error(pstate, saved_position);
  }
  return pstate_set_result_node(pstate, to_node(make_return_statement(expr)));
}

/**
 * @parse parse_if_statement
 */
pstatus_t parse_if_statement(pstate_t* pstate) {
  uint64_t saved_position = pstate->position;
  if (!pstate_expect_token_string(pstate, "if")
      || !pstate_expect_token_string(pstate, "(")
      || !parse_expression(pstate)) {
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_node_t* if_test = pstate_get_result_node(pstate);
  if (!pstate_expect_token_string(pstate, ")") || !parse_statement(pstate)) {
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_node_t* if_true = pstate_get_result_node(pstate);
  parse_node_t* if_false = NULL;
  if (pstate_match_token_string(pstate, "else")) {
    pstate_advance(pstate);
    if (!parse_statement(pstate)) {
      return pstate_propagate_error(pstate, saved_position);
    }
    if_false = pstate_get_result_node(pstate);
  }
  return pstate_set_result_node(
      pstate, to_node(make_if_statement(if_test, if_true, if_false)));
}

/**
 * @parse parse_while_statement
 */
pstatus_t parse_while_statement(pstate_t* pstate) {
  uint64_t saved_position = pstate->position;
  if (!pstate_expect_token_string(pstate, "while")
      || !pstate_expect_token_string(pstate, "(")
      || !parse_expression(pstate)) {
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_node_t* while_test = pstate_get_result_node(pstate);
  if (!pstate_expect_token_string(pstate, ")") || !parse_statement(pstate)) {
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_node_t* while_body = pstate_get_result_node(pstate);
  return pstate_set_result_node(
      pstate, to_node(make_while_statement(while_test, while_body)));
}

/**
 * @parse parse_do_statement
 */
pstatus_t parse_do_statement(pstate_t* pstate) {
  uint64_t saved_position = pstate->position;
  if (!pstate_expect_token_string(pstate, "do") || !parse_statement(pstate)) {
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_node_t* do_while_body = pstate_get_result_node(pstate);
  if (!pstate_expect_token_string(pstate, "(")) {
    return pstate_propagate_error(pstate, saved_position);
  }
  if (!parse_expression(pstate)) {
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_node_t* do_while_condition = pstate_get_result_node(pstate);
  if (!pstate_expect_token_string(pstate, ")")
      || !pstate_expect_token_string(pstate, ";")) {
    return pstate_propagate_error(pstate, saved_position);
  }
  return pstate_set_result_node(
      pstate, to_node(make_do_statement(do_while_body, do_while_condition)));
}

/**
 * @parse parse_for_statement
 */
pstatus_t parse_for_statement(pstate_t* pstate) {
  uint64_t saved_position = pstate->position;
  if (!pstate_expect_token_string(pstate, "for")
      || !pstate_expect_token_string(pstate, "(")) {
    return pstate_propagate_error(pstate, saved_position);
  }
  if (!parse_statement(pstate)) {
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_node_t* for_init = pstate_get_result_node(pstate);
  if (!parse_expression(pstate)) {
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_node_t* for_test = pstate_get_result_node(pstate);
  if (!pstate_expect_token_string(pstate, ";")) {
    return pstate_propagate_error(pstate, saved_position);
  }
  if (!parse_expression(pstate)) {
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_node_t* for_increment = pstate_get_result_node(pstate);
  if (!pstate_expect_token_string(pstate, ")")) {
    return pstate_propagate_error(pstate, saved_position);
  }
  if (!parse_statement(pstate)) {
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_node_t* for_body = pstate_get_result_node(pstate);
  return pstate_set_result_node(
      pstate,
      to_node(make_for_statement(for_init, for_test, for_increment, for_body)));
}

/**
 * @parse parse_switch_statement
 */
pstatus_t parse_switch_statement(pstate_t* pstate) {
  uint64_t saved_position = pstate->position;
  if (!pstate_expect_token_string(pstate, "switch")
      || !pstate_expect_token_string(pstate, "(")
      || !parse_expression(pstate)) {
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_node_t* switch_item = pstate_get_result_node(pstate);
  if (!pstate_expect_token_string(pstate, ")") || !parse_block(pstate)) {
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_node_t* block = pstate_get_result_node(pstate);
  return pstate_set_result_node(
      pstate, to_node(make_switch_statement(switch_item, block)));
}

/**
 * @parse parse_case_label
 */
pstatus_t parse_case_label(pstate_t* pstate) {
  uint64_t saved_position = pstate->position;
  if (!pstate_expect_token_string(pstate, "case")
      || !parse_expression(pstate)) {
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_node_t* case_expr = pstate_get_result_node(pstate);
  if (!pstate_expect_token_string(pstate, ":")) {
    return pstate_propagate_error(pstate, saved_position);
  }
  return pstate_set_result_node(pstate, to_node(make_case_label(case_expr)));
}

/**
 * @parse parse_expresion_statement
 *
 * This includes assignment expressions unless we are declaring a
 * variable.
 */
pstatus_t parse_expression_statement(pstate_t* pstate) {
  uint64_t saved_position = pstate->position;
  if (!parse_expression(pstate)) {
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_node_t* expr = pstate_get_result_node(pstate);
  if (!pstate_expect_token_string(pstate, ";")) {
    return pstate_propagate_error(pstate, saved_position);
  }
  return pstate_set_result_node(pstate,
                                to_node(make_expression_statement_node(expr)));
}

/**
 * @function parse_goto_statement
 */
pstatus_t parse_goto_statement(pstate_t* pstate) { return false; }

/**
 * @function parse_break_statement
 */
pstatus_t parse_break_statement(pstate_t* pstate) {
  uint64_t saved_position = pstate->position;
  token_t* break_keyword_token = pstate_peek(pstate, 0);
  if (!pstate_expect_token_string(pstate, "break")
      || !pstate_expect_token_string(pstate, ";")) {
    return pstate_propagate_error(pstate, saved_position);
  }
  return pstate_set_result_node(
      pstate, to_node(make_break_statement(break_keyword_token)));
}

/**
 * @function parse_continue_statement
 */
pstatus_t parse_continue_statement(pstate_t* pstate) { return false; }

/**
 * @function parse_label_statement
 */
pstatus_t parse_label_statement(pstate_t* pstate) { return false; }

/**
 * @function parse_variable_statement
 *
 * For example "uint64_t foo = 12345;"
 */
pstatus_t parse_variable_statement(pstate_t* pstate) { return false; }

/**
 * @function parse_empty_statement
 */
pstatus_t parse_empty_statement(pstate_t* pstate) { return false; }

/**
 * @function parse_default_label
 */
pstatus_t parse_default_label(pstate_t* pstate) { return false; }

/* ====================================================================== */
/* constructors, etc. for statement nodes */
/* ====================================================================== */

block_node_t* make_block_node() {
  block_node_t* result = malloc_struct(block_node_t);
  result->tag = PARSE_NODE_BLOCK;
  return result;
}

break_statement_node_t* make_break_statement(token_t* break_keyword_token) {
  break_statement_node_t* result = malloc_struct(break_statement_node_t);
  result->tag = PARSE_NODE_BREAK_STATEMENT;
  result->break_keyword_token = break_keyword_token;
  return result;
}

for_statement_node_t* make_for_statement(parse_node_t* for_init,
                                         parse_node_t* for_test,
                                         parse_node_t* for_increment,
                                         parse_node_t* for_body) {
  for_statement_node_t* result = malloc_struct(for_statement_node_t);
  result->tag = PARSE_NODE_FOR_STATEMENT;
  result->for_init = for_init;
  result->for_test = for_test;
  result->for_increment = for_increment;
  result->for_body = for_body;
  return result;
}

if_statement_node_t* make_if_statement(parse_node_t* if_condition,
                                       parse_node_t* if_true,
                                       parse_node_t* if_else) {
  if_statement_node_t* result = malloc_struct(if_statement_node_t);
  result->tag = PARSE_NODE_IF_STATEMENT;
  result->if_condition = if_condition;
  result->if_true = if_true;
  result->if_else = if_else;
  return result;
}

expression_statement_node_t*
    make_expression_statement_node(parse_node_t* expression) {
  expression_statement_node_t* result
      = malloc_struct(expression_statement_node_t);
  result->tag = PARSE_NODE_EXPRESSION_STATEMENT;
  result->expression = expression;
  return result;
}

return_statement_node_t* make_return_statement(parse_node_t* expression) {
  return_statement_node_t* result = malloc_struct(return_statement_node_t);
  result->tag = PARSE_NODE_RETURN_STATEMENT;
  result->expression = expression;
  return result;
}

while_statement_node_t* make_while_statement(parse_node_t* condition,
                                             parse_node_t* body) {
  while_statement_node_t* result = malloc_struct(while_statement_node_t);
  result->tag = PARSE_NODE_WHILE_STATEMENT;
  result->condition = condition;
  result->body = body;
  return result;
}

do_statement_node_t* make_do_statement(parse_node_t* body,
                                       parse_node_t* condition) {
  do_statement_node_t* result = malloc_struct(do_statement_node_t);
  result->tag = PARSE_NODE_DO_STATEMENT;
  result->body = body;
  result->condition = condition;
  return result;
}

switch_statement_node_t* make_switch_statement(parse_node_t* expression,
                                               parse_node_t* block) {
  switch_statement_node_t* result = malloc_struct(switch_statement_node_t);
  result->tag = PARSE_NODE_SWITCH_STATEMENT;
  result->expression = expression;
  result->block = block;
  return result;
}

case_label_node_t* make_case_label(parse_node_t* expression) {
  case_label_node_t* result = malloc_struct(case_label_node_t);
  result->tag = PARSE_NODE_CASE_LABEL;
  result->expression = expression;
  return result;
}
