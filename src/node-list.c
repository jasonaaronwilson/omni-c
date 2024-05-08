#line 2 "node-list.c"
#ifndef _NODE_LIST_H_
#define _NODE_LIST_H_

#include <c-armyknife-lib.h>

struct parse_node_s;

/**
 * @structure node_list_t
 *
 * A list of "child" parse nodes. (Currently implemented by wrapping
 * value_array_t*).
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
                                      struct parse_node_s* oc_node) {
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
 * Return true is the node list is empty.
 */
static inline boolean_t node_list_is_empty(node_list_t node_list) {
  return node_list_length(node_list) == 0;
}

/**
 * @function node_list_get
 *
 * Return the n-th element of the node list.
 */
static inline struct parse_node_s* node_list_get(node_list_t node_list,
                                                 uint64_t index) {
  if (node_list.list == NULL) {
    fatal_error(ERROR_ACCESS_OUT_OF_BOUNDS);
  }
  return cast(struct parse_node_s*, value_array_get(node_list.list, index).ptr);
}

#endif /* _NODE_LIST_H_ */
