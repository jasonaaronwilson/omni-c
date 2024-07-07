// SSCF generated file from: parser.c

#line 2 "parser.c"
#ifndef _PARSER_H_
#define _PARSER_H_

#include "compiler-errors.h"
#include "lexer.h"
#include "node-list.h"
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
  PARSE_NODE_LITERAL,
  PARSE_NODE_FUNCTION,
  PARSE_NODE_FUNCTION_ARGUMENT,
  PARSE_NODE_FUNCTION_BODY,
  PARSE_NODE_TYPEDEF,
  PARSE_NODE_GLOBAL_VARIABLE_DEFINITION,
  PARSE_NODE_UNPARSED_EXPRESSION,
  PARSE_NODE_ATTRIBUTE,
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
 * @struct parse_node_t
 *
 * This is the common return result for the various node parse
 * functions.
 */
typedef struct parse_result_S {
  parse_node_t* node;
  uint64_t next_token_position;
  compiler_error_t parse_error;
} parse_result_t;

/* ====================================================================== */
/* One node type per parse_node_type_t */
/* ====================================================================== */

/**
 * @structure declarations_node_t
 *
 * Represents all of the top-level declarations in a file (or later a
 * namespace).
 */
typedef struct declarations_node_S {
  parse_node_type_t tag;
  node_list_t declarations;
} declarations_node_t;

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
  TYPE_NODE_KIND_VARIABLE_SIZED_ARRAY,
  TYPE_NODE_KIND_PRIMITIVE_TYPENAME,
  TYPE_NODE_KIND_TYPENAME,
  TYPE_NODE_KIND_TYPE_EXPRESSION,
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
  //
  // To simplify handling of signed and unsigned types, we use a
  // canonical name by combining multiple tokens or just remapping a
  // token if necessary.
  oc_token_t* type_name;
  // These are for "complicated" user types like enum/struct/union
  parse_node_t* user_type;
  // Generic types (which C doesn't have but C++ kind of has) or even
  // just pointer and array types will have at least one child.
  node_list_t type_args;
} type_node_t;

/**
 * @structure type_node_t
 *
 * Represents a typedef in C or C++.
 */
typedef struct typedef_node_S {
  parse_node_type_t tag;
  oc_token_t* name;
  type_node_t* type_node;
} typedef_node_t;

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

/**
 * @structure function_body_node_t
 *
 * Represents an (unparsed) function body.
 */
typedef struct function_body_node_S {
  parse_node_type_t tag;
  oc_token_t* open_brace_token;
  oc_token_t* close_brace_token;
} function_body_node_t;


/**
 * @structure literal_node_t
 *
 * Represents a source code literal such as an integer/floating-point
 * number, character constant, a string literal, or a complex
 * initializer like {1, 2}.
 */
typedef struct literal_node_S {
  parse_node_type_t tag;
  oc_token_t* token;
  // This is used for string literals since in C multiple string
  // literals can appear in sequence and should be treated like a
  // single literal if these literals were smushed into one token.
  value_array_t* tokens;
  function_body_node_t* initializer_node;
} literal_node_t;

/**
 * @structure function_node_t
 *
 * Represents either a prototype or a full function definition.
 */
typedef struct function_node_S {
  parse_node_type_t tag;

  // __attribute__((...))  warn_unsed_result, noreturn, deprecated,
  // unused, format... format looks harder to parse than the others so
  // for now we will just keep all token between (( and )).
  node_list_t attributes;

  oc_token_t* storage_class_specifier; // static, extern, auto, register
  oc_token_t* function_specifier;      // inline

  type_node_t* return_type;

  oc_token_t* function_name;
  node_list_t function_args;

  // Until we learn how to fully parse expressions, this contains all
  // of the tokens between and including "{" and "}".
  function_body_node_t* body;
} function_node_t;

/**
 * @structure function_argument_node_t
 *
 * Represents an argument in a function declaration.
 */
typedef struct function_argument_node_S {
  parse_node_type_t tag;
  type_node_t* arg_type;
  oc_token_t* arg_name;
} function_argument_node_t;

/**
 * @structure global_variable_node_t
 *
 * Represents a top-level variable definition/declaration.
 */
typedef struct global_variable_node_S {
  parse_node_type_t tag;
  oc_token_t* storage_class_specifier; // static, extern, auto, register
  type_node_t* type;
  oc_token_t* name;
  parse_node_t* value;
  boolean_t number_of_array_suffixes;
} global_variable_node_t;

/**
 * @structure unparsed_expression_t
 *
 * Represents what might be a valid C or C++ expression but which we
 * used a simpler strategy. These nodes would presumably not exist
 * once the parser is complete but allows us to make temporary forward
 * progress.
 *
 * (Currently function_body_node_t is almost exactly like this node
 * but presumably that one will remain (or simply become a
 * statement_block_node_t once that exists).
 */
typedef struct unparsed_expression_S {
  parse_node_type_t tag;
  // TODO(jawilson): consider creating and using a token list...
  oc_token_t* first_token;
  oc_token_t* last_token;
} unparsed_expression_t;

/**
 * @structure attribute_node_t
 *
 * Represents __attribute__((...)) nodes. We aren't trying to fully
 * understand these yet (if we did, splitting them into independent
 * attributes would probably be wise so that we can tell if say
 * something has warn_unsed_result or noreturn (the other are less
 * interesting and if truly compiling to C or C++ then it might not
 * matter what we do).
 */
typedef struct attribute_node_S {
  parse_node_type_t tag;
  oc_token_t* inner_start_token;
  oc_token_t* inner_end_token;
} attribute_node_t;

// PARSE_NODE_UNPARSED_EXPRESSION

/* ====================================================================== */
/* General inlined accessors, helpers, and macros */
/* ====================================================================== */

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

/* ====================================================================== */
/* These feel like they can be automatically generated or become a C
   preprocessor macro - but that wouldn't work with our "javadoc"
   tool. Hmmm. */
/* ====================================================================== */

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
  if (ptr == NULL || ptr->tag != PARSE_NODE_STRUCT) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return cast(struct_node_t*, ptr);
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
 * @function to_declarations_node
 *
 * Safely cast a generic node to a declarations node after examining
 * it's tag.
 */
static inline declarations_node_t* to_declarations_node(parse_node_t* ptr) {
  if (ptr == NULL || ptr->tag != PARSE_NODE_DECLARATIONS) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return cast(declarations_node_t*, ptr);
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

/**
 * @function to_type_node
 *
 * Safely cast a generic node to a type node after examining it's
 * tag.
 */
static inline type_node_t* to_type_node(parse_node_t* ptr) {
  if (ptr == NULL || ptr->tag != PARSE_NODE_TYPE) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return cast(type_node_t*, ptr);
}

/**
 * @function to_literal_node
 *
 * Safely cast a generic node to a literal node after examining it's
 * tag.
 */
static inline literal_node_t* to_literal_node(parse_node_t* ptr) {
  if (ptr == NULL || ptr->tag != PARSE_NODE_LITERAL) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return cast(literal_node_t*, ptr);
}

/**
 * @function to_function_node
 *
 * Safely cast a generic node to a function node after examining it's
 * tag.
 */
static inline function_node_t* to_function_node(parse_node_t* ptr) {
  if (ptr == NULL || ptr->tag != PARSE_NODE_FUNCTION) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return cast(function_node_t*, ptr);
}

/**
 * @function to_function_argument_node
 *
 * Safely cast a generic node to a function_argument_node_t after
 * examining it's tag.
 */
static inline function_argument_node_t*
    to_function_argument_node(parse_node_t* ptr) {
  if (ptr == NULL || ptr->tag != PARSE_NODE_FUNCTION_ARGUMENT) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return cast(function_argument_node_t*, ptr);
}

/**
 * @function to_function_body_node
 *
 * Safely cast a generic node to a function_body_node_t after
 * examining it's tag.
 */
static inline function_body_node_t* to_function_body_node(parse_node_t* ptr) {
  if (ptr == NULL || ptr->tag != PARSE_NODE_FUNCTION_BODY) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return cast(function_body_node_t*, ptr);
}

/**
 * @function to_typedef_node
 *
 * Safely cast a generic node to a typedef_node_t after examining it's
 * tag.
 */
static inline typedef_node_t* to_typedef_node(parse_node_t* ptr) {
  if (ptr == NULL || ptr->tag != PARSE_NODE_TYPEDEF) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return cast(typedef_node_t*, ptr);
}

/**
 * @function to_global_variable_node
 *
 * Safely cast a generic node to a global_variable_node_t after
 * examining it's tag.
 */
static inline global_variable_node_t*
    to_global_variable_node(parse_node_t* ptr) {
  if (ptr == NULL || ptr->tag != PARSE_NODE_GLOBAL_VARIABLE_DEFINITION) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return cast(global_variable_node_t*, ptr);
}

/**
 * @function to_unparsed_expression
 *
 * Safely cast a generic node to an unparsed_expression_t node after
 * examining it's tag.
 */
static inline unparsed_expression_t* to_unparsed_expression(parse_node_t* ptr) {
  if (ptr == NULL || ptr->tag != PARSE_NODE_UNPARSED_EXPRESSION) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return cast(unparsed_expression_t*, ptr);
}

/**
 * @function to_attribute_node
 *
 * Safely cast a generic node to a attribute_node_t after examining
 * it's tag.
 */
static inline attribute_node_t* to_attribute_node(parse_node_t* ptr) {
  if (ptr == NULL || ptr->tag != PARSE_NODE_ATTRIBUTE) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return cast(attribute_node_t*, ptr);
}

/* ====================================================================== */
/* Inlined helpers for parse_result_t implementation */
/* ====================================================================== */

static inline parse_result_t parse_result_empty(void) {
  return (parse_result_t){0};
}

static inline boolean_t is_empty_result(parse_result_t result) {
  return (result.parse_error.parser_error_code == PARSE_ERROR_UNKNOWN)
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
  return (parse_result_t){.parse_error = (compiler_error_t){
                              .parser_error_code = error_code,
                              .error_token = error_token,
                          }};
}

static inline boolean_t is_error_result(parse_result_t result) {
  return result.parse_error.parser_error_code != 0;
}

static inline boolean_t is_valid_result(parse_result_t result) {
  return result.node != NULL;
}

/* ====================================================================== */
/* Node allocation and initialization (makes sure we have the right tag). */
/* ====================================================================== */

static inline declarations_node_t* malloc_declarations(void) {
  declarations_node_t* result = malloc_struct(declarations_node_t);
  result->tag = PARSE_NODE_DECLARATIONS;
  return result;
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

static inline type_node_t* malloc_type_node(void) {
  type_node_t* result = malloc_struct(type_node_t);
  result->tag = PARSE_NODE_TYPE;
  return result;
}

static inline struct_node_t* malloc_struct_node(void) {
  struct_node_t* result = malloc_struct(struct_node_t);
  result->tag = PARSE_NODE_STRUCT;
  return result;
}

static inline field_node_t* malloc_field_node(void) {
  field_node_t* result = malloc_struct(field_node_t);
  result->tag = PARSE_NODE_FIELD;
  return result;
}

static inline literal_node_t* malloc_literal_node(void) {
  literal_node_t* result = malloc_struct(literal_node_t);
  result->tag = PARSE_NODE_LITERAL;
  return result;
}

static inline function_node_t* malloc_function_node(void) {
  function_node_t* result = malloc_struct(function_node_t);
  result->tag = PARSE_NODE_FUNCTION;
  return result;
}

static inline function_argument_node_t* malloc_function_argument_node(void) {
  function_argument_node_t* result = malloc_struct(function_argument_node_t);
  result->tag = PARSE_NODE_FUNCTION_ARGUMENT;
  return result;
}

static inline function_body_node_t* malloc_function_body_node(void) {
  function_body_node_t* result = malloc_struct(function_body_node_t);
  result->tag = PARSE_NODE_FUNCTION_BODY;
  return result;
}

static inline typedef_node_t* malloc_typedef_node(void) {
  typedef_node_t* result = malloc_struct(typedef_node_t);
  result->tag = PARSE_NODE_TYPEDEF;
  return result;
}

static inline global_variable_node_t* malloc_global_variable_node(void) {
  global_variable_node_t* result = malloc_struct(global_variable_node_t);
  result->tag = PARSE_NODE_GLOBAL_VARIABLE_DEFINITION;
  return result;
}

static inline attribute_node_t* malloc_attribute_node(void) {
  attribute_node_t* result = malloc_struct(attribute_node_t);
  result->tag = PARSE_NODE_ATTRIBUTE;
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

parse_result_t parse_function_node(value_array_t* tokens, uint64_t position);
parse_result_t parse_function_argument_node(value_array_t* tokens,
                                            uint64_t position);
parse_result_t parse_function_body_node(value_array_t* tokens,
                                        uint64_t position);

parse_result_t parse_typedef_node(value_array_t* tokens, uint64_t position);

parse_result_t parse_global_variable_node(value_array_t* tokens,
                                          uint64_t position);

parse_result_t parse_literal_node(value_array_t* tokens, uint64_t position);

typedef struct canonical_type_result_s {
  oc_token_t* canonical_type;
  int consumed_tokens;
} canonical_type_result_t;

#include "parser.c.generated.h"

#endif /* _PARSER_H_ */
