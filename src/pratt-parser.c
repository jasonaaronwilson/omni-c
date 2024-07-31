#line 2 "pratt-parser.c"
#ifndef _PRATT_PARSER_H_
#define _PRATT_PARSER_H_

#include <c-armyknife-lib.h>

#include "parser.h"
#include "compiler-errors.h"

/**
 * @file pratt-parser.c
 *
 * This file implements expression parsing, typically a source of some
 * amount of inefficiency in some compilers. Instead we use the Pratt
 * parsing algorithm (similar to shunting yard) to parse expressions.
 *
 * See: https://journal.stuffwithstuff.com/2011/03/19/pratt-parsers-expression-parsing-made-easy/
 */

typedef enum {
  PRATT_PARSE_UNKNOWN,
  PRATT_PARSE_PREFIX_OPERATOR,
  PRATT_PARSE_BINARY_OPERATOR,
  PRATT_PARSE_POSTFIX_OPERATOR,
  PRATT_PARSE_CONDITIONAL,
  PRATT_PARSE_OPEN_PAREN,
  PRATT_PARSE_IDENTIFIER,
  
} pratt_parser_operation_t;

typedef enum {
  LEFT_TO_RIGHT,
  RIGHT_TO_LEFT,
} associativity_t;

// clang-format off
typedef enum {
  PRECEDENCE_UNKNOWN,        
  PRECEDENCE_PRIMARY,        // LEFT_TO_RIGHT ---- () [] -> . (literals and identifiers)
  PRECEDENCE_UNARY,          // RIGHT_TO_LEFT ---- ! ~ ++ -- + - * & (cast) sizeof
  PRECEDENCE_MULTIPICITIVE,  // LEFT_TO_RIGHT ---- * / %
  PRECEDENCE_ADDITIVE,       // LEFT_TO_RIGHT ---- + -
  PRECEDENCE_SHIFT,          // LEFT_TO_RIGHT ---- << >>
  PRECEDENCE_RELATIONAL,     // LEFT_TO_RIGHT ---- < <= > >=
  PRECEDENCE_EQUALITY,       // LEFT_TO_RIGHT ---- == !=
  PRECEDENCE_AND,            // LEFT_TO_RIGHT ---- &
  PRECEDENCE_XOR,            // LEFT_TO_RIGHT ---- ^
  PRECEDENCE_OR,             // LEFT_TO_RIGHT ---- |
  PRECEDENCE_LOGICAL_AND,    // LEFT_TO_RIGHT ---- &&
  PRECEDENCE_LOGICAL_OR,     // LEFT_TO_RIGHT ---- ||
  PRECEDENCE_CONDITIONAL,    // RIGHT_TO_LEFT ---- ?:
  PRECEDENCE_ASSIGNMENT,     // RIGHT_TO_LEFT ---- = += -= *= /= %= &= ^= |= <<= >>=
  PRECEDENCE_COMMA,          // LEFT_TO_RIGHT ---- ,
} precedence_t;
// clang-format on

typedef struct {
  oc_token_t* token;
  pratt_parser_operation_t operation;
  precedence_t precedence;
} pratt_parser_instruction_t;

#endif /* _PRATT_PARSER_H_ */

pratt_parser_instruction_t get_prefix_instruction(oc_token_t* token);
pratt_parser_instruction_t get_infix_instruction(oc_token_t* token);
parse_result_t pratt_handle_instruction(pratt_parser_instruction_t instruction,
					value_array_t* tokens, 
					uint64_t position,
					parse_node_t* left);

/**
 * @function pratt_parse_expression
 *
 * Parses an Omni C expression using "top-down operator precedence".
 */
parse_result_t pratt_parse_expression(value_array_t* tokens, uint64_t position, int precedence) {
  oc_token_t* token = token_at(tokens, position++);
  pratt_parser_instruction_t prefix_instruction = get_prefix_instruction(token);
  if (prefix_instruction.operation == PRATT_PARSE_UNKNOWN) {
    return parse_error_result(PARSE_ERROR_EXPECTED_PREFIX_OPERATOR_OR_TERMINAL, token);
  }

  parse_result_t left = pratt_handle_instruction(prefix_instruction, tokens, position, NULL);
  if (!is_valid_result(left)) {
    return left;
  }
  position = left.next_token_position;

  while (1) {
    oc_token_t* infix_token = token_at(tokens, position);
    pratt_parser_instruction_t infix_instruction = get_infix_instruction(infix_token);
    if (infix_instruction.operation == PRATT_PARSE_UNKNOWN) {
      break;
    }
    if (precedence < infix_instruction.precedence) {
      position++;
      left = pratt_handle_instruction(infix_instruction, tokens, position, left.node);
      if (!is_valid_result(left)) {
	return left;
      } else {
	position = left.next_token_position;
      }
    }
  }

  return left;
}

/**
 * @function get_prefix_instruction
 *
 * Essentially a table lookup on the token type (and the token string
 * since we don't have a unique type for each token though we could do
 * that in the future...)
 */
pratt_parser_instruction_t get_prefix_instruction(oc_token_t* token) {
  switch (token->type) {

  case TOKEN_TYPE_IDENTIFIER:
    return (pratt_parser_instruction_t) {
      .token = token,
      .operation = PRATT_PARSE_IDENTIFIER,
      .precedence = PRECEDENCE_PRIMARY,
    };

  case TOKEN_TYPE_INTEGER_LITERAL:
  case TOKEN_TYPE_FLOAT_LITERAL:
  case TOKEN_TYPE_STRING_LITERAL:
  case TOKEN_TYPE_CHARACTER_LITERAL:
    return (pratt_parser_instruction_t) {
      .token = token,
      .operation = PRATT_PARSE_PREFIX_OPERATOR,
      .precedence = PRECEDENCE_PRIMARY,
    };

  case TOKEN_TYPE_PUNCTUATION:
    break;

  default:
    return (pratt_parser_instruction_t) {
      .token = token,
      .operation = PRATT_PARSE_UNKNOWN,
      .precedence = PRECEDENCE_UNKNOWN,
    };
  }

  if (token_matches(token, "+")
      || token_matches(token, "-")
      || token_matches(token, "~")
      || token_matches(token, "!")) {
    return (pratt_parser_instruction_t) {
      .token = token,
      .operation = PRATT_PARSE_PREFIX_OPERATOR,
      .precedence = PRECEDENCE_UNARY,
    };
  }
}

/**
 * @function get_infix_instruction
 *
 * Essentially a table lookup on the token type (and the token string
 * since we don't have a unique type for each token though we could do
 * that in the future...)
 */
pratt_parser_instruction_t get_infix_instruction(oc_token_t* token) {
  if (token_matches(token, "+") || token_matches(token, "-")) {
    return (pratt_parser_instruction_t) {
      .token = token,
      .operation = PRATT_PARSE_BINARY_OPERATOR,
      .precedence = PRECEDENCE_ADDITIVE,
    };
  }
  if (token_matches(token, "*") || token_matches(token, "/")) {
    return (pratt_parser_instruction_t) {
      .token = token,
      .operation = PRATT_PARSE_BINARY_OPERATOR,
      .precedence = PRECEDENCE_MULTIPICITIVE,
    };
  }
  fatal_error(ERROR_ILLEGAL_STATE);
}

/**
 * @function pratt_handle_instruction
 *
 * This is where the work specified in the operation contained is
 * performed.
 */
parse_result_t pratt_handle_instruction(pratt_parser_instruction_t instruction,
					value_array_t* tokens, 
					uint64_t position,
					parse_node_t* left) {
  switch (instruction.operation) {
  default:
    break;
  }
  return parse_result_empty();
}
