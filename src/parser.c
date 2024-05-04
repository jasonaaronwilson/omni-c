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
 *
 * Unlike TSNode (from the tree sitter project), we have explicit
 * structures for many different types of parse nodes (similar to what
 * is done in certain languages like Haskell or a bit like what might
 * done in an OOP language even though everything is written in
 * C). Node pointers can be cast and dynamically type checked though
 * static type checking isn't as strong it would be if written in
 * Haskell. I prefer this style since you don't need to understand the
 * parser language definition to use the parse tree.
 */

/**
 * @enum parse_node_type_t
 *
 * We will have at least one node per major kind of C construct.
 */
typedef enum {
  PARSE_NODE_UNKNOWN,
  PARSE_NODE_LIST_OF_NODES,
  PARSE_NODE_STRUCT,
  PARSE_NODE_FIELD,
  PARSE_NODE_UNION,
  PARSE_NODE_GLOBAL_VARIABLE,
  PARSE_NODE_GLOBAL_FUNCTION,
} parse_node_type_t;

/**
 * @structure parse_node_t
 *
 * A generic parse node. Parse nodes can be freely cast to a generic
 * node via the function to_oc_node() or to a particular type via
 * to_struct_node(), etc. When casting to a more particular type, the
 * tag will be dynamically checked. If you are uncertain of the type
 * of a node, then is_struct_node(), etc. can be used to make sure the
 * operation is legal.
 *
 * parse_node_t is most used as a pointer, i.e., parse_node_t*, so the fact
 * that some nodes are much bigger than other nodes is irrelevant.
 */
typedef struct parse_node_s {
  parse_node_type_t tag;
} parse_node_t;

/**
 * @structure parse_node_list_t
 *
 * A list of "child" parse nodes. (Currently implemented by wrapping
 * value_array_t*).
 */
typedef struct node_list_S {
  value_array_t* list;
} parse_node_list_t;

/**
 * @enum parse_error_code_t
 *
 * Represents a particular type of parse error so that a particular
 * error template can be used to present these errors to the user.
 */
typedef enum {
  PARSE_ERROR_UNKNOWN,
} parse_error_code_t;

/**
 * @struct parse_node_t
 *
 * This is the common return result for the various node parse
 * functions.
 */
typedef parse_error_S {
  parse_error_code_t parse_error_code;
  token_t* parse_error_token;
} parse_error_t;

/**
 * @struct parse_node_t
 *
 * This is the common return result for the various node parse
 * functions.
 */
typedef parse_node_S {
  parse_node_t* result_node;
  uint64_t last_token_position;
  parse_error_t parse_error;
} parse_node_t;

/* ====================================================================== */
/* One node type per parse_node_type_t */
/* ====================================================================== */

/**
 * @enum type_node_kind_t
 *
 * Sub-categorizes the various kinds of type nodes.
 */
typedef enum {
  TYPE_NODE_KIND_UNKNOWN,
  TYPE_NODE_KIND_POINTER,
  TYPE_NODE_KIND_ARRAY,
  TYPE_NODE_KIND_SIZED_ARRAY,
  // TODO(jawilson): anonymous structures and unions?
  TYPE_NODE_KIND_STRUCT_PREFIXED_TYPENAME,
  TYPE_NODE_KIND_UNION_PREFIXED_TYPENAME,
  TYPE_NODE_KIND_ENUM_PREFIXED_TYPENAME,
  TYPE_NODE_KIND_PRIMITIVE_TYPENAME,
  TYPE_NODE_KIND_TYPENAME,
} type_node_kind_t;

/**
 * @structure type_node_t
 *
 * Represents a parsed "type", for example "uint64_t", "uint64_t*",
 * "uint64_t[]", etc. In the future these will be used to represent
 * generic types such as map<int, char*>, etc.
 */
typedef struct type_node_S {
  parse_node_type_t tag;
  type_node_kind_t type_node_kind;
  // This isn't set for pointer and array types since they modify
  // their first child in type_args. Think of these as being like the
  // generic types: array<T> or pointer<T>.
  oc_token_t* type_name;
  // Generic types (which C doesn't have but C++ kind of has) or even
  // just pointer and array types will have at least one child.
  parse_node_list_t type_args;
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
 * @structure oc_union_node_t
 *
 * Represents a partial or full union declaration.
 */
typedef struct union_node_S {
  parse_node_type_t tag;
  char *name; // this will be null for anonymous unions...
  node_list_t members; 
} struct_node_t;

/**
 * @structure field_node_t
 *
 * Represents a field in a structure or union definition.
 *
 * When bit_field_width is non-zero, the field is a bit field (this is
 * not legal with a union nodes).
 */
typedef struct field_node_S {
  parse_node_type_t tag;
  type_node_t* type;
  oc_token_t* name;
  oc_token_t* bit_field_width;
} struct_node_t;

/* ====================================================================== */
/* General inlined accessors, helpers, and macros */
/* ====================================================================== */

/**
 * Cast a particular type of node pointer to a "generic" node.
 *
 * This isn't particularly type safe though if we had over-loading in
 * C we could make a safer version.
 */
static inline parse_node_t* to_oc_node(void* ptr) {
  return cast(parse_node_t*, ptr);
}

/**
 * Helper to determine if a node is a struct node before trying to
 * cast it.
 */
static inline boolean_t is_struct_node(parse_node_t* ptr) {
  return (ptr != NULL) && (ptr.tag == PARSE_NODE_STRUCT);
}

/**
 * Safely cast a generic node to a particular type by examining it's
 * tag.
 */
static inline struct_node_t* to_struct_node(parse_node_t* ptr) {
  if (ptr == NULL || ptr.tag != PARSE_NODE_STRUCT) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return cast(struct_node_t*, ptr);
}

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
static inline void node_list_add_node(node_list_t* node_list, parse_node_t* oc_node) {
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
static inline parse_node_t node_list_is_empty(node_list_t node_list) {
  return node_list_length(node_list) == 0;
}

/**
 * @function node_list_get
 *
 * Return the n-th element of the node list.
 */
static inline parse_node_t* node_list_get(node_list_t node_list, uint64_t index) {
  if (node_list.list == NULL) {
    fatal_error(ERROR_ACCESS_OUT_OF_BOUNDS);
  }
  return cast(parse_node_t*, value_array_get(node_list.list, index).ptr);
}

/* ====================================================================== */
/* Inlined helpers for parse_node_result_t implementation */
/* ====================================================================== */

static inline parse_node_result_t parse_node_empty_result() {
  return (parse_node_result_t) { 0 };
}

static inline boolean_t is_error_result(parse_node_result_t result) {
  // TODO(jawilson): use some indication to determine if this is a
  // parse error (vs simply not matching, aka,
  // parse_node_empty)result().
  return false;
}

static inline boolean_t is_empty_result(parse_node_result_t result) {
  return result == NULL;
}

static inline parse_node_result_t* parse_node_result(parse_node_t* node, uint64_t last_token_position) {
  if (node == NULL) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return (parse_node_result_t*) { node, last_token_position };
}

/* ====================================================================== */
/* Forward declarations for all the invidual construct parsers */
/* ====================================================================== */

parse_node_result_t parse_structure_node(value_array_t* tokens, uint64_t position);

#endif /* _PARSER_H_ */

/* ====================================================================== */
/* Implementation of non-inlined functions */
/* ====================================================================== */

static inline oc_token_t* token_at(value_array_t* tokens, uint64_t position) {
  // TODO(jawilson): maybe return a sentinel token of some sort when
  // position is greater than the end of the array?
  return cast(oc_token_t*, value_array_get(tokens, position).ptr);
}

parse_node_result_t parse_structure_node(value_array_t* tokens, uint64_t position) {
  oc_token_t* token = token_at(tokens, position++);
  if (!token_matches(token, "struct")) {
    return parse_node_empty_result();
  }
  struct_node_t result = malloc_struct(struct_node_t);
 
  token = token_at(tokens, position++);
  if (token->type == TOKEN_TYPE_IDENTIFIER) {
    result->name = token;
    token = token_at(tokens, position++);
  }

  if (token_matches(token, ";")) {
    result->partial_definition = true;
    return parse_node_result(result, position);
  }

  if (token_matches(token, "{")) {
    while (true) {
      if (token_matches(token, "}")) {
	break;
      }
      parse_node_result_t field = parse_field_node(tokens, position);
      if (is_error_result(field)) {
	return field;
      }
      node_list_add_node(&result->fields, field.
    }
  }

  // ...HERE...
  
  return parse_node_result(result);
}

parse_result_t parse_field_node(value_array_t* tokens, uint64_t position) {
  parse_node_result_t field_type = parse_type_node(tokens, position);
  if (is_error_result(field_type)) {
    return field_type;
  }
  position = field_type.last_token_position;
  parse_node_result_t field_name = parse_type_node(tokens, position);
  if (is_error_result(field_name)) {
    return field_name;
  }
  position = field_type.last_token_position;
  if (token_matches(token_at(tokens, position), ":")) {
    // Capture field width here.
    position += 2;
  }
  if (!token_matches(token_at(tokens, position), ";")) {
    return parse_error_result(HERE);
  } 
}


