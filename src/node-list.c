/**
 * @file node-list.c
 *
 * Because omni-c's growable arrays aren't generic, this provides a
 * handy wrapper that consolidates some annoying type-casting to one
 * place and also works better as a structure member since we delay
 * the creation of a backing value_array_t* until something is added
 * to the node-list.
 */

/**
 * @structure node_list_t
 *
 * A list of of parse_node_t*
 */
typedef struct node_list_S {
  value_array_t* list;
} node_list_t;

/* ====================================================================== */
/* The inlined node_list implementation */
/* ====================================================================== */

/**
 * @function node_list_add_node
 *
 * Append a node to a list (typically a field of a particular kind of
 * node). In this case we need the address of that field though
 * reading is done without the extra address.
 */
static inline void node_list_add_node(node_list_t* node_list,
                                      parse_node_t* oc_node) {
  if (oc_node == NULL) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  if (node_list->list == NULL) {
    node_list->list = make_value_array(2);
  }
  value_array_add(node_list->list, ptr_to_value(oc_node));
}

/**
 * @function node_list_length
 *
 * Determine the length of the node list.
 */
static inline uint64_t node_list_length(node_list_t node_list) {
  if (node_list.list == NULL) {
    return 0;
  }
  return node_list.list->length;
}

/**
 * @function node_list_is_empty
 *
 * Return true if the node list is empty.
 */
static inline boolean_t node_list_is_empty(node_list_t node_list) {
  return node_list_length(node_list) == 0;
}

/**
 * @function node_list_get
 *
 * Return the n-th element of the node list.
 */
static inline parse_node_t* node_list_get(node_list_t node_list,
                                          uint64_t index) {
  if (node_list.list == NULL) {
    fatal_error(ERROR_ACCESS_OUT_OF_BOUNDS);
  }
  return value_array_get_ptr(node_list.list, index, typeof(parse_node_t*));
}
