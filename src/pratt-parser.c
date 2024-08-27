#line 2 "pratt-parser.c"
#ifndef _PRATT_PARSER_H_
#define _PRATT_PARSER_H_

#include <c-armyknife-lib.h>

#include "compiler-errors.h"
#include "literal-parser.h"
#include "parser.h"

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
} pratt_parser_operation_t;

typedef enum {
  LEFT_TO_RIGHT,
  RIGHT_TO_LEFT,
} associativity_t;

// clang-format off
typedef enum {
  PRECEDENCE_UNKNOWN,        
  PRECEDENCE_COMMA,          // LEFT_TO_RIGHT ---- ,
  PRECEDENCE_ASSIGNMENT,     // RIGHT_TO_LEFT ---- = += -= *= /= %= &= ^= |= <<= >>=
  PRECEDENCE_CONDITIONAL,    // RIGHT_TO_LEFT ---- ?:
  PRECEDENCE_LOGICAL_OR,     // LEFT_TO_RIGHT ---- ||
  PRECEDENCE_LOGICAL_AND,    // LEFT_TO_RIGHT ---- &&
  PRECEDENCE_OR,             // LEFT_TO_RIGHT ---- |
  PRECEDENCE_XOR,            // LEFT_TO_RIGHT ---- ^
  PRECEDENCE_AND,            // LEFT_TO_RIGHT ---- &
  PRECEDENCE_EQUALITY,       // LEFT_TO_RIGHT ---- == !=
  PRECEDENCE_RELATIONAL,     // LEFT_TO_RIGHT ---- < <= > >=
  PRECEDENCE_SHIFT,          // LEFT_TO_RIGHT ---- << >>
  PRECEDENCE_ADDITIVE,       // LEFT_TO_RIGHT ---- + -
  PRECEDENCE_MULTIPICITIVE,  // LEFT_TO_RIGHT ---- * / %
  PRECEDENCE_UNARY,          // RIGHT_TO_LEFT ---- ! ~ ++ -- + - * & (cast) sizeof
  PRECEDENCE_PRIMARY,        // LEFT_TO_RIGHT ---- () [] -> . (literals and identifiers)
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

// pratt_parser_instruction_t get_prefix_instruction(token_t* token);
// pratt_parser_instruction_t get_infix_instruction(token_t* token);

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
        recursive_precedence--;
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
      [[gnu::unused]] int recursive_precedence = instruction.precedence;
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
    return (pratt_parser_instruction_t){
        .token = token,
        .operation = PRATT_PARSE_IDENTIFIER,
        .precedence = PRECEDENCE_PRIMARY,
    };

  case TOKEN_TYPE_INTEGER_LITERAL:
  case TOKEN_TYPE_FLOAT_LITERAL:
  case TOKEN_TYPE_STRING_LITERAL:
  case TOKEN_TYPE_CHARACTER_LITERAL:
    return (pratt_parser_instruction_t){
        .token = token,
        .operation = PRATT_PARSE_LITERAL,
        .precedence = PRECEDENCE_PRIMARY,
    };

  case TOKEN_TYPE_PUNCTUATION:
    break;

  default:
    return (pratt_parser_instruction_t){
        .token = token,
        .operation = PRATT_PARSE_UNKNOWN,
        .precedence = PRECEDENCE_UNKNOWN,
    };
  }

  if (token_matches(token, "(")) {
    return (pratt_parser_instruction_t){
        .token = token,
        .operation = PRATT_PARSE_SUB_EXPRESSION,
        .precedence = PRECEDENCE_PRIMARY,
    };
  }

  // also need (cast) and sizeof
  if (token_matches(token, "+") || token_matches(token, "-")
      || token_matches(token, "~") || token_matches(token, "!")
      || token_matches(token, "!") || token_matches(token, "++")
      || token_matches(token, "--") || token_matches(token, "*")
      || token_matches(token, "&")) {
    return (pratt_parser_instruction_t){
        .token = token,
        .operation = PRATT_PARSE_PREFIX_OPERATOR,
        .precedence = PRECEDENCE_UNARY,
    };
  }
  return (pratt_parser_instruction_t){0};
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
    return (pratt_parser_instruction_t){
        .token = token,
        .operation = PRATT_PARSE_BINARY_OPERATOR,
        .precedence = PRECEDENCE_ADDITIVE,
    };
  }
  if (token_matches(token, "*") || token_matches(token, "/")
      || token_matches(token, "%")) {
    return (pratt_parser_instruction_t){
        .token = token,
        .operation = PRATT_PARSE_BINARY_OPERATOR,
        .precedence = PRECEDENCE_MULTIPICITIVE,
    };
  }
  if (token_matches(token, "<<") || token_matches(token, ">>")) {
    return (pratt_parser_instruction_t){
        .token = token,
        .operation = PRATT_PARSE_BINARY_OPERATOR,
        .precedence = PRECEDENCE_SHIFT,
    };
  }
  if (token_matches(token, "||")) {
    return (pratt_parser_instruction_t){
        .token = token,
        .operation = PRATT_PARSE_BINARY_OPERATOR,
        .precedence = PRECEDENCE_LOGICAL_OR,
    };
  }
  if (token_matches(token, "&&")) {
    return (pratt_parser_instruction_t){
        .token = token,
        .operation = PRATT_PARSE_BINARY_OPERATOR,
        .precedence = PRECEDENCE_LOGICAL_AND,
    };
  }
  if (token_matches(token, "|")) {
    return (pratt_parser_instruction_t){
        .token = token,
        .operation = PRATT_PARSE_BINARY_OPERATOR,
        .precedence = PRECEDENCE_OR,
    };
  }
  if (token_matches(token, "^")) {
    return (pratt_parser_instruction_t){
        .token = token,
        .operation = PRATT_PARSE_BINARY_OPERATOR,
        .precedence = PRECEDENCE_XOR,
    };
  }
  if (token_matches(token, "&")) {
    return (pratt_parser_instruction_t){
        .token = token,
        .operation = PRATT_PARSE_BINARY_OPERATOR,
        .precedence = PRECEDENCE_AND,
    };
  }
  if (token_matches(token, "==") || token_matches(token, "!=")) {
    return (pratt_parser_instruction_t){
        .token = token,
        .operation = PRATT_PARSE_BINARY_OPERATOR,
        .precedence = PRECEDENCE_EQUALITY,
    };
  }
  if (token_matches(token, "<") || token_matches(token, "<=")
      || token_matches(token, ">") || token_matches(token, ">=")) {
    return (pratt_parser_instruction_t){
        .token = token,
        .operation = PRATT_PARSE_BINARY_OPERATOR,
        .precedence = PRECEDENCE_RELATIONAL,
    };
  }
  if (token_matches(token, "->") || token_matches(token, ".")) {
    return (pratt_parser_instruction_t){
        .token = token,
        .operation = PRATT_PARSE_BINARY_OPERATOR,
        .precedence = PRECEDENCE_PRIMARY,
    };
  }
  if (token_matches(token, "++") || token_matches(token, "--")) {
    return (pratt_parser_instruction_t){
        .token = token,
        .operation = PRATT_PARSE_POSTFIX_OPERATOR,
        .precedence = PRECEDENCE_UNARY,
    };
  }
  if (token_matches(token, "=") || token_matches(token, "+=")
      || token_matches(token, "-=") || token_matches(token, "*=")
      || token_matches(token, "/=") || token_matches(token, "%=")
      || token_matches(token, "&=") || token_matches(token, "^=")
      || token_matches(token, "|=") || token_matches(token, "<<=")
      || token_matches(token, ">>=")) {
    return (pratt_parser_instruction_t){
        .token = token,
        .operation = PRATT_PARSE_BINARY_OPERATOR,
        .precedence = PRECEDENCE_ASSIGNMENT,
    };
  }
  if (token_matches(token, "[")) {
    return (pratt_parser_instruction_t){
        .token = token,
        .operation = PRATT_PARSE_INDEX_EXPRESSION,
        .precedence = PRECEDENCE_PRIMARY,
    };
  }

  return (pratt_parser_instruction_t){0};
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
