/**
 * @structure variable_definition_node_t
 *
 * Represents a variable definition/declaration at the top-level or
 * inside of a function.
 */
typedef variable_definition_node_t = struct {
  parse_node_type_t tag;
  boolean_t is_global;
  token_t* storage_class_specifier; // static, extern, auto, register
  type_node_t* type;
  token_t* name;
  parse_node_t* value;
  // TODO(jawilson): put suffixes into the type
  value_array_t* suffixes;
};

/**
 * @function to_variable_definition_node
 *
 * Safely cast a generic node to a variable_definition_node_t after
 * examining it's tag.
 */
static inline variable_definition_node_t*
    to_variable_definition_node(parse_node_t* ptr) {
  if (ptr == nullptr || ptr->tag != PARSE_NODE_VARIABLE_DEFINITION) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return cast(variable_definition_node_t*, ptr);
}

static inline variable_definition_node_t* make_variable_definition_node(void) {
  variable_definition_node_t* result
      = malloc_struct(variable_definition_node_t);
  result->tag = PARSE_NODE_VARIABLE_DEFINITION;
  return result;
}

/**
 * @function parse_expression
 *
 * Parses an expression.
 */
pstatus_t parse_expression(pstate_t* pstate) {
  return pratt_parse_expression(pstate, 0);
}

/**
 * @function parse_initializer
 *
 * Parses an initializer like {0, 1, 3}
 */
pstatus_t parse_initializer(pstate_t* pstate) {
  // TODO(jawilson): implement full parsing of initializers
  uint64_t saved_position = pstate->position;
  if (token_matches(pstate_peek(pstate, 0), "{")) {
    return parse_balanced_construct(pstate);
  }
  return pstate_error(pstate, saved_position,
                      PARSE_ERROR_CLOSE_BRACKET_EXPECTED);
}


/**
 * @function parse_variable_definition_node
 *
 * Parses a global or local variable.
 */
pstatus_t parse_variable_definition_node(pstate_t* pstate) {
  uint64_t saved_position = pstate->position;
  token_t* storage_class_specifier = nullptr;

  while (pstate_expect_token_string(pstate, "static")
         || pstate_expect_token_string(pstate_ignore_error(pstate), "extern")) {
    if (storage_class_specifier != nullptr) {
      return pstate_error(pstate, saved_position,
                          PARSE_ERROR_CONFLICTING_STORAGE_CLASS_SPECIFIER);
    }
    storage_class_specifier = pstate_get_result_token(pstate);
  }
  pstate_ignore_error(pstate);
  if (!parse_type_node(pstate)) {
    return pstate_propagate_error(pstate, saved_position);
  }
  type_node_t* type_node = to_type_node(pstate_get_result_node(pstate));
  if (!pstate_expect_token_type(pstate, TOKEN_TYPE_IDENTIFIER)) {
    return pstate_error(pstate, saved_position,
                        PARSE_ERROR_IDENTIFIER_EXPECTED);
  }
  token_t* name = pstate_get_result_token(pstate);

  variable_definition_node_t* result = make_variable_definition_node();
  result->type = type_node;
  result->name = name;

  while (pstate_match_token_string(pstate, "[")) {
    if (!parse_balanced_construct(pstate)) {
      return pstate_propagate_error(pstate, saved_position);
    }
    if (result->suffixes == nullptr) {
      result->suffixes = make_value_array(1);
    }
    value_array_add(result->suffixes,
                    ptr_to_value(pstate_get_result_node(pstate)));
  }

  if (pstate_expect_token_string(pstate, "=")) {
    if (parse_initializer(pstate)
        || parse_expression(pstate_ignore_error(pstate))) {
      result->value = pstate_get_result_node(pstate);
    } else {
      return pstate_propagate_error(pstate, saved_position);
    }
  } else {
    pstate_ignore_error(pstate);
  }

  if (!pstate_expect_token_string(pstate, ";")) {
    return pstate_propagate_error(pstate, saved_position);
  }

  return pstate_set_result_node(pstate, to_node(result));
}
