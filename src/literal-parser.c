/**
 * @structure literal_node_t
 *
 * Represents a source code literal such as an integer/floating-point
 * number, character constant, a string literal, or a complex
 * initializer like {1, 2}.
 */

typedef literal_node_t = struct {
  parse_node_type_t tag;
  token_t* token;
  // This is used for string literals since in C multiple string
  // literals can appear in sequence and should be treated like a
  // single literal if these literals were smushed into one token.
  value_array_t* tokens;
  parse_node_t* initializer_node;
  parse_node_t* initializer_type;
};

// (type_node) { initializer-list }
typedef compound_literal_node_t = struct {
  parse_node_type_t tag;
  parse_node_t* type_node;
  // Expressions or designated_initializer_node_t
  node_list_t initializers;
};

typedef designated_initializer_node_t = struct {
  parse_node_type_t tag;
  // For [index] = value
  parse_node_t* index_expression;
  // For structures/union .member_name = value
  token_t* member_name;
  parse_node_t* value;
};

static inline literal_node_t* make_literal_node(void) {
  literal_node_t* result = malloc_struct(literal_node_t);
  result->tag = PARSE_NODE_LITERAL;
  return result;
}

static inline compound_literal_node_t* make_compound_literal_node(void) {
  compound_literal_node_t* result = malloc_struct(compound_literal_node_t);
  result->tag = PARSE_NODE_COMPOUND_LITERAL;
  return result;
}

static inline compound_literal_node_t*
    to_compound_literal_node(parse_node_t* ptr) {
  if (ptr == nullptr || ptr->tag != PARSE_NODE_COMPOUND_LITERAL) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return cast(compound_literal_node_t*, ptr);
}

static inline designated_initializer_node_t*
    make_designated_initializer_node(void) {
  designated_initializer_node_t* result
      = malloc_struct(designated_initializer_node_t);
  result->tag = PARSE_NODE_DESIGNATED_INITIALIZER;
  return result;
}

static inline designated_initializer_node_t*
    to_designated_initializer_node(parse_node_t* ptr) {
  if (ptr == nullptr || ptr->tag != PARSE_NODE_DESIGNATED_INITIALIZER) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return cast(designated_initializer_node_t*, ptr);
}

pstatus_t parse_literal_node(pstate_t* pstate) {
  uint64_t saved_position = pstate->position;
  if (pstate_expect_token_type(pstate, TOKEN_TYPE_STRING_LITERAL)) {
    literal_node_t* result = make_literal_node();
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
    literal_node_t* result = make_literal_node();
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
    literal_node_t* result = make_literal_node();
    result->initializer_node = pstate_get_result_node(pstate);
    return pstate_set_result_node(pstate, to_node(result));
  }

  // This is the "macro" syntax for compound_literals, not a real
  // compound_literal that is fully parse. We'd like to get rid of
  // this hack at some point.
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

    literal_node_t* result = make_literal_node();
    result->initializer_node = initializer_node;
    result->initializer_type = type_node;
    return pstate_set_result_node(pstate, to_node(result));
  }

  if (parse_compound_literal(pstate)) {
    return true;
  } else {
    pstate_ignore_error(pstate);
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
    literal_node_t* result = make_literal_node();
    result->token = pstate_get_result_token(pstate);
    return pstate_set_result_node(pstate, to_node(result));
  }

  return pstate_error(pstate, saved_position, PARSE_ERROR_NOT_LITERAL_NODE);
}

// (type) { initializer-list }
pstatus_t parse_compound_literal(pstate_t* pstate) {
  log_info("Trying to parse a casted compound literal...");

  uint64_t saved_position = pstate->position;
  if (!pstate_expect_token_string(pstate, "(")) {
    log_info("Failed to match expected token '('...");
    return pstate_propagate_error(pstate, saved_position);
  }
  log_info("MATCHED token '('...");
  if (!parse_type_node(pstate)) {
    log_info("Failed to match a parse type node...");
    return pstate_propagate_error(pstate, saved_position);
  }
  log_info("MATCHED type...");
  parse_node_t* type_node = pstate_get_result_node(pstate);
  if (!pstate_expect_token_string(pstate, ")")) {
    log_info("Failed to match expected token ')'...");
    return pstate_propagate_error(pstate, saved_position);
  }
  log_info("MATCHED ')'...");
  if (!pstate_expect_token_string(pstate, "{")) {
    log_info("Failed to match expected token '{'...");
    return pstate_propagate_error(pstate, saved_position);
  }
  log_info("MATCHED '{'...");
  node_list_t initializers = compound_literal(node_list_t, {0});

  while (true) {
    if (pstate_is_eof(pstate)) {
      log_info("Didn't find a matching '}' before EOF...");
      return pstate_propagate_error(pstate, saved_position);
    }
    if (pstate_match_token_string(pstate, "}")) {
      log_info("MATCHED expected token '}'...");
      pstate_advance(pstate);
      break;
    }

    if (node_list_length(initializers) > 0) {
      if (!pstate_expect_token_string(pstate, ",")) {
        log_info("Failed to match expected token ','...");
        return pstate_propagate_error(pstate, saved_position);
      }
      log_info("MATCHED expected token ','...");
    }

    if (parse_designated_initializer_node(pstate)) {
      log_info("MATCHED designated initializer...");
      parse_node_t* initializer = pstate_get_result_node(pstate);
      node_list_add_node(&initializers, initializer);
    } else {
      log_info(
          "Failed to match a designated initializer (trying literal node)...");
      if (parse_literal_node(pstate)) {
        log_info("MATCHED literal...");
        parse_node_t* initializer = pstate_get_result_node(pstate);
        node_list_add_node(&initializers, initializer);
      } else {
        log_info("Failed to match designated initializer or literal...");
        return pstate_error(pstate, saved_position,
                            PARSE_ERROR_BAD_INITIALIZER);
      }
    }
  }

  compound_literal_node_t* result = make_compound_literal_node();
  result->type_node = type_node;
  result->initializers = initializers;
  pstate_set_result_node(pstate, to_node(result));

  return true;
}

// .member_name = expression
pstatus_t parse_designated_initializer_node(pstate_t* pstate) {
  log_info("Trying to parse a designated_initializer_node...");

  uint64_t saved_position = pstate->position;

  // TODO(jawilson): [N] = ...

  if (!pstate_expect_token_string(pstate, ".")) {
    return pstate_propagate_error(pstate, saved_position);
  }
  if (!pstate_expect_token_type(pstate, TOKEN_TYPE_IDENTIFIER)) {
    return pstate_propagate_error(pstate, saved_position);
  }

  token_t* member_name = pstate_get_result_token(pstate);

  if (!pstate_expect_token_string(pstate, "=")) {
    return pstate_propagate_error(pstate, saved_position);
  }

  if (!parse_expression(pstate)) {
    return pstate_propagate_error(pstate, saved_position);
  }

  designated_initializer_node_t* result = make_designated_initializer_node();
  result->member_name = member_name;
  result->value = pstate_get_result_node(pstate);
  pstate_set_result_node(pstate, to_node(result));

  return true;
}
