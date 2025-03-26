/**
 * @file node-list.c
 *
 * This tries to provide "zero is valid" semantics so automatic
 * allocation of the underlying value_array_t is done rather than just
 * allocate an array when the parent is created. In terms of clarity,
 * I think this fails on several fronts.
 *
 * It's nice in that fewer unsafe casts are done because we use the
 * somewhat (dynmaically safeish) to_XYZ_node "upcasts".
 *
 * I would never have tried this if we simply had generic type safe
 * arrays with bounds checking (declared like array_t<parse_node_t*>).
 * 
 * It's kind of a failure TBH. Luckily the forward looking plan was
 * always to get rid of this class in favor of array_t<parse_node_t*>.
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
  if (oc_node == nullptr) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  if (node_list->list == nullptr) {
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
  if (node_list.list == nullptr) {
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
  if (node_list.list == nullptr) {
    fatal_error(ERROR_ACCESS_OUT_OF_BOUNDS);
  }
  return value_array_get_ptr(node_list.list, index, typeof(parse_node_t*));
}
