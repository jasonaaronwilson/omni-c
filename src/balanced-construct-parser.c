#line 2 "balanced-construct-parser.c"
#ifndef _BALANCED_CONSTRUCT_PARSER_H_
#define _BALANCED_CONSTRUCT_PARSER_H_

#include <c-armyknife-lib.h>
#include <ctype.h>

#include "compiler-errors.h"
#include "parser.h"
#include "pstate.h"

/**
 * @structure balanced_construct_node_t
 *
 * Represents a region of the source code that wasn't parsed because
 * the parser is incomplete. These can still be included in the output
 * by grabbing the exact string between start_token and end_token
 * (inclusive) though this may include other junk like partial
 * portions of C preprocessor commands so it is far from perfect...
 */
typedef struct balanced_construct_node_S {
  parse_node_type_t tag;
  // These are used when the function body was parse via the
  // parse_balanced_construct.
  token_t* start_token;
  token_t* end_token;
} balanced_construct_node_t;

/**
 * @function to_balanced_construct_node
 *
 * Safely cast a generic node to a balanced_construct_node_t after
 * examining it's tag.
 */
static inline balanced_construct_node_t*
    to_balanced_construct_node(parse_node_t* ptr) {
  if (ptr == NULL || ptr->tag != PARSE_NODE_BALANCED_CONSTRUCT) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return cast(balanced_construct_node_t*, ptr);
}

/**
 * @function malloc_balanced_construct_node
 */
static inline balanced_construct_node_t* malloc_balanced_construct_node() {
  balanced_construct_node_t* result = malloc_struct(balanced_construct_node_t);
  result->tag = PARSE_NODE_BALANCED_CONSTRUCT;
  return result;
}

#include "balanced-construct-parser.c.generated.h"

#endif /* _BALANCED_CONSTRUCT_PARSER_H_ */

/**
 * @function parse_balanced_construct
 *
 * This is hack which allows us to parse just about anything between
 * delimiters like function bodies, etc. without having written a full
 * parser for them. While this is in use, the parser isn't complete.
 */
pstatus_t parse_balanced_construct(pstate_t* pstate) {
  uint64_t saved_position = pstate->position;

  int open_parens = 0;
  int open_brackets = 0;
  int open_braces = 0;
  do {
    if (pstate_expect_token_string(pstate, "(")) {
      open_parens++;
    } else if (pstate_expect_token_string(pstate_ignore_error(pstate), "[")) {
      open_brackets++;
    } else if (pstate_expect_token_string(pstate_ignore_error(pstate), "{")) {
      open_braces++;
    } else if (pstate_expect_token_string(pstate_ignore_error(pstate), ")")) {
      open_parens--;
    } else if (pstate_expect_token_string(pstate_ignore_error(pstate), "]")) {
      open_brackets--;
    } else if (pstate_expect_token_string(pstate_ignore_error(pstate), "}")) {
      open_braces--;
    } else {
      pstate_advance(pstate_ignore_error(pstate));
    }
  } while (open_parens + open_brackets + open_braces > 0);

  if (pstate->position == saved_position + 1) {
    // FIXME error type
    return pstate_error(pstate, saved_position,
                        PARSE_ERROR_OPEN_BRACE_EXPECTED);
  }

  balanced_construct_node_t* result = malloc_balanced_construct_node();
  result->start_token = token_at(pstate->tokens, saved_position);
  result->end_token = pstate_peek(pstate, -1);

  return pstate_set_result_node(pstate, to_node(result));
}
