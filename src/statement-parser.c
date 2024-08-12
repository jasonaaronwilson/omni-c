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
  token_t* semi_colon_token;
} empty_statement_node_t;

/**
 * @structure block_node_t
 */
typedef struct {
  parse_node_type_t tag;
  node_list_t statements;
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
  token_t* continue_keyword_token;
} continue_statement_node_t;

/**
 * @structure label_statement_t
 */
typedef struct {
  parse_node_type_t tag;
  token_t* label;
} label_statement_node_t;

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
      // || parse_variable_statement(pstate_ignore_error(pstate))
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
  block_node_t* result = make_block_node();
  while (parse_statement(pstate)) {
    node_list_add_node(&result->statements, pstate_get_result_node(pstate));
  }
  if (!pstate_expect_token_string(pstate, "}")) {
    return pstate_propagate_error(pstate, saved_position);
  }
  return pstate_set_result_node(pstate, to_node(result));
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
pstatus_t parse_goto_statement(pstate_t* pstate) {
  uint64_t saved_position = pstate->position;
  token_t* label_token = pstate_peek(pstate, 1);
  if (!pstate_expect_token_string(pstate, "goto")
      || !pstate_expect_token_type(pstate, TOKEN_TYPE_IDENTIFIER)
      || !pstate_expect_token_string(pstate, ";")) {
    return pstate_propagate_error(pstate, saved_position);
  }
  return pstate_set_result_node(pstate,
                                to_node(make_goto_statement(label_token)));
}

/**
 * @function parse_break_statement
 */
pstatus_t parse_break_statement(pstate_t* pstate) {
  uint64_t saved_position = pstate->position;
  token_t* keyword_token = pstate_peek(pstate, 0);
  if (!pstate_expect_token_string(pstate, "break")
      || !pstate_expect_token_string(pstate, ";")) {
    return pstate_propagate_error(pstate, saved_position);
  }
  return pstate_set_result_node(pstate,
                                to_node(make_break_statement(keyword_token)));
}

/**
 * @function parse_continue_statement
 */
pstatus_t parse_continue_statement(pstate_t* pstate) {
  uint64_t saved_position = pstate->position;
  token_t* keyword_token = pstate_peek(pstate, 0);
  if (!pstate_expect_token_string(pstate, "continue")
      || !pstate_expect_token_string(pstate, ";")) {
    return pstate_propagate_error(pstate, saved_position);
  }
  return pstate_set_result_node(
      pstate, to_node(make_continue_statement(keyword_token)));
}

/**
 * @function parse_label_statement
 */
pstatus_t parse_label_statement(pstate_t* pstate) {
  uint64_t saved_position = pstate->position;
  token_t* label_token = pstate_peek(pstate, 0);
  if (!pstate_expect_token_type(pstate, TOKEN_TYPE_IDENTIFIER)
      || !pstate_expect_token_string(pstate, ":")) {
    return pstate_propagate_error(pstate, saved_position);
  }
  return pstate_set_result_node(pstate,
                                to_node(make_label_statement(label_token)));
}

/**
 * @function parse_empty_statement
 */
pstatus_t parse_empty_statement(pstate_t* pstate) {
  uint64_t saved_position = pstate->position;
  token_t* semi_colon_token = pstate_peek(pstate, 0);
  if (!pstate_expect_token_string(pstate, ";")) {
    return pstate_propagate_error(pstate, saved_position);
  }
  return pstate_set_result_node(pstate,
                                to_node(make_empty_statement(semi_colon_token)));
}

/**
 * @function parse_default_label
 */
pstatus_t parse_default_label(pstate_t* pstate) { return false; }

/* ====================================================================== */
/* constructors, etc. */
/* ====================================================================== */

/**
 * @function make_break_statement
 *
 * Allocating a break statement node and set it's field values.
 */
break_statement_node_t* make_break_statement(token_t* break_keyword_token) {
  break_statement_node_t* result = malloc_struct(break_statement_node_t);
  result->tag = PARSE_NODE_BREAK_STATEMENT;
  result->break_keyword_token = break_keyword_token;
  return result;
}

/**
 * @function to_break_statement_node
 *
 * Safely cast a generic node pointer to a break_statement_node_t
 * pointer after examining it's tag.
 */
break_statement_node_t* to_break_statement_node(parse_node_t* ptr) {
  if (ptr == NULL || ptr->tag != PARSE_NODE_BREAK_STATEMENT) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return cast(break_statement_node_t*, ptr);
}

// ----------------------------------------------------------------------

/**
 * @function make_continue_statement
 *
 * Allocating a break statement node and set it's field values.
 */
continue_statement_node_t* make_continue_statement(token_t* keyword_token) {
  continue_statement_node_t* result = malloc_struct(continue_statement_node_t);
  result->tag = PARSE_NODE_CONTINUE_STATEMENT;
  result->continue_keyword_token = keyword_token;
  return result;
}

/**
 * @function to_continue_statement_node
 *
 * Safely cast a generic node pointer to a continue_statement_node_t
 * pointer after examining it's tag.
 */
continue_statement_node_t* to_continue_statement_node(parse_node_t* ptr) {
  if (ptr == NULL || ptr->tag != PARSE_NODE_CONTINUE_STATEMENT) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return cast(continue_statement_node_t*, ptr);
}

// ----------------------------------------------------------------------

/**
 * @function make_label_statement
 *
 * Allocating a break statement node and set it's field values.
 */
label_statement_node_t* make_label_statement(token_t* label) {
  label_statement_node_t* result = malloc_struct(label_statement_node_t);
  result->tag = PARSE_NODE_LABEL_STATEMENT;
  result->label = label;
  return result;
}

/**
 * @function to_label_statement_node
 *
 * Safely cast a generic node pointer to a label_statement_node_t
 * pointer after examining it's tag.
 */
label_statement_node_t* to_label_statement_node(parse_node_t* ptr) {
  if (ptr == NULL || ptr->tag != PARSE_NODE_LABEL_STATEMENT) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return cast(label_statement_node_t*, ptr);
}

// ----------------------------------------------------------------------

/**
 * @function make_goto_statement
 *
 * Allocating a break statement node and set it's field values.
 */
goto_statement_node_t* make_goto_statement(token_t* label) {
  goto_statement_node_t* result = malloc_struct(goto_statement_node_t);
  result->tag = PARSE_NODE_GOTO_STATEMENT;
  result->label = label;
  return result;
}

/**
 * @function to_goto_statement_node
 *
 * Safely cast a generic node pointer to a goto_statement_node_t
 * pointer after examining it's tag.
 */
goto_statement_node_t* to_goto_statement_node(parse_node_t* ptr) {
  if (ptr == NULL || ptr->tag != PARSE_NODE_GOTO_STATEMENT) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return cast(goto_statement_node_t*, ptr);
}

// ----------------------------------------------------------------------

/**
 * @function make_empty_statement
 *
 * Allocating a break statement node and set it's field values.
 */
empty_statement_node_t* make_empty_statement(token_t* semi_colon_token) {
  empty_statement_node_t* result = malloc_struct(empty_statement_node_t);
  result->tag = PARSE_NODE_EMPTY_STATEMENT;
  result->semi_colon_token = semi_colon_token;
  return result;
}

/**
 * @function to_empty_statement_node
 *
 * Safely cast a generic node pointer to a empty_statement_node_t
 * pointer after examining it's tag.
 */
empty_statement_node_t* to_empty_statement_node(parse_node_t* ptr) {
  if (ptr == NULL || ptr->tag != PARSE_NODE_EMPTY_STATEMENT) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return cast(empty_statement_node_t*, ptr);
}

// ----------------------------------------------------------------------

/**
 * @function make_switch_statement
 *
 * Allocating a switch statement node and set it's field values.
 */
switch_statement_node_t* make_switch_statement(parse_node_t* expression,
                                               parse_node_t* block) {
  switch_statement_node_t* result = malloc_struct(switch_statement_node_t);
  result->tag = PARSE_NODE_SWITCH_STATEMENT;
  result->expression = expression;
  result->block = block;
  return result;
}

/**
 * @function to_switch_statement_node
 *
 * Safely cast a generic node pointer to a switch_statement_node_t
 * pointer after examining it's tag.
 */
switch_statement_node_t* to_switch_statement_node(parse_node_t* ptr) {
  if (ptr == NULL || ptr->tag != PARSE_NODE_SWITCH_STATEMENT) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return cast(switch_statement_node_t*, ptr);
}

// ----------------------------------------------------------------------

/**
 * @function make_case_label
 *
 * Allocating a case label node and set it's field values.
 */
case_label_node_t* make_case_label(parse_node_t* expression) {
  case_label_node_t* result = malloc_struct(case_label_node_t);
  result->tag = PARSE_NODE_CASE_LABEL;
  result->expression = expression;
  return result;
}

/**
 * @function to_case_label_node
 *
 * Safely cast a generic node pointer to a case_label_node_t
 * pointer after examining it's tag.
 */
case_label_node_t* to_case_label_node(parse_node_t* ptr) {
  if (ptr == NULL || ptr->tag != PARSE_NODE_CASE_LABEL) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return cast(case_label_node_t*, ptr);
}

// ----------------------------------------------------------------------

/**
 * @function make_block_node
 *
 * Allocating a block node and set it's field values. Typically
 * statements are added to the block node after creation.
 */
block_node_t* make_block_node() {
  block_node_t* result = malloc_struct(block_node_t);
  result->tag = PARSE_NODE_BLOCK;
  return result;
}

/**
 * @function to_block_node
 *
 * Safely cast a generic node pointer to a block_node_t
 * pointer after examining it's tag.
 */
block_node_t* to_block_node(parse_node_t* ptr) {
  if (ptr == NULL || ptr->tag != PARSE_NODE_BLOCK) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return cast(block_node_t*, ptr);
}

// ----------------------------------------------------------------------

/**
 * @function make_for_statement_node
 *
 * Allocating a for statement node and set it's field
 * values.
 */
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

/**
 * @function to_for_statement_node
 *
 * Safely cast a generic node pointer to a for_statement_node_t
 * pointer after examining it's tag.
 */
for_statement_node_t* to_for_statement_node(parse_node_t* ptr) {
  if (ptr == NULL || ptr->tag != PARSE_NODE_FOR_STATEMENT) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return cast(for_statement_node_t*, ptr);
}

// ----------------------------------------------------------------------

/**
 * @function make_if_statement
 *
 * Allocating a if statement node and set it's field values.
 */
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

/**
 * @function to_if_statement_node
 *
 * Safely cast a generic node pointer to a if_statement_node_t
 * pointer after examining it's tag.
 */
if_statement_node_t* to_if_statement_node(parse_node_t* ptr) {
  if (ptr == NULL || ptr->tag != PARSE_NODE_IF_STATEMENT) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return cast(if_statement_node_t*, ptr);
}

// ----------------------------------------------------------------------

/**
 * @function make_expression_statement
 *
 * Allocating a expression statement node and set it's field values.
 */
expression_statement_node_t*
    make_expression_statement_node(parse_node_t* expression) {
  expression_statement_node_t* result
      = malloc_struct(expression_statement_node_t);
  result->tag = PARSE_NODE_EXPRESSION_STATEMENT;
  result->expression = expression;
  return result;
}

/**
 * @function to_expression_statement_node
 *
 * Safely cast a generic node pointer to a expression_statement_node_t
 * pointer after examining it's tag.
 */
expression_statement_node_t* to_expression_statement_node(parse_node_t* ptr) {
  if (ptr == NULL || ptr->tag != PARSE_NODE_EXPRESSION_STATEMENT) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return cast(expression_statement_node_t*, ptr);
}

// ----------------------------------------------------------------------

/**
 * @function make_return_statement
 *
 * Allocating a return statement node and set it's field values.
 */
return_statement_node_t* make_return_statement(parse_node_t* expression) {
  return_statement_node_t* result = malloc_struct(return_statement_node_t);
  result->tag = PARSE_NODE_RETURN_STATEMENT;
  result->expression = expression;
  return result;
}

/**
 * @function to_return_statement_node
 *
 * Safely cast a generic node pointer to a return_statement_node_t
 * pointer after examining it's tag.
 */
return_statement_node_t* to_return_statement_node(parse_node_t* ptr) {
  if (ptr == NULL || ptr->tag != PARSE_NODE_RETURN_STATEMENT) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return cast(return_statement_node_t*, ptr);
}

// ----------------------------------------------------------------------

/**
 * @function make_while_statement
 *
 * Allocating a while statement node and set it's field values.
 */
while_statement_node_t* make_while_statement(parse_node_t* condition,
                                             parse_node_t* body) {
  while_statement_node_t* result = malloc_struct(while_statement_node_t);
  result->tag = PARSE_NODE_WHILE_STATEMENT;
  result->condition = condition;
  result->body = body;
  return result;
}

/**
 * @function to_while_statement_node
 *
 * Safely cast a generic node pointer to a while_statement_node_t
 * pointer after examining it's tag.
 */
while_statement_node_t* to_while_statement_node(parse_node_t* ptr) {
  if (ptr == NULL || ptr->tag != PARSE_NODE_WHILE_STATEMENT) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return cast(while_statement_node_t*, ptr);
}

// ----------------------------------------------------------------------

/**
 * @function make_do_statement
 *
 * Allocating a do statement node and set it's field values.
 */
do_statement_node_t* make_do_statement(parse_node_t* body,
                                       parse_node_t* condition) {
  do_statement_node_t* result = malloc_struct(do_statement_node_t);
  result->tag = PARSE_NODE_DO_STATEMENT;
  result->body = body;
  result->condition = condition;
  return result;
}

/**
 * @function to_do_statement_node
 *
 * Safely cast a generic node pointer to a do_statement_node_t
 * pointer after examining it's tag.
 */
do_statement_node_t* to_do_statement_node(parse_node_t* ptr) {
  if (ptr == NULL || ptr->tag != PARSE_NODE_DO_STATEMENT) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return cast(do_statement_node_t*, ptr);
}
