#line 2 "pratt-parser.c"
#ifndef _PRATT_PARSER_H_
#define _PRATT_PARSER_H_

#include <c-armyknife-lib.h>

#include "compiler-errors.h"
#include "literal-parser.h"
#include "parser.h"
#include "type-parser.h"

/**
 * @file pratt-parser.c
 *
 * This file implements expression parsing, typically a source of some
 * amount of inefficiency in some compilers. Instead we use the Pratt
 * parsing algorithm (similar to shunting yard) to parse expressions.
 *
 * See:
 * https://journal.stuffwithstuff.com/2011/03/19/pratt-parsers-expression-parsing-made-easy/
 */

typedef enum {
  PRATT_PARSE_UNKNOWN,
  PRATT_PARSE_PREFIX_OPERATOR,
  PRATT_PARSE_BINARY_OPERATOR,
  PRATT_PARSE_POSTFIX_OPERATOR,
  PRATT_PARSE_CONDITIONAL,
  PRATT_PARSE_IDENTIFIER,
  PRATT_PARSE_LITERAL,
  PRATT_PARSE_SUB_EXPRESSION,
  PRATT_PARSE_INDEX_EXPRESSION,
  PRATT_PARSE_SIZEOF,
  PRATT_PARSE_CAST_MACRO,
  PRATT_PARSE_TYPE_OF,
  PRATT_PARSE_CALL,
} pratt_parser_operation_t;

typedef enum {
  LEFT_TO_RIGHT,
  RIGHT_TO_LEFT,
} associativity_t;

// clang-format off
typedef enum {
  PRECEDENCE_UNKNOWN = 0,
  PRECEDENCE_COMMA = 2,          // LEFT_TO_RIGHT ---- ,
  PRECEDENCE_ASSIGNMENT = 4,     // RIGHT_TO_LEFT ---- = += -= *= /= %= &= ^= |= <<= >>=
  PRECEDENCE_CONDITIONAL = 6,    // RIGHT_TO_LEFT ---- ?:
  PRECEDENCE_LOGICAL_OR = 8,     // LEFT_TO_RIGHT ---- ||
  PRECEDENCE_LOGICAL_AND = 10,   // LEFT_TO_RIGHT ---- &&
  PRECEDENCE_OR = 12,            // LEFT_TO_RIGHT ---- |
  PRECEDENCE_XOR = 14,           // LEFT_TO_RIGHT ---- ^
  PRECEDENCE_AND = 16,           // LEFT_TO_RIGHT ---- &
  PRECEDENCE_EQUALITY = 18,      // LEFT_TO_RIGHT ---- == !=
  PRECEDENCE_RELATIONAL = 20,    // LEFT_TO_RIGHT ---- < <= > >=
  PRECEDENCE_SHIFT = 22,         // LEFT_TO_RIGHT ---- << >>
  PRECEDENCE_ADDITIVE = 24,      // LEFT_TO_RIGHT ---- + -
  PRECEDENCE_MULTIPICITIVE = 26, // LEFT_TO_RIGHT ---- * / %
  PRECEDENCE_UNARY = 28,         // RIGHT_TO_LEFT ---- ! ~ ++ -- + - * & (cast) sizeof
  PRECEDENCE_PRIMARY = 30,       // LEFT_TO_RIGHT ---- () [] -> . (literals and identifiers)
} precedence_t;
// clang-format on

typedef struct {
  token_t* token;
  pratt_parser_operation_t operation;
  precedence_t precedence;
} pratt_parser_instruction_t;

/**
 * @structure identifier_node_t
 *
 * Represents an identifier (variable, named constant, etc.). In other
 * parts of the parser we generally didn't need these since usually
 * just having the token itself was sufficient however this fits the
 * paradigm better for expressions.
 */
typedef struct {
  parse_node_type_t tag;
  token_t* token;
} identifier_node_t;

/**
 * @structure operator_node_t
 *
 * Represents A + B, etc.
 */
typedef struct {
  parse_node_type_t tag;
  token_t* operator;
  parse_node_t* left;
  parse_node_t* right;
} operator_node_t;

/**
 * @structure call_node_t
 *
 * Represents a function call.
 */
typedef struct {
  parse_node_type_t tag;
  parse_node_t* function;
  node_list_t args;
} call_node_t;

/**
 * @structure conditional_node_t
 *
 * Represents a conditional (ternary) operator.
 */
typedef struct {
  parse_node_type_t tag;
  parse_node_t* condition;
  parse_node_t* expr_if_true;
  parse_node_t* expr_if_false;
} conditional_node_t;

static inline operator_node_t* malloc_operator_node(void) {
  operator_node_t* result = malloc_struct(operator_node_t);
  result->tag = PARSE_NODE_OPERATOR;
  return result;
}

static inline identifier_node_t* malloc_identifier_node(void) {
  identifier_node_t* result = malloc_struct(identifier_node_t);
  result->tag = PARSE_NODE_IDENTIFIER;
  return result;
}

static inline call_node_t* malloc_call_node(void) {
  call_node_t* result = malloc_struct(call_node_t);
  result->tag = PARSE_NODE_CALL;
  return result;
}

static inline conditional_node_t* malloc_conditional_node(void) {
  conditional_node_t* result = malloc_struct(conditional_node_t);
  result->tag = PARSE_NODE_CONDITIONAL;
  return result;
}

/**
 * @function to_identifier_node
 *
 * Safely cast a generic node to an identifier node after examining
 * it's tag.
 */
static inline identifier_node_t* to_identifier_node(parse_node_t* ptr) {
  if (ptr == NULL || ptr->tag != PARSE_NODE_IDENTIFIER) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return cast(identifier_node_t*, ptr);
}

/**
 * @function to_operator_node
 *
 * Safely cast a generic node to a binary operator node after
 * examining it's tag.
 */
static inline operator_node_t* to_operator_node(parse_node_t* ptr) {
  if (ptr == NULL || ptr->tag != PARSE_NODE_OPERATOR) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return cast(operator_node_t*, ptr);
}

/**
 * @function to_operator_node
 *
 * Safely cast a generic node to a function call node after examining
 * it's tag.
 */
static inline call_node_t* to_call_node(parse_node_t* ptr) {
  if (ptr == NULL || ptr->tag != PARSE_NODE_CALL) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return cast(call_node_t*, ptr);
}

/**
 * @function to_conditional_node
 *
 * Safely cast a generic node to a conditional node after examining
 * it's tag.
 */
static inline conditional_node_t* to_conditional_node(parse_node_t* ptr) {
  if (ptr == NULL || ptr->tag != PARSE_NODE_CONDITIONAL) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return cast(conditional_node_t*, ptr);
}

static inline pratt_parser_instruction_t
    make_parser_instruction(token_t* token, pratt_parser_operation_t operation,
                            precedence_t precedence) {
  return compound_literal(
      pratt_parser_instruction_t,
      {.token = token, .operation = operation, .precedence = precedence});
}

#include "pratt-parser.c.generated.h"

#endif /* _PRATT_PARSER_H_ */

/**
 * @function pratt_parse_expression
 *
 * Parse a C "expression" with the "Pratt" algorithm.
 */
pstatus_t pratt_parse_expression(pstate_t* pstate, int precedence) {
  uint64_t saved_position = pstate->position;
  token_t* token = pstate_peek(pstate, 0);
  if (token == NULL) {
    return pstate_error(pstate, saved_position, PARSE_ERROR_EOF);
  }
  pratt_parser_instruction_t prefix_instruction = get_prefix_instruction(token);
  if (prefix_instruction.operation == PRATT_PARSE_UNKNOWN) {
    log_warn("(RETURNING ERROR) No prefix for %s\n", token_to_string(token));
    return pstate_error(pstate, saved_position,
                        PARSE_ERROR_EXPECTED_PREFIX_OPERATOR_OR_TERMINAL);
  }

  if (!pratt_handle_instruction(pstate, prefix_instruction, NULL)) {
    log_warn("(RETURNING ERROR) handle instruction\n", token_to_string(token));
    return pstate_propagate_error(pstate, saved_position);
  }

  parse_node_t* left = pstate_get_result_node(pstate);

  while (1) {
    pratt_parser_instruction_t infix_instruction
        = get_infix_instruction(pstate_peek(pstate, 0));
    if (infix_instruction.operation == PRATT_PARSE_UNKNOWN
        || (precedence >= infix_instruction.precedence)) {
      return pstate_set_result_node(pstate, left);
    }
    if (!pratt_handle_instruction(pstate, infix_instruction, left)) {
      return pstate_propagate_error(pstate, saved_position);
    }
    left = pstate_get_result_node(pstate);
  }

  /* NOT REACHED */
  return NULL;
}

/**
 * @function pratt_handle_instruction
 *
 * This is where the work specified in the "instruction" is performed.
 */
pstatus_t pratt_handle_instruction(pstate_t* pstate,
                                   pratt_parser_instruction_t instruction,
                                   parse_node_t* left) {
  uint64_t saved_position = pstate->position;
  token_t* token = pstate_peek(pstate, 0);

  switch (instruction.operation) {
  case PRATT_PARSE_BINARY_OPERATOR:
    do {
      pstate_advance(pstate);
      int recursive_precedence = instruction.precedence;
      if (precedence_to_associativity(recursive_precedence) == LEFT_TO_RIGHT) {
        recursive_precedence++;
      }
      if (!pratt_parse_expression(pstate, recursive_precedence)) {
        return pstate_propagate_error(pstate, saved_position);
      }
      operator_node_t* result = malloc_operator_node();
      result->operator= token;
      result->left = left;
      result->right = pstate_get_result_node(pstate);
      return pstate_set_result_node(pstate, to_node(result));
    } while (0);

  case PRATT_PARSE_IDENTIFIER:
    do {
      pstate_advance(pstate);
      identifier_node_t* result = malloc_identifier_node();
      result->token = token;
      return pstate_set_result_node(pstate, to_node(result));
    } while (0);

  case PRATT_PARSE_LITERAL:
    return parse_literal_node(pstate);

  case PRATT_PARSE_PREFIX_OPERATOR:
    do {
      pstate_advance(pstate);
      if (!pratt_parse_expression(pstate, instruction.precedence)) {
        return pstate_propagate_error(pstate, saved_position);
      }
      operator_node_t* result = malloc_operator_node();
      result->operator= token;
      if (left != NULL) {
        fatal_error(ERROR_ILLEGAL_STATE);
      }
      result->left = NULL;
      result->right = pstate_get_result_node(pstate);
      return pstate_set_result_node(pstate, to_node(result));
    } while (0);

  case PRATT_PARSE_POSTFIX_OPERATOR:
    do {
      pstate_advance(pstate);
      operator_node_t* result = malloc_operator_node();
      result->operator= token;
      result->left = left;
      result->right = NULL;
      return pstate_set_result_node(pstate, to_node(result));
    } while (0);

  case PRATT_PARSE_SUB_EXPRESSION:
    do {
      if (!pstate_expect_token_string(pstate, "(")) {
        fatal_error(ERROR_ILLEGAL_STATE);
      }
      if (!pratt_parse_expression(pstate, 0)) {
        return pstate_propagate_error(pstate, saved_position);
      }
      parse_node_t* inner_expression = pstate_get_result_node(pstate);
      if (!pstate_expect_token_string(pstate, ")")) {
        return pstate_propagate_error(pstate, saved_position);
      }
      return pstate_set_result_node(pstate, inner_expression);
    } while (0);
    break;

  case PRATT_PARSE_INDEX_EXPRESSION:
    do {
      if (!pstate_expect_token_string(pstate, "[")) {
        fatal_error(ERROR_ILLEGAL_STATE);
      }
      if (!pratt_parse_expression(pstate, 0)) {
        return pstate_propagate_error(pstate, saved_position);
      }
      parse_node_t* inner_expression = pstate_get_result_node(pstate);
      if (!pstate_expect_token_string(pstate, "]")) {
        return pstate_propagate_error(pstate, saved_position);
      }
      operator_node_t* result = malloc_operator_node();
      result->operator= token;
      result->left = left;
      result->right = inner_expression;
      return pstate_set_result_node(pstate, to_node(result));
    } while (0);
    break;

  case PRATT_PARSE_SIZEOF:
    do {
      if (!pstate_expect_token_string(pstate, "sizeof")) {
        fatal_error(ERROR_ILLEGAL_STATE);
      }
      if (!pstate_expect_token_string(pstate, "(")) {
        return pstate_propagate_error(pstate, saved_position);
      }
      if (!pratt_parse_expression(pstate, 0)) {
        return pstate_propagate_error(pstate, saved_position);
      }
      parse_node_t* inner_expression = pstate_get_result_node(pstate);
      if (!pstate_expect_token_string(pstate, ")")) {
        return pstate_propagate_error(pstate, saved_position);
      }
      operator_node_t* result = malloc_operator_node();
      result->operator= token;
      result->right = inner_expression;
      return pstate_set_result_node(pstate, to_node(result));
    } while (0);
    break;

  case PRATT_PARSE_CAST_MACRO:
    do {
      if (!pstate_expect_token_string(pstate, "cast")) {
        fatal_error(ERROR_ILLEGAL_STATE);
      }
      if (!pstate_expect_token_string(pstate, "(")) {
        return pstate_propagate_error(pstate, saved_position);
      }
      if (!parse_type_node(pstate)) {
        return pstate_propagate_error(pstate, saved_position);
      }
      parse_node_t* type_node = pstate_get_result_node(pstate);
      if (!pstate_expect_token_string(pstate, ",")
          || !pratt_parse_expression(pstate, PRECEDENCE_ASSIGNMENT)) {
        return pstate_propagate_error(pstate, saved_position);
      }
      parse_node_t* expression = pstate_get_result_node(pstate);
      if (!pstate_expect_token_string(pstate, ")")) {
        return pstate_propagate_error(pstate, saved_position);
      }
      operator_node_t* result = malloc_operator_node();
      result->operator= token;
      result->left = type_node;
      result->right = expression;
      return pstate_set_result_node(pstate, to_node(result));
    } while (0);
    break;

  case PRATT_PARSE_TYPE_OF:
    do {
      if (!pstate_expect_token_string(pstate, "typeof")) {
        fatal_error(ERROR_ILLEGAL_STATE);
      }
      if (!pstate_expect_token_string(pstate, "(")) {
        return pstate_propagate_error(pstate, saved_position);
      }
      if (!parse_type_node(pstate)) {
        return pstate_propagate_error(pstate, saved_position);
      }
      parse_node_t* type_node = pstate_get_result_node(pstate);
      if (!pstate_expect_token_string(pstate, ")")) {
        return pstate_propagate_error(pstate, saved_position);
      }
      operator_node_t* result = malloc_operator_node();
      result->operator= token;
      result->left = type_node;
      return pstate_set_result_node(pstate, to_node(result));
    } while (0);
    break;


  case PRATT_PARSE_CALL:
    do {
      if (!pstate_expect_token_string(pstate, "(")) {
        fatal_error(ERROR_ILLEGAL_STATE);
      }
      call_node_t* result = malloc_call_node();
      result->function = left;
      boolean_t expect_comma = false;
      while (!token_matches(pstate_peek(pstate, 0), ")")) {
        if (expect_comma) {
          if (!pstate_expect_token_string(pstate, ",")) {
            return pstate_propagate_error(pstate, saved_position);
          }
        } else {
          expect_comma = true;
        }
        if (!pratt_parse_expression(pstate, PRECEDENCE_ASSIGNMENT)) {
          return pstate_propagate_error(pstate, saved_position);
        }
        node_list_add_node(&result->args, pstate_get_result_node(pstate));
      }
      if (!pstate_expect_token_string(pstate, ")")) {
        return pstate_propagate_error(pstate, saved_position);
      }
      return pstate_set_result_node(pstate, to_node(result));
    } while (0);
    break;

  case PRATT_PARSE_CONDITIONAL:
    do {
      pstate_advance(pstate);
      int recursive_precedence = instruction.precedence;
      if (!pratt_parse_expression(pstate, recursive_precedence)) {
        return pstate_propagate_error(pstate, saved_position);
      }
      parse_node_t* expr_if_true = pstate_get_result_node(pstate);
      if (!pstate_expect_token_string(pstate, ":")
          || !pratt_parse_expression(pstate, recursive_precedence)) {
        return pstate_propagate_error(pstate, saved_position);
      }
      parse_node_t* expr_if_false = pstate_get_result_node(pstate);
      conditional_node_t* result = malloc_conditional_node();
      result->condition = left;
      result->expr_if_true = expr_if_true;
      result->expr_if_false = expr_if_false;
      return pstate_set_result_node(pstate, to_node(result));
    } while (0);

  default:
    break;
  }
  return pstate_error(pstate, saved_position,
                      PARSE_ERROR_UNHANDLED_INSTRUCTION);
}

/* ====================================================================== */
/*                These are effectively table lookups...                  */
/* ====================================================================== */

/**
 * @function get_prefix_instruction
 *
 * Essentially a table lookup on the token type (and the token string
 * since we don't have a unique type for each token though we could do
 * that in the future...)
 */
pratt_parser_instruction_t get_prefix_instruction(token_t* token) {
  switch (token->type) {

  case TOKEN_TYPE_IDENTIFIER:
    if (token_matches(token, "sizeof")) {
      return make_parser_instruction(token, PRATT_PARSE_SIZEOF,
                                     PRECEDENCE_UNARY);
    }
    if (token_matches(token, "cast")) {
      return make_parser_instruction(token, PRATT_PARSE_CAST_MACRO,
                                     PRECEDENCE_UNARY);
    }
    if (token_matches(token, "typeof")) {
      return make_parser_instruction(token, PRATT_PARSE_TYPE_OF,
                                     PRECEDENCE_UNARY);
    }
    if (token_matches(token, "compound_literal")) {
      return make_parser_instruction(token, PRATT_PARSE_LITERAL,
                                     PRECEDENCE_UNARY);
    }
    return make_parser_instruction(token, PRATT_PARSE_IDENTIFIER,
                                   PRECEDENCE_PRIMARY);

  case TOKEN_TYPE_INTEGER_LITERAL:
  case TOKEN_TYPE_FLOAT_LITERAL:
  case TOKEN_TYPE_STRING_LITERAL:
  case TOKEN_TYPE_CHARACTER_LITERAL:
    return make_parser_instruction(token, PRATT_PARSE_LITERAL,
                                   PRECEDENCE_PRIMARY);

  case TOKEN_TYPE_PUNCTUATION:
    break;

  default:
    return make_parser_instruction(token, PRATT_PARSE_UNKNOWN,
                                   PRECEDENCE_UNKNOWN);
  }

  if (token_matches(token, "(")) {
    return make_parser_instruction(token, PRATT_PARSE_SUB_EXPRESSION,
                                   PRECEDENCE_PRIMARY);
  }

  // also need (cast)
  if (token_matches(token, "+") || token_matches(token, "-")
      || token_matches(token, "~") || token_matches(token, "!")
      || token_matches(token, "!") || token_matches(token, "++")
      || token_matches(token, "--") || token_matches(token, "*")
      || token_matches(token, "&")) {
    return make_parser_instruction(token, PRATT_PARSE_PREFIX_OPERATOR,
                                   PRECEDENCE_UNARY);
  }

  return compound_literal(pratt_parser_instruction_t, {0});
}

/**
 * @function get_infix_instruction
 *
 * Essentially a table lookup on the token type (and the token string
 * since we don't have a unique type for each token though we could do
 * that in the future...)
 */
pratt_parser_instruction_t get_infix_instruction(token_t* token) {
  if (token_matches(token, "+") || token_matches(token, "-")) {
    return make_parser_instruction(token, PRATT_PARSE_BINARY_OPERATOR,
                                   PRECEDENCE_ADDITIVE);
  }
  if (token_matches(token, "*") || token_matches(token, "/")
      || token_matches(token, "%")) {
    return make_parser_instruction(token, PRATT_PARSE_BINARY_OPERATOR,
                                   PRECEDENCE_MULTIPICITIVE);
  }
  if (token_matches(token, "<<") || token_matches(token, ">>")) {
    return make_parser_instruction(token, PRATT_PARSE_BINARY_OPERATOR,
                                   PRECEDENCE_SHIFT);
  }
  if (token_matches(token, "||")) {
    return make_parser_instruction(token, PRATT_PARSE_BINARY_OPERATOR,
                                   PRECEDENCE_LOGICAL_OR);
  }
  if (token_matches(token, "&&")) {
    return make_parser_instruction(token, PRATT_PARSE_BINARY_OPERATOR,
                                   PRECEDENCE_LOGICAL_AND);
  }
  if (token_matches(token, "|")) {
    return make_parser_instruction(token, PRATT_PARSE_BINARY_OPERATOR,
                                   PRECEDENCE_OR);
  }
  if (token_matches(token, "^")) {
    return make_parser_instruction(token, PRATT_PARSE_BINARY_OPERATOR,
                                   PRECEDENCE_XOR);
  }
  if (token_matches(token, "&")) {
    return make_parser_instruction(token, PRATT_PARSE_BINARY_OPERATOR,
                                   PRECEDENCE_AND);
  }
  if (token_matches(token, "==") || token_matches(token, "!=")) {
    return make_parser_instruction(token, PRATT_PARSE_BINARY_OPERATOR,
                                   PRECEDENCE_EQUALITY);
  }
  if (token_matches(token, "<") || token_matches(token, "<=")
      || token_matches(token, ">") || token_matches(token, ">=")) {
    return make_parser_instruction(token, PRATT_PARSE_BINARY_OPERATOR,
                                   PRECEDENCE_RELATIONAL);
  }
  if (token_matches(token, "->") || token_matches(token, ".")) {
    return make_parser_instruction(token, PRATT_PARSE_BINARY_OPERATOR,
                                   PRECEDENCE_PRIMARY);
  }
  if (token_matches(token, "++") || token_matches(token, "--")) {
    return make_parser_instruction(token, PRATT_PARSE_POSTFIX_OPERATOR,
                                   PRECEDENCE_UNARY);
  }
  if (token_matches(token, "=") || token_matches(token, "+=")
      || token_matches(token, "-=") || token_matches(token, "*=")
      || token_matches(token, "/=") || token_matches(token, "%=")
      || token_matches(token, "&=") || token_matches(token, "^=")
      || token_matches(token, "|=") || token_matches(token, "<<=")
      || token_matches(token, ">>=")) {
    return make_parser_instruction(token, PRATT_PARSE_BINARY_OPERATOR,
                                   PRECEDENCE_ASSIGNMENT);
  }
  if (token_matches(token, "[")) {
    return make_parser_instruction(token, PRATT_PARSE_INDEX_EXPRESSION,
                                   PRECEDENCE_PRIMARY);
  }
  if (token_matches(token, "(")) {
    return make_parser_instruction(token, PRATT_PARSE_CALL, PRECEDENCE_PRIMARY);
  }
  if (token_matches(token, "?")) {
    return make_parser_instruction(token, PRATT_PARSE_CONDITIONAL,
                                   PRECEDENCE_CONDITIONAL);
  }

  return compound_literal(pratt_parser_instruction_t, {0});
}

/**
 * @function precedence_to_associativity
 *
 * Associativity determines how we treat repeated sequences with
 * operators at the same level.
 *
 * A OP B OP C.
 *
 * LEFT_TO_RIGHT would be `(A OP B) OP C` while RIGHT_TO_LEFT would be
 * `A OP (B OP C)`
 */
associativity_t precedence_to_associativity(precedence_t precedence) {
  switch (precedence) {
  case PRECEDENCE_PRIMARY:
    return LEFT_TO_RIGHT;
  case PRECEDENCE_UNARY:
    return RIGHT_TO_LEFT;
  case PRECEDENCE_MULTIPICITIVE:
    return LEFT_TO_RIGHT;
  case PRECEDENCE_ADDITIVE:
    return LEFT_TO_RIGHT;
  case PRECEDENCE_SHIFT:
    return LEFT_TO_RIGHT;
  case PRECEDENCE_RELATIONAL:
    return LEFT_TO_RIGHT;
  case PRECEDENCE_EQUALITY:
    return LEFT_TO_RIGHT;
  case PRECEDENCE_AND:
    return LEFT_TO_RIGHT;
  case PRECEDENCE_XOR:
    return LEFT_TO_RIGHT;
  case PRECEDENCE_OR:
    return LEFT_TO_RIGHT;
  case PRECEDENCE_LOGICAL_AND:
    return LEFT_TO_RIGHT;
  case PRECEDENCE_LOGICAL_OR:
    return LEFT_TO_RIGHT;
  case PRECEDENCE_CONDITIONAL:
    return RIGHT_TO_LEFT;
  case PRECEDENCE_ASSIGNMENT:
    return RIGHT_TO_LEFT;
  case PRECEDENCE_COMMA:
    return LEFT_TO_RIGHT;
  default:
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return 0;
}
