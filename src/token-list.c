/**
 * @structure token_list_t
 *
 * A list of of token_t's.
 */
typedef struct token_list_S {
  value_array_t* list;
} token_list_t;

/* ====================================================================== */
/* The inlined token_list implementation */
/* ====================================================================== */

/**
 * @function token_list_add
 *
 * Append a token to a list (typically a field of a particular kind of
 * token). In this case we need the address of that field though
 * reading is done without the extra address.
 */
static inline void token_list_add(token_list_t* token_list, token_t* token) {
  if (token == NULL) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  if (token_list->list == NULL) {
    token_list->list = make_value_array(2);
  }
  value_array_add(token_list->list, ptr_to_value(token));
}

/**
 * @function token_list_length
 *
 * Determine the length of the token list.
 */
static inline uint64_t token_list_length(token_list_t token_list) {
  if (token_list.list == NULL) {
    return 0;
  }
  return token_list.list->length;
}

/**
 * @function token_list_is_empty
 *
 * Return true if the token list is empty.
 */
static inline boolean_t token_list_is_empty(token_list_t token_list) {
  return token_list_length(token_list) == 0;
}

/**
 * @function token_list_get
 *
 * Return the n-th element of the token list.
 */
static inline token_t* token_list_get(token_list_t token_list, uint64_t index) {
  if (token_list.list == NULL) {
    fatal_error(ERROR_ACCESS_OUT_OF_BOUNDS);
  }
  return value_array_get_ptr(token_list.list, index, typeof(token_t*));
}

/**
 * @function token_list_contains
 *
 * Return true if the token list contains a token with the given text.
 */
static inline boolean_t token_list_contains(token_list_t token_list,
                                            char* token_string) {
  for (int i = 0; i < token_list_length(token_list); i++) {
    if (token_matches(token_list_get(token_list, i), token_string)) {
      return true;
    }
  }
  return false;
}
