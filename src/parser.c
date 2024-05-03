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

/**
 * @enum parse_node_type_t
 *
 * We will have at least one node per major kind of C construct.
 */
typedef enum {
  OC_NODE_UNKNOWN,
  OC_NODE_LIST_OF_NODES,
  OC_NODE_STRUCT,
  OC_NODE_FIELD,
  OC_NODE_UNION,
  OC_NODE_GLOBAL_VARIABLE,
  OC_NODE_GLOBAL_FUNCTION,
} parse_node_type_t;

/**
 * @structure oc_node_list_t
 *
 * A list of "child" parse nodes. (Currently implemented by wrapping
 * value_array_t*).
 */
typedef struct node_list_S {
  value_array_t* list;
} oc_node_list_t;

/**
 * @structure type_node_t
 *
 * Represents a parsed "type", for example "uint64_t", "uint64_t*",
 * "uint64_t[]", etc.
 */
typedef struct type_node_S {
  parse_node_type_t tag;

  // Only one of the next three fields will be set. When
  // is_pointer_type or is_array_type are set, there is expected to be
  // a type_arg child node (even if it is just the void type). This
  // allows us to cleanly build up complex types like
  // "uint64_t**". Coincidentally, if we want to have generic types,
  // then the list of type_args becomes even more useful.
  oc_token_t* type_name;
  boolean_t is_pointer_type;
  boolean_t is_array_type;

  // Generic types (which C doesn't have but C++ kind of has) or even
  // just pointer and array types will have at least one child.
  oc_node_list_t type_args;
} type_node_t;


/**
 * @structure struct_node_t
 *
 * Represents a partial or full structure declaration.
 */
typedef struct struct_node_S {
  parse_node_type_t tag;
  oc_token_t* name;
  node_list_t fields; 
  boolean_t partial_definition;
} struct_node_t;

/**
 * @structure field_node_t
 *
 * Represents a field in a structure or union definition.
 *
 * When bit_field_width is non-zero, the field is a bit field.
 */
typedef struct field_node_S {
  parse_node_type_t tag;
  type_node_t* type;
  oc_token_t* name;
  oc_token_t* bit_field_width;
} struct_node_t;

/**
 * @structure oc_node_list_t
 *
 * Represents a partial or full structure declaration.
 */
typedef struct union_node_S {
  parse_node_type_t tag;
  char *name; // this will be null for anonymous unions...
  node_list_t members; 
} struct_node_t;

/**
 * @structure oc_node_t
 *
 * A generic parse node. Parse nodes can be freely cast to a generic
 * node via the function to_oc_node() or to a particular type via
 * to_struct_node(), etc. When casting to a more particular type, the
 * tag will be dynamically checked. If you are uncertain of the type
 * of a node, then is_struct_node(), etc. can be used to make sure the
 * operation is legal.
 */
typedef struct oc_node_s {
  union {
    parse_node_type_t tag;
    struct_node_t struct_node;
    struct_node_t union_node;
  };
} oc_node_t;

/**
 * Cast a particular type of node pointer to a "generic" node.
 *
 * This isn't particularly type safe though if we had over-loading in
 * C we could make a safer version.
 */
static inline oc_node_t* to_oc_node(void* ptr) {
  return cast(oc_node_t*, ptr);
}

/**
 * Determine if a node is a struct node before trying to cast it.
 */
static inline boolean_t is_struct_node(oc_node_t* ptr) {
  return (ptr != NULL) && (ptr.tag == OC_NODE_STRUCT;
}

/**
 * Safely cast a generic node to a particular type by examining it's
 * tag.
 */
static inline struct_node_t* to_struct_node(oc_node_t* ptr) {
  if (ptr == NULL || ptr.tag != OC_NODE_STRUCT) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return cast(struct_node_t*, ptr);
}

/**
 * @function node_list_add_node
 *
 * Append a node to a list (typically a field of a particular kind of
 * node). In this case we need the address of that field though
 * reading is done without the extra address.
 */
static inline void node_list_add_node(node_list_t* node_list, oc_node_t* oc_node) {
  if (oc_node == NULL) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
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

typedef oc_node_result_S {
  oc_node_t* node;
  uint64_t last_token_position;
  // TODO(jawilson): parse error information here!
} oc_node_result_t;

 static inline oc_node_result_t oc_node_empty_result() {
   return (oc_node_result_t) { 0 };
 }

 static inline boolean_t is_error_result(oc_node_result_t result) {
   // TODO(jawilson): use some indication to determine if this is a
   // parse error (vs simply not matching, aka,
   // oc_node_empty)result().
   return false;
 }

 static inline boolean_t is_empty_result(oc_node_result_t result) {
   return result == NULL;
 }

 static inline oc_node_result_t* oc_node_result(oc_node_t* node, uint64_t last_token_position) {
   if (node == NULL) {
     fatal_error(ERROR_ILLEGAL_STATE);
   }
   return (oc_node_result_t*) { node, last_token_position };
 }

oc_node_result_t* parse_structure(value_array_t* tokens, uint64_t position);

#endif /* _PARSER_H_ */

static inline oc_token_t* token_at(value_array_t* tokens, uint64_t position) {
  // TODO(jawilson): maybe return a sentinel token of some sort when
  // position is greater than the end of the array.
  return cast(oc_token_t*, value_array_get(tokens, position).ptr);
}

oc_node_result_t* parse_structure(value_array_t* tokens, uint64_t position) {
  oc_token_t* token = token_at(tokens, position++);
  if (!token_matches(token, "struct")) {
    return oc_node_empty_result();
  }
  struct_node_t result = malloc_struct(struct_node_t);
 
  token = token_at(tokens, position++);
  if (token->type == TOKEN_TYPE_IDENTIFIER) {
    result->name = token;
    token = token_at(tokens, position++);
  }

  if (token_matches(token, ";")) {
    result->partial_definition = true;
    return oc_node_result(result, position);
  }

  if (token_matches(token, "{")) {
    
    while (true) {
    }
  }

  // ...HERE...
  
  return oc_node_result(result);
}
