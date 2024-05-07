#line 2 "parser.c"
#ifndef _PARSER_H_
#define _PARSER_H_

#include "lexer.h"
#include <c-armyknife-lib.h>
#include <ctype.h>

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
  PARSE_NODE_DECLARATIONS,
  PARSE_NODE_ENUM,
  PARSE_NODE_ENUM_ELEMENT,
  PARSE_NODE_FIELD,
  PARSE_NODE_GLOBAL_FUNCTION,
  PARSE_NODE_GLOBAL_VARIABLE,
  PARSE_NODE_LIST_OF_NODES,
  PARSE_NODE_STRUCT,
  PARSE_NODE_UNION,
  PARSE_NODE_TYPE,
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
 * @structure node_list_t
 *
 * A list of "child" parse nodes. (Currently implemented by wrapping
 * value_array_t*).
 */
typedef struct node_list_S {
  value_array_t* list;
} node_list_t;

/**
 * @enum parse_error_code_t
 *
 * Represents a particular type of parse error so that a particular
 * error template can be used to present these errors to the user.
 */
typedef enum {
  PARSE_ERROR_UNKNOWN,
  PARSE_ERROR_COMMA_OR_EQUAL_SIGN_EXPECTED,
  PARSE_ERROR_EXPECTED_FIELD_WIDTH_OR_SEMICOLON,
  PARSE_ERROR_EXPECTED_SEMICOLON,
  PARSE_ERROR_IDENTIFIER_EXPECTED,
  PARSE_ERROR_INTEGER_LITERAL_EXPECTED,
  PARSE_ERROR_OPEN_BRACE_EXPECTED,
  PARSE_ERROR_UNRECOGNIZED_TOP_LEVEL_DECLARATION,
} parse_error_code_t;

/**
 * @struct parse_error_t
 *
 * This is the common return result for the various node parse
 * functions.
 */
typedef struct parse_error_S {
  parse_error_code_t error_code;
  oc_token_t* error_token;
} parse_error_t;

/**
 * @struct parse_node_t
 *
 * This is the common return result for the various node parse
 * functions.
 */
typedef struct parse_result_S {
  parse_node_t* node;
  uint64_t next_token_position;
  parse_error_t parse_error;
} parse_result_t;

/* ====================================================================== */
/* One node type per parse_node_type_t */
/* ====================================================================== */

/**
 * @structure declarations_t
 *
 * Represents all of the top-level declarations in a file (or later a
 * namespace).
 */
typedef struct declarations_S {
  parse_node_type_t tag;
  node_list_t declarations;
} declarations_t;

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
  node_list_t type_args;
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
  char* name; // this will be null for anonymous unions...
  node_list_t members;
} union_node_t;

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
} field_node_t;

/**
 * @structure enum_node_t
 *
 * Represents an enum reference or definition.
 */
typedef struct enum_node_S {
  parse_node_type_t tag;
  oc_token_t* name;
  node_list_t elements;
} enum_node_t;

/**
 * @structure enum_element_t
 *
 * Represents one "enumerator", enum constant, enum member, etc. Name
 * is not optional. Value is optional.
 */
typedef struct enum_element_S {
  parse_node_type_t tag;
  oc_token_t* name;
  oc_token_t* value;
} enum_element_t;

/* ====================================================================== */
/* General inlined accessors, helpers, and macros */
/* ====================================================================== */

static inline oc_token_t* token_at(value_array_t* tokens, uint64_t position) {
  // TODO(jawilson): maybe return a sentinel token of some sort when
  // position is greater than the end of the array?
  return cast(oc_token_t*, value_array_get(tokens, position).ptr);
}

static inline boolean_t token_matches(oc_token_t* token, char* str) {
  int str_len = strlen(str);
  return str_len == (token->end - token->start)
         && buffer_match_string_at(token->buffer, token->start, str);
}

/**
 * @function to_node
 *
 * Cast a particular type of node pointer to a "generic" node.
 *
 * This isn't particularly type safe though if we had over-loading in
 * C we could make a safer version.
 */
static inline parse_node_t* to_node(void* ptr) {
  return cast(parse_node_t*, ptr);
}

/**
 * Helper to determine if a node is a struct node before trying to
 * cast it.
 */
static inline boolean_t is_struct_node(parse_node_t* ptr) {
  return (ptr != NULL) && (ptr->tag == PARSE_NODE_STRUCT);
}

/**
 * Safely cast a generic node to a struct node after examining it's
 * tag.
 */
static inline struct_node_t* to_struct_node(parse_node_t* ptr) {
  if (ptr == NULL || ptr->tag != PARSE_NODE_STRUCT) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return cast(struct_node_t*, ptr);
}

/**
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
 * Safely cast a generic node to a declarations node after examining
 * it's tag.
 */
static inline declarations_t* to_declarations(parse_node_t* ptr) {
  if (ptr == NULL || ptr->tag != PARSE_NODE_DECLARATIONS) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return cast(declarations_t*, ptr);
}

/**
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
 * Safely cast a generic node to an enum_element_t* after examining
 * it's tag.
 */
static inline enum_element_t* to_enum_element(parse_node_t* ptr) {
  if (ptr == NULL || ptr->tag != PARSE_NODE_ENUM_ELEMENT) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return cast(enum_element_t*, ptr);
}

/**
 * Safely cast a generic node to a type node after examining it's
 * tag.
 */
static inline type_node_t* to_type_node(parse_node_t* ptr) {
  if (ptr == NULL || ptr->tag != PARSE_NODE_TYPE) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return cast(type_node_t*, ptr);
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
static inline parse_node_t* node_list_get(node_list_t node_list,
                                          uint64_t index) {
  if (node_list.list == NULL) {
    fatal_error(ERROR_ACCESS_OUT_OF_BOUNDS);
  }
  return cast(parse_node_t*, value_array_get(node_list.list, index).ptr);
}

/* ====================================================================== */
/* Inlined helpers for parse_result_t implementation */
/* ====================================================================== */

static inline parse_result_t parse_result_empty() {
  return (parse_result_t){0};
}

static inline boolean_t is_empty_result(parse_result_t result) {
  return (result.parse_error.error_code == PARSE_ERROR_UNKNOWN)
         && (result.node == NULL);
}

static inline parse_result_t parse_result(parse_node_t* node,
                                          uint64_t last_token_position) {
  if (node == NULL) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return (parse_result_t){node, last_token_position};
}

static inline parse_result_t parse_error_result(parse_error_code_t error_code,
                                                oc_token_t* error_token) {
  return (parse_result_t){.parse_error = (parse_error_t){
                              .error_code = error_code,
                              .error_token = error_token,
                          }};
}

static inline boolean_t is_error_result(parse_result_t result) {
  return result.parse_error.error_code != 0;
}

static inline boolean_t is_valid_result(parse_result_t result) {
  return result.node != NULL;
}

/* ====================================================================== */
/* Node allocation and initialization (makes sure we have the right tag). */
/* ====================================================================== */

static inline declarations_t* malloc_declarations() {
  declarations_t* result = malloc_struct(declarations_t);
  result->tag = PARSE_NODE_DECLARATIONS;
  return result;
}

static inline enum_node_t* malloc_enum_node() {
  enum_node_t* result = malloc_struct(enum_node_t);
  result->tag = PARSE_NODE_ENUM;
  return result;
}

static inline enum_element_t* malloc_enum_element() {
  enum_element_t* result = malloc_struct(enum_element_t);
  result->tag = PARSE_NODE_ENUM_ELEMENT;
  return result;
}

static inline type_node_t* malloc_type_node() {
  type_node_t* result = malloc_struct(type_node_t);
  result->tag = PARSE_NODE_TYPE;
  return result;
}

static inline struct_node_t* malloc_struct_node() {
  struct_node_t* result = malloc_struct(struct_node_t);
  result->tag = PARSE_NODE_STRUCT;
  return result;
}

static inline field_node_t* malloc_field_node() {
  field_node_t* result = malloc_struct(field_node_t);
  result->tag = PARSE_NODE_FIELD;
  return result;
}

/* ====================================================================== */
/* Enumeration to string */
/* ====================================================================== */

char* parse_node_type_to_string(parse_node_type_t type);
char* parse_error_code_to_string(parse_error_code_t error_code);
char* type_node_kind_to_string(type_node_kind_t kind);

/* ====================================================================== */
/* Forward declarations all the other routines. */
/* ====================================================================== */

parse_result_t parse_declarations(value_array_t* tokens, uint64_t position);
parse_result_t parse_declaration(value_array_t* tokens, uint64_t position);
parse_result_t parse_structure_node(value_array_t* tokens, uint64_t position);
parse_result_t parse_field_node(value_array_t* tokens, uint64_t position);
parse_result_t parse_type_node(value_array_t* tokens, uint64_t position);
parse_result_t parse_enum_node(value_array_t* tokens, uint64_t position);
parse_result_t parse_enum_element_node(value_array_t* tokens,
                                       uint64_t position);

#endif /* _PARSER_H_ */

/* ====================================================================== */
/* Implementation of non-inlined functions */
/* ====================================================================== */

parse_result_t parse_declarations(value_array_t* tokens, uint64_t position) {
  log_info("parse_declarations(_, %d)", position & 0xffffffff);
  declarations_t* result = malloc_declarations();
  while (position < tokens->length) {
    parse_result_t declaration = parse_declaration(tokens, position);
    if (is_error_result(declaration)) {
      return declaration;
    }
    node_list_add_node(&result->declarations, declaration.node);
    position = declaration.next_token_position;
  }
  return parse_result(to_node(result), position);
}

parse_result_t parse_declaration(value_array_t* tokens, uint64_t position) {
  log_info("parse_declaration(_, %d)", position & 0xffffffff);
  parse_result_t decl = parse_enum_node(tokens, position);
  if (!is_empty_result(decl)) {
    if (is_error_result(decl)) {
      return decl;
    } else if (token_matches(token_at(tokens, decl.next_token_position), ";")) {
      return parse_result(decl.node, decl.next_token_position + 1);
    }
  }

  decl = parse_structure_node(tokens, position);
  if (!is_empty_result(decl)) {
    if (is_error_result(decl)) {
      return decl;
    } else if (token_matches(token_at(tokens, decl.next_token_position), ";")) {
      return parse_result(decl.node, decl.next_token_position + 1);
    }
  }

  return parse_error_result(PARSE_ERROR_UNRECOGNIZED_TOP_LEVEL_DECLARATION,
                            token_at(tokens, position));
}

parse_result_t parse_structure_node(value_array_t* tokens, uint64_t position) {
  log_info("parse_structure_node(_, %d)", position & 0xffffffff);
  oc_token_t* token = token_at(tokens, position++);
  if (!token_matches(token, "struct")) {
    return parse_result_empty();
  }
  struct_node_t* result = malloc_struct_node();

  token = token_at(tokens, position++);
  if (token->type == TOKEN_TYPE_IDENTIFIER) {
    result->name = token;
    token = token_at(tokens, position++);
  }

  if (token_matches(token, "{")) {
    while (true) {
      token = token_at(tokens, position);
      if (token_matches(token, "}")) {
        position++;
        break;
      }
      parse_result_t field = parse_field_node(tokens, position);
      if (is_error_result(field)) {
        return field;
      }
      node_list_add_node(&result->fields, field.node);
      position = field.next_token_position;
    }
  }

  return parse_result(to_node(result), position);
}

parse_result_t parse_field_node(value_array_t* tokens, uint64_t position) {
  log_info("parse_field_node(_, %d)", position & 0xffffffff);
  parse_result_t field_type = parse_type_node(tokens, position);
  if (is_error_result(field_type)) {
    return field_type;
  }
  position = field_type.next_token_position;
  oc_token_t* field_name = token_at(tokens, position++);
  // make sure it's an identifier!
  if (token_matches(token_at(tokens, position), ":")) {
    // Capture field width here.
    position += 2;
  }
  if (!token_matches(token_at(tokens, position), ";")) {
    return parse_error_result(PARSE_ERROR_EXPECTED_FIELD_WIDTH_OR_SEMICOLON,
                              token_at(tokens, position));
  }
  position++;
  field_node_t* result = malloc_field_node();
  result->type = to_type_node(field_type.node);
  result->name = field_name;
  return parse_result(to_node(result), position);
}

parse_result_t parse_type_node(value_array_t* tokens, uint64_t position) {
  log_info("parse_type_node(_, %d)", position & 0xffffffff);
  // TODO(jawilson): allow parens for more complicated types...
  type_node_t* result = malloc_type_node();
  oc_token_t* type_name = token_at(tokens, position++);
  if (type_name->type != TOKEN_TYPE_IDENTIFIER) {
    return parse_error_result(PARSE_ERROR_IDENTIFIER_EXPECTED, type_name);
  }
  result->type_name = type_name;
  while (true) {
    oc_token_t* next = token_at(tokens, position);
    if (token_matches(next, "*")) {
      position++;
      type_node_t* ptr_result = malloc_type_node();
      ptr_result->type_node_kind = TYPE_NODE_KIND_POINTER;
      node_list_add_node(&ptr_result->type_args, to_node(result));
      result = ptr_result;
    } else {
      break;
    }
  }
  return parse_result(to_node(result), position);
}


/**
 * Parse a node representing something that starts with the "enum"
 * keyword. This can be used as part of a type or else part of an enum
 * declaration.
 */
parse_result_t parse_enum_node(value_array_t* tokens, uint64_t position) {
  log_info("parse_enum_node(_, %d)", position & 0xffffffff);
  oc_token_t* keyword_token = token_at(tokens, position++);
  if (!token_matches(keyword_token, "enum")) {
    return parse_result_empty();
  }
  enum_node_t* result = malloc_enum_node();
  oc_token_t* token = token_at(tokens, position++);
  if (token->type == TOKEN_TYPE_IDENTIFIER) {
    result->name = token;
    token = token_at(tokens, position++);
  }
  if (!token_matches(token, "{")) {
    // TODO(jawilson): brace or identifier in some cases...
    parse_error_result(PARSE_ERROR_OPEN_BRACE_EXPECTED, token);
  }
  while (true) {
    token = token_at(tokens, position);
    if (token_matches(token, "}")) {
      break;
    }
    parse_result_t element_result = parse_enum_element_node(tokens, position);
    if (is_error_result(element_result)) {
      return element_result;
    } else {
      node_list_add_node(&result->elements, element_result.node);
      position = element_result.next_token_position;
    }
  }
  return parse_result(to_node(result), position + 1);
}

parse_result_t parse_enum_element_node(value_array_t* tokens,
                                       uint64_t position) {
  log_info("parse_enum_element_node(_, %d)", position & 0xffffffff);
  enum_element_t* result = malloc_enum_element();
  oc_token_t* name = token_at(tokens, position++);
  result->name = name;
  if (name->type != TOKEN_TYPE_IDENTIFIER) {
    return parse_error_result(PARSE_ERROR_IDENTIFIER_EXPECTED, name);
  }

  oc_token_t* next = token_at(tokens, position);
  if (token_matches(next, "=")) {
    position++;
    oc_token_t* value = token_at(tokens, position);
    if (value->type != TOKEN_TYPE_INTEGER_LITERAL) {
      return parse_error_result(PARSE_ERROR_INTEGER_LITERAL_EXPECTED, value);
    }
    result->value = value;
    position++;
    next = token_at(tokens, position);
  }

  if (token_matches(next, ",")) {
    return parse_result(to_node(result), position + 1);
  } else if (token_matches(next, "}")) {
    return parse_result(to_node(result), position);
  } else {
    return parse_error_result(PARSE_ERROR_COMMA_OR_EQUAL_SIGN_EXPECTED, next);
  }
}

char* parse_node_type_to_string(parse_node_type_t type) {
  switch (type) {
  default:
  case PARSE_NODE_UNKNOWN:
    return "PARSE_NODE_UNKNOWN";
  case PARSE_NODE_DECLARATIONS:
    return "PARSE_NODE_DECLARATIONS";
  case PARSE_NODE_ENUM:
    return "PARSE_NODE_ENUM";
  case PARSE_NODE_ENUM_ELEMENT:
    return "PARSE_NODE_ENUM_ELEMENT";
  case PARSE_NODE_FIELD:
    return "PARSE_NODE_FIELD";
  case PARSE_NODE_GLOBAL_FUNCTION:
    return "PARSE_NODE_GLOBAL_FUNCTION";
  case PARSE_NODE_GLOBAL_VARIABLE:
    return "PARSE_NODE_GLOBAL_VARIABLE";
  case PARSE_NODE_LIST_OF_NODES:
    return "PARSE_NODE_LIST_OF_NODES";
  case PARSE_NODE_STRUCT:
    return "PARSE_NODE_STRUCT";
  case PARSE_NODE_UNION:
    return "PARSE_NODE_UNION";
  case PARSE_NODE_TYPE:
    return "PARSE_NODE_TYPE";
  }
  return "**NOT REACHED**";
}

char* parse_error_code_to_string(parse_error_code_t error_code) {
  switch (error_code) {
  default:
  case PARSE_ERROR_UNKNOWN:
    return "PARSE_ERROR_UNKNOWN";
  case PARSE_ERROR_COMMA_OR_EQUAL_SIGN_EXPECTED:
    return "PARSE_ERROR_COMMA_OR_EQUAL_SIGN_EXPECTED";
  case PARSE_ERROR_EXPECTED_FIELD_WIDTH_OR_SEMICOLON:
    return "PARSE_ERROR_EXPECTED_FIELD_WIDTH_OR_SEMICOLON";
  case PARSE_ERROR_EXPECTED_SEMICOLON:
    return "PARSE_ERROR_EXPECTED_SEMICOLON";
  case PARSE_ERROR_IDENTIFIER_EXPECTED:
    return "PARSE_ERROR_IDENTIFIER_EXPECTED";
  case PARSE_ERROR_INTEGER_LITERAL_EXPECTED:
    return "PARSE_ERROR_INTEGER_LITERAL_EXPECTED";
  case PARSE_ERROR_OPEN_BRACE_EXPECTED:
    return "PARSE_ERROR_OPEN_BRACE_EXPECTED";
  case PARSE_ERROR_UNRECOGNIZED_TOP_LEVEL_DECLARATION:
    return "PARSE_ERROR_UNRECOGNIZED_TOP_LEVEL_DECLARATION";
  }
  return "**NOT REACHED**";
}

char* type_node_kind_to_string(type_node_kind_t kind) {
  switch (kind) {
  default:
  case TYPE_NODE_KIND_UNKNOWN:
    return "TYPE_NODE_KIND_UNKNOWN";
  case TYPE_NODE_KIND_POINTER:
    return "TYPE_NODE_KIND_POINTER";
  case TYPE_NODE_KIND_ARRAY:
    return "TYPE_NODE_KIND_ARRAY";
  case TYPE_NODE_KIND_SIZED_ARRAY:
    return "TYPE_NODE_KIND_SIZED_ARRAY";
  case TYPE_NODE_KIND_STRUCT_PREFIXED_TYPENAME:
    return "TYPE_NODE_KIND_STRUCT_PREFIXED_TYPENAME";
  case TYPE_NODE_KIND_UNION_PREFIXED_TYPENAME:
    return "TYPE_NODE_KIND_UNION_PREFIXED_TYPENAME";
  case TYPE_NODE_KIND_ENUM_PREFIXED_TYPENAME:
    return "TYPE_NODE_KIND_ENUM_PREFIXED_TYPENAME";
  case TYPE_NODE_KIND_PRIMITIVE_TYPENAME:
    return "TYPE_NODE_KIND_PRIMITIVE_TYPENAME";
  case TYPE_NODE_KIND_TYPENAME:
    return "TYPE_NODE_KIND_TYPENAME";
  }
  return "**NOT REACHED**";
}
