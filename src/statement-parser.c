#line 2 "statement-parser.c"
#ifndef _STATEMNET_PARSER_H_
#define _STATEMNET_PARSER_H_

#include "parser.h"

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
} do_statement_node_t;

/**
 * @structure while_statement_node_t
 */
typedef struct {
  parse_node_type_t tag;
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
} block_node_t;

/**
 * @structure return_statement_node_t
 */
typedef struct {
  parse_node_type_t tag;
} return_statement_node_t;

/**
 * @structure switch_statement_node_t
 */
typedef struct {
  parse_node_type_t tag;
} switch_statement_node_t;

/**
 * @structure case_label_node_t
 */
typedef struct {
  parse_node_type_t tag;
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
} goto_statement_node_t;

/**
 * @structure break_statement_node_t
 */
typedef struct {
  parse_node_type_t tag;
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
} label_node_t;

/**
 * @structure variable_statement_node_t
 */
typedef struct {
  parse_node_type_t tag;
} variable_statement_node_t;

/**
 * @structure expression_statement_node_t
 */
typedef struct {
  parse_node_type_t tag;
} expression_statement_node_t;

// Should come after any includes that provide types.

#include "statement-parser.c.generated.h"

#endif /* _STATEMENT_PARSER_H_ */

pstatus_t parse_expression(pstate_t* pstate) {
  // FIXME!
  return true;
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
  if (parse_block(pstate_ignore_error(pstate))
      || parse_return_statement(pstate_ignore_error(pstate))
      || parse_if_statement(pstate_ignore_error(pstate))
      || parse_while_statement(pstate_ignore_error(pstate))
      || parse_do_statement(pstate_ignore_error(pstate))
      || parse_for_statement(pstate_ignore_error(pstate))
      || parse_switch_statement(pstate_ignore_error(pstate))
      || parse_case_label(pstate_ignore_error(pstate))
      || parse_default_label(pstate_ignore_error(pstate))
      || parse_goto_statement(pstate_ignore_error(pstate))
      || parse_break_statement(pstate_ignore_error(pstate))
      || parse_continue_statement(pstate_ignore_error(pstate))
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
  if (!pstate_expect_token_string("{")) {
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_node_t* result = make_block_node();
  while (parse_statement(pstate)) {
    // TODO(jawilson) saved individual statements
  }
  if (!pstate_expect_token_string("}")) {
    return pstate_propagate_error(pstate, saved_position);
  }
  return pstate_node_result(pstate, result);
}

/**
 * @parse parse_return_statement
 */
pstatus_t parse_return_statement(pstate_t* pstate) {
  uint64_t saved_position = pstate->position;
  if (!pstate_expect_token_string("return")) {
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_expression(pstate);
  parse_node_t* expr = pstate_get_optional_node(pstate);
  if (!pstate_expect_token_string(";")) {
    return pstate_propagate_error(pstate, saved_position);
  }
  return pstate_node_result(make_return_statement(expr));
}

/**
 * @parse parse_if_statement
 */
pstatus_t parse_if_statement(pstate_t* pstate) {
  uint64_t saved_position = pstate->position;
  if (!pstate_expect_token_string("if") || !pstate_expect_token_string("(")
      || !pstate_parse_expression(pstate)) {
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_node_t* if_test = pstate_node(pstate);
  if (!pstate_expect_token_string(")") || !parse_statement(pstate)) {
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_node_t* if_true = pstate_node(pstate);
  parse_node_t* if_false = NULL;
  if (pstate_match_token_string("else")) {
    pstate_advance(pstate);
    if (!parse_statement(pstate)) {
      return pstate_propagate_error(pstate, saved_position);
    }
    if_false = pstate_node(pstate);
  }
  return pstate_node_result(make_if_statement(if_test, if_true, if_false));
}

/**
 * @parse parse_while_statement
 */
pstatus_t parse_while_statement(pstate_t* pstate) {
  uint64_t saved_position = pstate->position;
  if (!pstate_expect_token_string("while") || !pstate_expect_token_string("(")
      || !pstate_parse_expression(pstate)) {
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_node_t* while_test = pstate_node(pstate);
  if (!pstate_expect_token_string(")") || !parse_statement(pstate)) {
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_node_t* while_body = pstate_node(pstate);
  return pstate_node_result(make_while_statement(while_test, while_body));
}

/**
 * @parse parse_do_statement
 */
pstatus_t parse_do_statement(pstate_t* pstate) {
  uint64_t saved_position = pstate->position;
  if (!pstate_expect_token_string(pstate, "do") || !parse_statement(pstate)) {
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_node_t* do_while_body = pstate_node(pstate);
  if (!pstate_expect_token_string("(")) {
    return pstate_propagate_error(pstate, saved_position);
  }
  if (!parse_expression(pstate)) {
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_node_t* do_while_test = pstate_node(pstate);
  if (!pstate_expect_token_string(")")
      || !pstate_expect_token_string(pstate, ";")) {
    return pstate_propagate_error(pstate, saved_position);
  }
  return pstate_node_result(
      make_do_while_statement(do_while_body, do_while_test));
}

/**
 * @parse parse_for_statement
 */
pstatus_t parse_for_statement(pstate_t* pstate) {
  uint64_t saved_position = pstate->position;
  if (!pstate_expect_token_string(pstate, "for")
      || !pstate_expect_token(pstate, "(")) {
    return pstate_propagate_error(pstate, saved_position);
  }
  if (!parse_statement(pstate)) {
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_node_t* for_init = pstate_node(pstate);
  if (!parse_expression(pstate)) {
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_node_t* for_test = pstate_node(pstate);
  if (!pstate_expect_token_string(pstate, ";")) {
    return pstate_propagate_error(pstate, saved_position);
  }
  if (!parse_expression(pstate)) {
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_node_t* for_increment = pstate_node(pstate);
  if (!pstate_expect_token_string(pstate, ")")) {
    return pstate_propagate_error(pstate, saved_position);
  }
  if (!parse_statement(pstate)) {
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_node_t* for_body = pstate_node(pstate);
  return pstate_node_result(
      make_for_statement(for_init, for_test, for_increment, for_body));
}

/**
 * @parse parse_switch_statement
 */
pstatus_t parse_switch_statement(pstate_t* pstate) {
  uint64_t saved_position = pstate->position;
  if (!pstate_expect_token_string(pstate, "switch")
      || !pstate_expect_token_string(pstate, "(")
      || !parse_experssion(pstate)) {
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_node_t* switch_item = pstate_node(pstate);
  if (!pstate_expect_token_string(")") || !parse_block(pstate)) {
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_node_t* block = pstate_node(pstate);
  return pstate_node_result(make_switch_statement(switch_item, block));
}

/**
 * @parse parse_case_label
 */
pstatus_t parse_case_label(pstate_t* pstate) {
  uint64_t saved_position = pstate->position;
  if (!pstate_expect_token_string(pstate, "case")
      || !parse_experssion(pstate)) {
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_node_t* case_expr = pstate_node(pstate);
  if (!pstate_expect_token_string(":")) {
    return pstate_propagate_error(pstate, saved_position);
  }
  return pstate_node_result(make_case_label(case_expr));
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
  parse_node_t* expr = pstate_node(pstate);
  if (!pstate_expect_token_string(";")) {
    return pstate_propagate_error(pstate, saved_position);
  }
  return pstate_node_result(make_expression_statement(expr));
}

/**
 * @function parse_goto_statement
 */
pstatus_t parse_goto_statement(pstate_t* pstate) { return false; }

/**
 * @function parse_break_statement
 */
pstatus_t parse_break_statement(pstate_t* pstate) { return false; }

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

/* ====================================================================== */
/* constructors, etc. for statement nodes */
/* ====================================================================== */

block_node_t* make_block_node() {
  for_statement_node_t* result = malloc_struct(for_statement_node_t);
  result->tag = PARSE_NODE_BLOCK;
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