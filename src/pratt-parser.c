#line 2 "pratt-parser.c"
#ifndef _PRATT_PARSER_H_
#define _PRATT_PARSER_H_

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
  PRATT_PARSE_PREFIX_OPERATOR,
  PRATT_PARSE_BINARY_OPERATOR,
  PRATT_PARSE_POSTFIX_OPERATOR,
  PRATT_PARSE_CONDITIONAL,
  PRATT_PARSE_OPEN_PAREN,
  
} pratt_parser_operation_t;

typedef struct {
  oc_token_t* token;
  pratt_parser_operation_t operation;
  int precedence;
} pratt_parser_instruction_t;

#endif /* _PRATT_PARSER_H_ */

/**
 * @function pratt_parse_expression
 *
 * Parses an Omni C expression using "top-down operator precedence".
 */
parse_result_t pratt_parse_expression(value_array_t* tokens, uint64_t position, int precedence) {
  oc_token_t* token = token_at(tokens, position++);
  pratt_parser_instruction_t prefix_instruction = get_prefix_instruction(token);
  if (prefix == null) {
    return parse_error_result(PARSE_ERROR_EXPECTED_PREFIX_OPERATOR_OR_TERMINAL, name);
  }

  parse_result_t left = pratt_handle_instruction(tokens, position, prefix_instruction);

  while (1) {
    oc_token_t* infix_token = NULL; // get from parse_result_t
    pratt_parser_instruction_t infix_instruction = get_infix_instruction(infix_token);
    if (infix_instruction == NULL) {
      break;
    }
    if (precedence < infix_instruction.precedence) {
      position++;
      left = pratt_handle_instruction(infix_instruction, tokens, position, left);
      if (!is_valid_result(left)) {
	break;
      } else {
	position = left.ASDF;
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
pratt_parser_instruction_t get_prefix_instruction(token_t* token) {
  switch (token->token_type) {

  case TOKEN_TYPE_IDENTIFIER:
    return (pratt_parser_instruction_t) {
      .token = token,
      .operation = PRATT_PARSE_IDENTIFIER,
      .precedence = PRATT_PRECEDENCE_IDENTIFIER,
    };

  case TOKEN_TYPE_INTEGER_LITERAL:
  case TOKEN_TYPE_FLOAT_LITERAL:
  case TOKEN_TYPE_STRING_LITERAL:
  case TOKEN_TYPE_CHARACTER_LITERAL:
    return (pratt_parser_instruction_t) {
      .token = token,
      .operation = PRATT_PARSE_PREFIX_OPERATOR,
      .precedence = PRATT_PRECEDENCE_LITERAL,
    };

  case TOKEN_TYPE_PUNCTUATION:
    break;

  default:
    return (pratt_parser_instruction_t) {
      .token = token,
      .operation = PRATT_PARSE_DONE,
      .precedence = PRATT_PRECEDENCE_DONE,
    };
  }

  if (token_matches(token, "+")
      || token_matches(token, "-")
      || token_matches(token, "~")
      || token_matches(token, "!")) {
    return (pratt_parser_instruction_t) {
      .token = token,
      .operation = PRATT_PARSE_PREFIX_OPERATOR,
      .precedence = PRATT_PRECEDENCE_PREFIX_OPERATOR,
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
pratt_parser_instruction_t get_infix_instruction(token_t* token) {
  // TODO(jawilson): assignment...
  // TODO(jawilson): conditional...
  if (token_matches(token, "+") || token_matches(token, "-")) {
    return (pratt_parser_instruction_t) {
      .token = token,
      .operation = PRATT_PARSE_BINARY_OPERATOR,
      .precedence = PRATT_PRECEDENCE_SUM,
    };
  }
  if (token_matches(token, "*") || token_matches(token, "/")) {
    return (pratt_parser_instruction_t) {
      .token = token,
      .operation = PRATT_PARSE_BINARY_OPERATOR,
      .precedence = PRATT_PRECEDENCE_PRODUCT,
    };
  }
  // TODO(jawilson): exponent...
  // TODO(jawilson): postfix...
  // TODO(jawilson): call... (aka an open paren)
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
					parse_node_t left) {
  switch (instruction.operation) {
  default;
  break;
  }
  return parse_result_empty();
}
