#line 2 "parser.c"
#ifndef _PARSER_H_
#define _PARSER_H_

#include <c-armyknife-lib.h>
#include <ctype.h>
#include "lexer.h"

/**
 * @file parser
 *
 * This is a hand-written parser which is meant to eventually fully
 * parse Omni C and ISO C (and maybe even parts of C++ that aren't
 * actually legal Omni C code for FFI reasons). It is intended to work
 * with "lexer.c" in this same directory.
 *
 * The first version of this parser will be used to parse enough
 * "simplified" ISO C so that we can properly extract high level
 * constructs like structs to automatically generate header files and
 * automatically generate code (such as the parsing and printing
 * enumerations).
 */
typedef enum {
  OC_NODE_UNKNOWN,
  OC_NODE_LIST_OF_NODES,
  OC_NODE_STRUCT,
  OC_NODE_UNION,
  OC_NODE_GLOBAL_VARIABLE,
  OC_NODE_GLOBAL_FUNCTION,
} parse_node_type_t;

/**
 * @structure oc_node_list_t
 *
 * We will implement lists of nodes by wrapping value_array_t to have
 * an interface that is simpler to use (as long as a structure begins
 * it's life zeroed out which using alloc_struct does nicely and there
 * are patterns for when not using pointers).
 */
typedef struct node_list_S {
  // I'm not sure we need a tag since I'm not sure we need this to be
  // a direct child of union in oc_node_t.
  //
  // parse_node_type_t tag;
  //
  value_array_t* list;
} node_list_t;

/**
 * @structure oc_node_list_t
 *
 */
typedef struct struct_node_S {
  parse_node_type_t tag;
  char *name; // this will be null for anonymous structs...
  node_list_t fields; 
} struct_node_t;

/**
 * @structure oc_node_list_t
 *
 */
typedef struct union_node_S {
  parse_node_type_t tag;
  char *name; // this will be null for anonymous unions...
  node_list_t members; 
} struct_node_t;

/**
 * @structure oc_node_t
 *
 * This represents an Omni C parse node.
 *
 * In a dynamically typed language (i.e. Scheme) without inheritance,
 * you wouldn't even have this this type (Scheme would typically just
 * use an s-expression based format and then provide getters and
 * setters if it is felt that is needed and might end up with some
 * routines with a case expression or cond expression if
 * necessary).
 *
 * In languages with algebraic types (i.e. Haskell), you might have
 * this type but not really need to deal with it too much.
 *
 * In OOP languages (i.e. C++, Java or even dynamically typed
 * languages with inheritance like Ruby), you would probably have a
 * base class and then extend from it.
 *
 * We are slightly like OOP because each member of the union begins
 * with a tag which is similar to how OOP always (?) has a vtable at
 * the begining of an object. From experience, we already know the
 * compiler will do an insufficient job at type-checking so our backup
 * plan is lot's of dynamic type-checking based on that tag field. In
 * particular, even if cast_oc_node() can't be written in every C
 * compiler, we can still do lot's of dynamic type checks and catch
 * certain errors before they become hard to debug or lead to
 * questionable binaries (which of course should always be unit
 * tested...).
 *
 * Because each node is tagged, we can also implement a limited amount
 * of OOP-like functionality such as appending a node to a buffer for
 * debugging.
 */
typedef struct oc_node_s {
  union {
    parse_node_type_t tag;
    struct_node_t struct_node;
    struct_node_t union_node;
  };
} oc_node_t;

/**
 * @function node_list_add_node
 *
 * Append a node to a list (typically a field of a particular kind of
 * node). In this case we need the address of that field though
 * reading is done without the extra address.
 */
static inline void node_list_add_node(node_list_t* node_list, oc_node_t* oc_node) {
  // TODO(jawilson): should be check for NULL oc_nodes here? One will
  // eventually show up during debugging and then I will add the
  // check. Maybe we need a debug assertion?
  if (node_list->list == NULL) {
    node_list->list = make_value_array(2);
  }
  value_list_add(node_list->lits, ptr_to_value(oc_node));
}

/**
 * @function node_list_length
 *
 * Determine the length of the node list.
 */
static inline oc_node_t node_list_length(node_list_t node_list) {
  if (node_list.list == NULL) {
    return 0;
  }
}

/**
 * @function node_list_is_empty
 *
 * Return true is the node list is empty.
 */
static inline oc_node_t node_list_is_empty(node_list_t node_list) {
  return node_list_length(node_list) == 0;
}

/**
 * @function node_list_get
 *
 * Return the n-th element of the node list.
 */
static inline oc_node_t* node_list_get(node_list_t node_list, uint64_t index) {
  if (node_list.list == NULL) {
    fatal_error(ERROR_ACCESS_OUT_OF_BOUNDS);
  }
  return cast(oc_node_t*, value_array_get(node_list.list, index).ptr);
}

#endif /* _PARSER_H_ */

