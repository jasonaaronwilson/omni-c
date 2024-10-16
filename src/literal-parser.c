#line 2 "literal-parser.c"
#ifndef _LITERAL_PARSER_H_
#define _LITERAL_PARSER_H_

#include <c-armyknife-lib.h>
#include <ctype.h>

#include "balanced-construct-parser.h"
#include "compiler-errors.h"
#include "parser.h"
#include "pstate.h"

/**
 * @structure literal_node_t
 *
 * Represents a source code literal such as an integer/floating-point
 * number, character constant, a string literal, or a complex
 * initializer like {1, 2}.
 */
typedef struct literal_node_S {
  parse_node_type_t tag;
  token_t* token;
  // This is used for string literals since in C multiple string
  // literals can appear in sequence and should be treated like a
  // single literal if these literals were smushed into one token.
  value_array_t* tokens;
  parse_node_t* initializer_node;
  parse_node_t* initializer_type;
} literal_node_t;

static inline literal_node_t* malloc_literal_node(void) {
  literal_node_t* result = malloc_struct(literal_node_t);
  result->tag = PARSE_NODE_LITERAL;
  return result;
}

#include "literal-parser.c.generated.h"

#endif /* _LITERAL_PARSER_H_ */

#include "type-parser.h"

pstatus_t parse_literal_node(pstate_t* pstate) {
  uint64_t saved_position = pstate->position;
  if (pstate_expect_token_type(pstate, TOKEN_TYPE_STRING_LITERAL)) {
    literal_node_t* result = malloc_literal_node();
    result->tokens = make_value_array(1);
    value_array_add(result->tokens,
                    ptr_to_value(pstate_get_result_token(pstate)));
    while (pstate_expect_token_type(pstate, TOKEN_TYPE_STRING_LITERAL)) {
      value_array_add(result->tokens,
                      ptr_to_value(pstate_get_result_token(pstate)));
    }
    pstate_ignore_error(pstate);
    return pstate_set_result_node(pstate, to_node(result));
  }

  if (pstate_expect_token_type(pstate_ignore_error(pstate),
                               TOKEN_TYPE_INTEGER_LITERAL)
      || pstate_expect_token_type(pstate_ignore_error(pstate),
                                  TOKEN_TYPE_FLOAT_LITERAL)
      || pstate_expect_token_type(pstate_ignore_error(pstate),
                                  TOKEN_TYPE_CHARACTER_LITERAL)) {
    literal_node_t* result = malloc_literal_node();
    result->tokens = make_value_array(1);
    value_array_add(result->tokens,
                    ptr_to_value(pstate_get_result_token(pstate)));
    return pstate_set_result_node(pstate, to_node(result));
  } else {
    pstate_ignore_error(pstate);
  }

  if (pstate_match_token_string(pstate, "{")) {
    if (!parse_balanced_construct(pstate)) {
      return pstate_propagate_error(pstate, saved_position);
    }
    literal_node_t* result = malloc_literal_node();
    result->initializer_node = pstate_get_result_node(pstate);
    return pstate_set_result_node(pstate, to_node(result));
  }

  if (pstate_match_token_string(pstate, "compound_literal")) {
    pstate_advance(pstate);
    if (!pstate_expect_token_string(pstate, "(") || !parse_type_node(pstate)) {
      return pstate_propagate_error(pstate, saved_position);
    }
    parse_node_t* type_node = pstate_get_result_node(pstate);
    if (!pstate_expect_token_string(pstate, ",")) {
      return pstate_propagate_error(pstate, saved_position);
    }
    if (!parse_balanced_construct(pstate)) {
      return pstate_propagate_error(pstate, saved_position);
    }
    parse_node_t* initializer_node = pstate_get_result_node(pstate);
    if (!pstate_expect_token_string(pstate, ")")) {
      return pstate_propagate_error(pstate, saved_position);
    }

    literal_node_t* result = malloc_literal_node();
    result->initializer_node = initializer_node;
    result->initializer_type = type_node;
    return pstate_set_result_node(pstate, to_node(result));
  }

  if (pstate_expect_token_string(pstate, "NULL")
      || pstate_expect_token_string(pstate_ignore_error(pstate), "nullptr")
      || pstate_expect_token_string(pstate_ignore_error(pstate), "true")
      || pstate_expect_token_string(pstate_ignore_error(pstate), "false")
      || pstate_expect_token_type(pstate_ignore_error(pstate),
                                  TOKEN_TYPE_INTEGER_LITERAL)
      || pstate_expect_token_type(pstate_ignore_error(pstate),
                                  TOKEN_TYPE_FLOAT_LITERAL)
      || pstate_expect_token_type(pstate_ignore_error(pstate),
                                  TOKEN_TYPE_CHARACTER_LITERAL)) {
    literal_node_t* result = malloc_literal_node();
    result->token = pstate_get_result_token(pstate);
    return pstate_set_result_node(pstate, to_node(result));
  }

  return pstate_error(pstate, saved_position, PARSE_ERROR_NOT_LITERAL_NODE);
}
