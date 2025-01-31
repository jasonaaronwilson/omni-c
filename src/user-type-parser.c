/**
 * @structure struct_node_t
 *
 * Represents a partial or full structure declaration.
 */
typedef struct struct_node_S {
  parse_node_type_t tag;
  token_t* name;
  node_list_t fields;
  boolean_t partial_definition;
} struct_node_t;

/**
 * @structure union_node_t
 *
 * Represents a partial or full union declaration.
 */
typedef struct union_node_S {
  parse_node_type_t tag;
  token_t* name;
  node_list_t fields;
  boolean_t partial_definition;
} union_node_t;

/**
 * @structure field_node_t
 *
 * Represents a field in a structure or union definition.
 *
 * When bit_field_width is non-zero, the field is a bit field (is this
 * legal with a union nodes?).
 */
typedef struct field_node_S {
  parse_node_type_t tag;
  type_node_t* type;
  token_t* name;
  token_t* bit_field_width;
  // TODO(jawilson): put suffixes into the type
  value_array_t* suffixes;
} field_node_t;

/**
 * @structure enum_node_t
 *
 * Represents an enum reference or definition.
 */
typedef struct enum_node_S {
  parse_node_type_t tag;
  token_t* name;
  node_list_t elements;
  boolean_t partial_definition;
} enum_node_t;

/**
 * @structure enum_element_t
 *
 * Represents one "enumerator", enum constant, enum member, etc. Name
 * is not optional. Value is optional.
 */
typedef struct enum_element_S {
  parse_node_type_t tag;
  token_t* name;
  parse_node_t* value_expr;
} enum_element_t;

/**
 * @function is_struct_node
 *
 * Helper to determine if a node is a struct node (for example before
 * trying to cast it using to_struct_node).
 */
static inline boolean_t is_struct_node(parse_node_t* ptr) {
  return (ptr != NULL) && (ptr->tag == PARSE_NODE_STRUCT);
}

/**
 * @function is_enum_node
 *
 * Helper to determine if a node is an enum node (for example before
 * trying to cast it using to_enum_node).
 */
static inline boolean_t is_enum_node(parse_node_t* ptr) {
  return (ptr != NULL) && (ptr->tag == PARSE_NODE_ENUM);
}

/**
 * @function to_struct_node
 *
 * Safely cast a generic node to a struct node after examining it's
 * tag.
 */
static inline struct_node_t* to_struct_node(parse_node_t* ptr) {
  // TODO(jawilson): figure this ugliness out...
  if (ptr == NULL
      || !(ptr->tag == PARSE_NODE_STRUCT || ptr->tag == PARSE_NODE_UNION)) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return cast(struct_node_t*, ptr);
}

/**
 * @function to_union_node
 *
 * Safely cast a generic node to a struct node after examining it's
 * tag.
 */
static inline union_node_t* to_union_node(parse_node_t* ptr) {
  if (ptr == NULL || ptr->tag != PARSE_NODE_UNION) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return cast(union_node_t*, ptr);
}

/**
 * @function to_field_node
 *
 * Safely cast a generic node to a struct node after examining it's
 * tag.
 */
static inline field_node_t* to_field_node(parse_node_t* ptr) {
  if (ptr == NULL || ptr->tag != PARSE_NODE_FIELD) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return cast(field_node_t*, ptr);
}

/**
 * @function to_enum_node
 *
 * Safely cast a generic node to an enum_node_t* node after examining
 * it's tag.
 */
static inline enum_node_t* to_enum_node(parse_node_t* ptr) {
  if (ptr == NULL || ptr->tag != PARSE_NODE_ENUM) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return cast(enum_node_t*, ptr);
}

/**
 * @function to_enum_element_node
 *
 * Safely cast a generic node to an enum_element_t* after examining
 * it's tag.
 */
static inline enum_element_t* to_enum_element_node(parse_node_t* ptr) {
  if (ptr == NULL || ptr->tag != PARSE_NODE_ENUM_ELEMENT) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return cast(enum_element_t*, ptr);
}

static inline enum_node_t* malloc_enum_node(void) {
  enum_node_t* result = malloc_struct(enum_node_t);
  result->tag = PARSE_NODE_ENUM;
  return result;
}

static inline enum_element_t* malloc_enum_element(void) {
  enum_element_t* result = malloc_struct(enum_element_t);
  result->tag = PARSE_NODE_ENUM_ELEMENT;
  return result;
}

static inline struct_node_t* malloc_struct_node(void) {
  struct_node_t* result = malloc_struct(struct_node_t);
  result->tag = PARSE_NODE_STRUCT;
  return result;
}

static inline union_node_t* malloc_union_node(void) {
  union_node_t* result = malloc_struct(union_node_t);
  result->tag = PARSE_NODE_UNION;
  return result;
}

static inline field_node_t* malloc_field_node(void) {
  field_node_t* result = malloc_struct(field_node_t);
  result->tag = PARSE_NODE_FIELD;
  return result;
}

/**
 * @function parse_structure_node
 */
pstatus_t parse_structure_node(pstate_t* pstate) {
  uint64_t saved_position = pstate->position;
  if (!pstate_expect_token_string(pstate, "struct")) {
    return pstate_propagate_error(pstate, saved_position);
  }
  struct_node_t* result = malloc_struct_node();

  if (pstate_expect_token_type(pstate, TOKEN_TYPE_IDENTIFIER)) {
    result->name = pstate_get_result_token(pstate);
  } else {
    pstate_ignore_error(pstate);
  }

  if (pstate_expect_token_string(pstate, "{")) {
    result->partial_definition = false;
    while (true) {
      if (pstate_expect_token_string(pstate, "}")) {
        break;
      }
      pstate_ignore_error(pstate);
      if (!parse_field_node(pstate)) {
        return pstate_propagate_error(pstate, saved_position);
      }
      node_list_add_node(&result->fields, pstate_get_result_node(pstate));
    }
  } else {
    result->partial_definition = true;
    pstate_ignore_error(pstate);
  }

  return pstate_set_result_node(pstate, to_node(result));
}

/**
 * @function parse_field_node
 */
pstatus_t parse_field_node(pstate_t* pstate) {
  uint64_t saved_position = pstate->position;
  if (!parse_type_node(pstate)) {
    return pstate_propagate_error(pstate, saved_position);
  }
  type_node_t* field_type = to_type_node(pstate_get_result_node(pstate));
  if (!pstate_expect_token_type(pstate, TOKEN_TYPE_IDENTIFIER)) {
    log_warn("Allowing unnamed field in case the type is a union");
    pstate_ignore_error(pstate);
    // return pstate_propagate_error(pstate, saved_position);
  }
  token_t* field_name = pstate_get_result_token(pstate);
  if (pstate_expect_token_string(pstate, ":")) {
    // TODO(jawilson): capture field width here.
    pstate_advance(pstate);
  } else {
    pstate_ignore_error(pstate);
  }

  value_array_t* suffixes = NULL;
  // Handle internal arrays inside of structures
  while (pstate_match_token_string(pstate, "[")) {
    if (!parse_balanced_construct(pstate)) {
      return pstate_propagate_error(pstate, saved_position);
    }
    if (suffixes == NULL) {
      suffixes = make_value_array(1);
    }
    value_array_add(suffixes, ptr_to_value(pstate_get_result_node(pstate)));
  }

  if (!pstate_expect_token_string(pstate, ";")) {
    return pstate_propagate_error(pstate, saved_position);
  }
  field_node_t* result = malloc_field_node();
  result->type = field_type;
  result->name = field_name;
  result->suffixes = suffixes;
  return pstate_set_result_node(pstate, to_node(result));
}

/**
 * @function parse_union_node
 */
pstatus_t parse_union_node(pstate_t* pstate) {
  uint64_t saved_position = pstate->position;
  if (!pstate_expect_token_string(pstate, "union")) {
    return pstate_propagate_error(pstate, saved_position);
  }
  union_node_t* result = malloc_union_node();

  if (pstate_expect_token_type(pstate, TOKEN_TYPE_IDENTIFIER)) {
    result->name = pstate_get_result_token(pstate);
  } else {
    pstate_ignore_error(pstate);
  }

  if (pstate_expect_token_string(pstate, "{")) {
    result->partial_definition = false;
    while (true) {
      if (pstate_expect_token_string(pstate, "}")) {
        break;
      }
      pstate_ignore_error(pstate);
      if (!parse_field_node(pstate)) {
        return pstate_propagate_error(pstate, saved_position);
      }
      node_list_add_node(&result->fields, pstate_get_result_node(pstate));
    }
  } else {
    result->partial_definition = true;
    pstate_ignore_error(pstate);
  }

  return pstate_set_result_node(pstate, to_node(result));
}

/**
 * @function parse_user_type_node
 *
 * Parses an enum, struct or (eventually) union.
 */
pstatus_t parse_user_type_node(pstate_t* pstate) {
  uint64_t saved_position = pstate->position;
  if (parse_enum_node(pstate)
      || parse_structure_node(pstate_ignore_error(pstate))
      || parse_union_node(pstate_ignore_error(pstate))) {
    return true;
  } else {
    return pstate_propagate_error(pstate, saved_position);
  }
}

/**
 * Parse a node representing something that starts with the "enum"
 * keyword. This can be used as part of a typedef or else part of an
 * enum declaration.
 */
pstatus_t parse_enum_node(pstate_t* pstate) {
  uint64_t saved_position = pstate->position;
  if (!pstate_expect_token_string(pstate, "enum")) {
    return pstate_propagate_error(pstate, saved_position);
  }
  enum_node_t* result = malloc_enum_node();

  if (pstate_expect_token_type(pstate, TOKEN_TYPE_IDENTIFIER)) {
    result->name = pstate_get_result_token(pstate);
  } else {
    pstate_ignore_error(pstate);
  }

  if (!pstate_expect_token_string(pstate, "{")) {
    pstate_ignore_error(pstate);
    result->partial_definition = true;
    return pstate_set_result_node(pstate, to_node(result));
  }

  while (true) {
    if (pstate_expect_token_string(pstate, "}")) {
      break;
    }
    pstate_ignore_error(pstate);
    if (!parse_enum_element_node(pstate)) {
      return pstate_propagate_error(pstate, saved_position);
    }
    node_list_add_node(&result->elements, pstate_get_result_node(pstate));
  }

  return pstate_set_result_node(pstate, to_node(result));
}

/**
 * @function parse_enum_element_node
 *
 * Parse an enum element with an optional expression (hopefully
 * constant) and eat any trailing comma.
 */
pstatus_t parse_enum_element_node(pstate_t* pstate) {
  uint64_t saved_position = pstate->position;
  if (!pstate_expect_token_type(pstate, TOKEN_TYPE_IDENTIFIER)) {
    return pstate_propagate_error(pstate, saved_position);
  }
  token_t* name = pstate_get_result_token(pstate);
  parse_node_t* value_expr = NULL;
  if (pstate_expect_token_string(pstate, "=")) {
    if (!pratt_parse_expression(pstate, 0)) {
      return pstate_propagate_error(pstate, saved_position);
    }
    value_expr = pstate_get_result_node(pstate);
  }

  if (!pstate_expect_token_string(pstate, ",")) {
    pstate_ignore_error(pstate);
  }

  enum_element_t* result = malloc_enum_element();
  result->name = name;
  result->value_expr = value_expr;

  return pstate_set_result_node(pstate, to_node(result));
}
