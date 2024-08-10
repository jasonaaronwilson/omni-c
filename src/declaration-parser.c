#line 2 "declaration-parser.c"
#ifndef _DECLARATION_PARSER_H_
#define _DECLARATION_PARSER_H_

#include <c-armyknife-lib.h>
#include <ctype.h>

#include "compiler-errors.h"
#include "lexer.h"
#include "node-list.h"
#include "parser.h"
#include "pstate.h"
#include "token-list.h"

/**
 * @file declaration-parser.c
 *
 * This is the parser for top-level declarations in a C file for
 * things like functions (and prototypes), typedefs, structs, enums,
 * etc.
 */

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
  token_t* type_name;
  // These are for "complicated" user types like enum/struct/union
  parse_node_t* user_type;
  // Generic types (which C doesn't have but C++ kind of has) or even
  // just pointer and array types will have at least one child.
  node_list_t type_args;
} type_node_t;

/**
 * @structure typedef_node_t
 *
 * Represents a typedef in C or C++.
 */
typedef struct typedef_node_S {
  parse_node_type_t tag;
  token_t* name;
  type_node_t* type_node;
} typedef_node_t;

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
 * When bit_field_width is non-zero, the field is a bit field (this is
 * not legal with a union nodes).
 */
typedef struct field_node_S {
  parse_node_type_t tag;
  type_node_t* type;
  token_t* name;
  token_t* bit_field_width;
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
  token_t* value;
} enum_element_t;

/**
 * @structure function_body_node_t
 *
 * Represents an (unparsed) function body.
 */
typedef struct function_body_node_S {
  parse_node_type_t tag;
  token_t* open_brace_token;
  token_t* close_brace_token;
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
  token_t* token;
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

  token_t* storage_class_specifier; // static, extern, auto, register
  token_list_t function_specifiers; // inline, _Norturn, _Atomic

  type_node_t* return_type;

  token_t* function_name;
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
  token_t* arg_name;
  boolean_t is_var_args;
} function_argument_node_t;

/**
 * @structure global_variable_node_t
 *
 * Represents a top-level variable definition/declaration.
 */
typedef struct global_variable_node_S {
  parse_node_type_t tag;
  token_t* storage_class_specifier; // static, extern, auto, register
  type_node_t* type;
  token_t* name;
  parse_node_t* value;
  value_array_t* suffixes;
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
  token_t* first_token;
  token_t* last_token;
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
  token_t* inner_start_token;
  token_t* inner_end_token;
} attribute_node_t;

/**
 * @structure cpp_include_node_t
 *
 * Represents either a "system" or "user" include.
 */
typedef struct {
  parse_node_type_t tag;
  char* text;
} cpp_include_node_t;

/**
 * @structure cpp_define_node_t
 *
 * Represents either a simple substitution like `#define FOO 123` or a
 * macro that takes zero or more arguments.
 */
typedef struct {
  parse_node_type_t tag;
  char* text;
} cpp_define_node_t;

// PARSE_NODE_UNPARSED_EXPRESSION

/* ====================================================================== */
/* General inlined accessors, helpers, and macros */
/* ====================================================================== */

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
 *
 * TODO(jawilson): this is a little ugly but if will hopefully make
 * things work...
 */
static inline struct_node_t* to_struct_node(parse_node_t* ptr) {
  if (ptr == NULL
      || !(ptr->tag == PARSE_NODE_STRUCT || ptr->tag == PARSE_NODE_UNION)) {
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

/**
 * @function to_cpp_define_node
 *
 * Safely cast a generic node to a cpp_define_node_t after examining
 * it's tag.
 */
static inline cpp_define_node_t* to_cpp_define_node(parse_node_t* ptr) {
  if (ptr == NULL || ptr->tag != PARSE_NODE_CPP_DEFINE) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return cast(cpp_define_node_t*, ptr);
}

/**
 * @function to_cpp_include_node
 *
 * Safely cast a generic node to a cpp_include_node_t after examining
 * it's tag.
 */
static inline cpp_include_node_t* to_cpp_include_node(parse_node_t* ptr) {
  if (ptr == NULL || ptr->tag != PARSE_NODE_CPP_INCLUDE) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return cast(cpp_include_node_t*, ptr);
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

static inline cpp_include_node_t* malloc_cpp_include_node(void) {
  cpp_include_node_t* result = malloc_struct(cpp_include_node_t);
  result->tag = PARSE_NODE_CPP_INCLUDE;
  return result;
}

static inline cpp_define_node_t* malloc_cpp_define_node(void) {
  cpp_define_node_t* result = malloc_struct(cpp_define_node_t);
  result->tag = PARSE_NODE_CPP_DEFINE;
  return result;
}

/* ====================================================================== */
/* Enumeration to string */
/* ====================================================================== */

char* parse_node_type_to_string(parse_node_type_t type);
char* type_node_kind_to_string(type_node_kind_t kind);

/* ====================================================================== */
/* Forward declarations all the other routines. */
/* ====================================================================== */

typedef struct canonical_type_result_s {
  token_t* canonical_type;
  int consumed_tokens;
} canonical_type_result_t;

#include "declaration-parser.c.generated.h"

#endif /* _DECLARATION_PARSER_H_ */

/* ====================================================================== */
/* Implementation of non-inlined functions */
/* ====================================================================== */

parse_result_t parse_declarations(value_array_t* tokens, uint64_t position) {
  pstate_t pstate = (pstate_t){.tokens = tokens};
  declarations_node_t* result = malloc_declarations();
  while (pstate.position < tokens->length) {
    if (!parse_declaration(&pstate)) {
      parse_result_t parse_result = (parse_result_t){0};
      parse_result.parse_error = pstate.error;
      return parse_result;
    }
    node_list_add_node(&result->declarations, pstate_get_result_node(&pstate));
  }
  return parse_result(to_node(result), pstate.position);
}

/**
 * @function parse_declaration
 */
pstatus_t parse_declaration(pstate_t* pstate) {
  uint64_t saved_position = pstate->position;
  if (parse_function_node(pstate)
      || parse_typedef_node(pstate_ignore_error(pstate))
      || parse_enum_node_declaration(pstate_ignore_error(pstate))
      || parse_global_variable_node(pstate_ignore_error(pstate))
      || parse_structure_node_declaration(pstate_ignore_error(pstate))
      || parse_union_node_declaration(pstate_ignore_error(pstate))) {
    return true;
  }
  return pstate_error(pstate, saved_position,
                      PARSE_ERROR_UNRECOGNIZED_TOP_LEVEL_DECLARATION);
}

/**
 * @function parse_enum_node_declaration
 *
 * Parse a enum node plus the trailing ";".
 */
pstatus_t parse_enum_node_declaration(pstate_t* pstate) {
  uint64_t saved_position = pstate->position;
  // We can't simply chain these because the expect token wipes out
  // any nodes that haven't get grabbed. Hmmm.
  if (!parse_enum_node(pstate)) {
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_node_t* result = pstate_get_result_node(pstate);
  if (!pstate_expect_token_string(pstate, ";")) {
    return pstate_propagate_error(pstate, saved_position);
  }

  return pstate_set_result_node(pstate, result);
}

/**
 * @function parse_structure_node_declaration
 *
 * Parse a structure node plus the trailing ";".
 */
pstatus_t parse_structure_node_declaration(pstate_t* pstate) {
  uint64_t saved_position = pstate->position;
  // We can't simply chain these because the expect token wipes out
  // any nodes that haven't get grabbed. Hmmm.
  if (!parse_structure_node(pstate)) {
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_node_t* result = pstate_get_result_node(pstate);
  if (!pstate_expect_token_string(pstate, ";")) {
    return pstate_propagate_error(pstate, saved_position);
  }

  return pstate_set_result_node(pstate, result);
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
  if (!pstate_expect_token_string(pstate, ";")) {
    return pstate_propagate_error(pstate, saved_position);
  }
  field_node_t* result = malloc_field_node();
  result->type = field_type;
  result->name = field_name;
  return pstate_set_result_node(pstate, to_node(result));
}

/**
 * @function parse_union_node_declaration
 *
 * Parse a union node plus the trailing ";".
 */
pstatus_t parse_union_node_declaration(pstate_t* pstate) {
  uint64_t saved_position = pstate->position;
  // We can't simply chain these because the expect token wipes out
  // any nodes that haven't get grabbed. Hmmm.
  if (!parse_union_node(pstate)) {
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_node_t* result = pstate_get_result_node(pstate);
  if (!pstate_expect_token_string(pstate, ";")) {
    return pstate_propagate_error(pstate, saved_position);
  }

  return pstate_set_result_node(pstate, result);
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
 * @function parse_attribute_node
 *
 * Parses an attribute like __attribute__((warn_unsed_result)), etc.
 */
pstatus_t parse_attribute_node(pstate_t* pstate) {
  uint64_t saved_position = pstate->position;
  if (!pstate_expect_token_string(pstate, "__attribute__")
      || !parse_balanced_construct(pstate)) {
    return pstate_propagate_error(pstate, saved_position);
  }
  // function_body_node_t* body =
  // to_function_body_node(pstate_get_result_node(pstate));
  attribute_node_t* result = malloc_attribute_node();
  result->inner_start_token = token_at(pstate->tokens, saved_position);
  result->inner_end_token = pstate_peek(pstate, -1);
  return pstate_set_result_node(pstate, to_node(result));
}

/**
 * @function parse_function_node
 *
 * Parses a function node (either a prototype or full function
 * definition).
 */
pstatus_t parse_function_node(pstate_t* pstate) {
  uint64_t saved_position = pstate->position;

  token_t* storage_class_specifier = NULL;
  token_list_t function_specifiers = {0};
  node_list_t attributes = {0};

  while (1) {
    if (pstate_expect_token_string(pstate, "static")
        || pstate_expect_token_string(pstate_ignore_error(pstate), "extern")
        || pstate_expect_token_string(pstate_ignore_error(pstate), "auto")
        || pstate_expect_token_string(pstate_ignore_error(pstate),
                                      "register")) {
      if (storage_class_specifier == NULL) {
        storage_class_specifier = pstate_get_result_token(pstate);
      } else {
        return pstate_error(pstate, saved_position,
                            PARSE_ERROR_CONFLICTING_STORAGE_CLASS_SPECIFIER);
      }
    } else if (pstate_expect_token_string(pstate_ignore_error(pstate), "inline")
               || pstate_expect_token_string(pstate_ignore_error(pstate),
                                             "_Noreturn")) {
      token_list_add(&function_specifiers, pstate_get_result_token(pstate));
    } else if (parse_attribute_node(pstate_ignore_error(pstate))) {
      node_list_add_node(&attributes, pstate_get_result_node(pstate));
    } else {
      pstate_ignore_error(pstate);
      break;
    }
  }

  if (!parse_type_node(pstate)) {
    return pstate_propagate_error(pstate, saved_position);
  }
  type_node_t* return_type = to_type_node(pstate_get_result_node(pstate));

  if (!pstate_expect_token_type(pstate, TOKEN_TYPE_IDENTIFIER)) {
    return pstate_propagate_error(pstate, saved_position);
  }
  token_t* fn_name = pstate_get_result_token(pstate);

  if (!pstate_expect_token_string(pstate, "(")) {
    return pstate_propagate_error(pstate, saved_position);
  }

  function_node_t* fn_node = malloc_function_node();
  fn_node->attributes = attributes;
  fn_node->storage_class_specifier = storage_class_specifier;
  fn_node->function_specifiers = function_specifiers;
  fn_node->return_type = return_type;
  fn_node->function_name = fn_name;

  while (parse_function_argument_node(pstate)) {
    node_list_add_node(&fn_node->function_args, pstate_get_result_node(pstate));
  }
  pstate_ignore_error(pstate);

  if (!pstate_expect_token_string(pstate, ")")) {
    return pstate_propagate_error(pstate, saved_position);
  }

  if (parse_function_body_node(pstate)) {
    fn_node->body = to_function_body_node(pstate_get_result_node(pstate));
  } else {
    pstate_ignore_error(pstate);
    if (!pstate_expect_token_string(pstate, ";")) {
      return pstate_propagate_error(pstate, saved_position);
    }
  }

  return pstate_set_result_node(pstate, to_node(fn_node));
}

/**
 * @function parse_function_argument_node
 *
 * Parses either just a type, or a type plus an identifier.
 */
pstatus_t parse_function_argument_node(pstate_t* pstate) {
  uint64_t saved_position = pstate->position;
  function_argument_node_t* result = malloc_function_argument_node();

  if (pstate_match_token_string(pstate, "...")) {
    pstate_advance(pstate);
    result->is_var_args = true;
  } else {
    if (!parse_type_node(pstate)) {
      return pstate_propagate_error(pstate, saved_position);
    }
    result->arg_type = to_type_node(pstate_get_result_node(pstate));
    if (pstate_expect_token_type(pstate, TOKEN_TYPE_IDENTIFIER)) {
      result->arg_name = pstate_get_result_token(pstate);
    } else {
      pstate_ignore_error(pstate);
    }
  }
  if (pstate_match_token_string(pstate, ",")) {
    pstate_advance(pstate);
  }
  return pstate_set_result_node(pstate, to_node(result));
}

/**
 * @function parse_function_body_node
 *
 * "Parses" the body of a function by treating it as simply a token
 * list that must balance all of it's delimiters (except "<>" since
 * they appear as non-delimiters in other contexts).
 */
pstatus_t parse_function_body_node(pstate_t* pstate) {
  return parse_balanced_construct(pstate);
}

canonical_type_result_t make_type_token_result(char* str, int consumed_tokens) {
  token_t* canonical_token = malloc_struct(token_t);
  canonical_token->type = TOKEN_TYPE_IDENTIFIER;
  canonical_token->buffer = buffer_from_string(str);
  canonical_token->start = 0;
  canonical_token->end = strlen(str);
  return (canonical_type_result_t){.canonical_type = canonical_token,
                                   .consumed_tokens = consumed_tokens};
}

/**
 * @function parse_canonical_type
 *
 * Certain types in C comprise more than one natural C token so we
 * combine multiple tokens and simultaneously convert to a canonical
 * form.
 */
canonical_type_result_t parse_canonical_type(pstate_t* pstate) {
  token_t* a = pstate_peek(pstate, 0);
  token_t* b = pstate_peek(pstate, 1);
  token_t* c = pstate_peek(pstate, 2);

  if (token_matches(a, "signed") && token_matches(b, "short")
      && token_matches(c, "int")) {
    return make_type_token_result("short", 3);
  }

  if (token_matches(a, "unsigned") && token_matches(b, "short")
      && token_matches(c, "int")) {
    return make_type_token_result("unsigned short int", 3);
  }

  if (token_matches(a, "signed") && token_matches(b, "long")
      && token_matches(c, "int")) {
    return make_type_token_result("long", 3);
  }

  if (token_matches(a, "unsigned") && token_matches(b, "long")
      && token_matches(c, "int")) {
    return make_type_token_result("unsigned long", 3);
  }

  if (token_matches(a, "short") && token_matches(b, "int")) {
    return make_type_token_result("short", 2);
  }

  if (token_matches(a, "signed") && token_matches(b, "short")) {
    return make_type_token_result("short", 2);
  }

  if (token_matches(a, "signed") && token_matches(b, "int")) {
    return make_type_token_result("int", 2);
  }

  if (token_matches(a, "long") && token_matches(b, "int")) {
    return make_type_token_result("long", 2);
  }

  if (token_matches(a, "long") && token_matches(b, "long")) {
    return make_type_token_result("long long", 2);
  }

  if (token_matches(a, "long") && token_matches(b, "double")) {
    return make_type_token_result("long double", 2);
  }

  if (token_matches(a, "signed") && token_matches(b, "long")) {
    return make_type_token_result("long", 2);
  }

  if (token_matches(a, "unsigned") && token_matches(b, "int")) {
    return make_type_token_result("unsigned int", 2);
  }

  if (token_matches(a, "unsigned") && token_matches(b, "long")) {
    return make_type_token_result("unsigned long", 2);
  }

  if (token_matches(a, "unsigned") && token_matches(b, "char")) {
    return make_type_token_result("unsigned char", 2);
  }

  if (token_matches(a, "signed") && token_matches(b, "char")) {
    return make_type_token_result("char", 2);
  }

  if (token_matches(a, "long") && token_matches(b, "float")) {
    // TODO(jawilson): make using this a warning or something
    return make_type_token_result("double", 2);
  }

  if (token_matches(a, "signed")) {
    return make_type_token_result("int", 1);
  }

  if (token_matches(a, "char")) {
    return make_type_token_result("char", 1);
  }

  if (token_matches(a, "int")) {
    return make_type_token_result("int", 1);
  }

  if (token_matches(a, "long")) {
    return make_type_token_result("long", 1);
  }

  if (token_matches(a, "float")) {
    return make_type_token_result("float", 1);
  }

  if (token_matches(a, "double")) {
    return make_type_token_result("double", 1);
  }

  return (canonical_type_result_t){.canonical_type = NULL,
                                   .consumed_tokens = 0};
}

/**
 * @function parse_user_type_node
 *
 * Parses an enum, struct or (eventually) union.
 */
pstatus_t parse_user_type_node(pstate_t* pstate) {
  uint64_t saved_position = pstate->position;
  // TODO(jawilson): unions!
  if (parse_enum_node(pstate)
      || parse_structure_node(pstate_ignore_error(pstate))
      || parse_union_node(pstate_ignore_error(pstate))) {
    return true;
  } else {
    return pstate_propagate_error(pstate, saved_position);
  }
}

/**
 * @function parse_type_node
 *
 * Parses any C type.
 */
pstatus_t parse_type_node(pstate_t* pstate) {
  uint64_t saved_position = pstate->position;
  type_node_t* result = malloc_type_node();

  // Simply skip const to see if that will help us with
  // c-armyknife-lib
  if (pstate_match_token_string(pstate, "const")) {
    pstate_advance(pstate);
  }

  // First see if we have a canonical type result...
  canonical_type_result_t canonical_type_result = parse_canonical_type(pstate);
  token_t* type_name = canonical_type_result.canonical_type;

  if (type_name != NULL) {
    while (canonical_type_result.consumed_tokens > 0) {
      canonical_type_result.consumed_tokens--;
      pstate_advance(pstate);
    }
    result->type_node_kind = TYPE_NODE_KIND_PRIMITIVE_TYPENAME;
    result->type_name = type_name;
  } else if (parse_user_type_node(pstate)) {
    result->type_node_kind = TYPE_NODE_KIND_TYPE_EXPRESSION;
    result->user_type = pstate_get_result_node(pstate);
  } else if (pstate_expect_token_type(pstate, TOKEN_TYPE_IDENTIFIER)) {
    result->type_node_kind = TYPE_NODE_KIND_TYPENAME;
    result->type_name = pstate_get_result_token(pstate);
  } else {
    return pstate_propagate_error(pstate, saved_position);
  }

  // TODO(jawilson): allow parens for more complicated types...

  while (true) {
    if (pstate_expect_token_string(pstate, "*")) {
      type_node_t* ptr_result = malloc_type_node();
      ptr_result->type_node_kind = TYPE_NODE_KIND_POINTER;
      node_list_add_node(&ptr_result->type_args, to_node(result));
      result = ptr_result;
    } else if (token_matches(pstate_peek(pstate, 0), "[")) {
      pstate_advance(pstate);
      type_node_t* array_result = malloc_type_node();
      array_result->type_node_kind = TYPE_NODE_KIND_ARRAY;
      node_list_add_node(&array_result->type_args, to_node(result));
      if (pstate_expect_token_type(pstate, TOKEN_TYPE_INTEGER_LITERAL)) {
        literal_node_t* literal = malloc_literal_node();
        literal->token = pstate_get_result_token(pstate);
        array_result->type_node_kind = TYPE_NODE_KIND_SIZED_ARRAY;
        node_list_add_node(&array_result->type_args, to_node(literal));
      } else {
        pstate_ignore_error(pstate);
      }
      // TODO(jawilson): parse VLA
      if (!pstate_expect_token_string(pstate, "]")) {
        return pstate_propagate_error(pstate, saved_position);
      }
      result = array_result;
    } else {
      pstate_ignore_error(pstate);
      break;
    }
  }
  return pstate_set_result_node(pstate, to_node(result));
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
 * Parse an enum element with an optional integer value and eat any
 * trailing comma.
 */
pstatus_t parse_enum_element_node(pstate_t* pstate) {
  uint64_t saved_position = pstate->position;
  if (!pstate_expect_token_type(pstate, TOKEN_TYPE_IDENTIFIER)) {
    return pstate_propagate_error(pstate, saved_position);
  }
  token_t* name = pstate_get_result_token(pstate);
  token_t* value = NULL;
  if (pstate_expect_token_string(pstate, "=")) {
    if (!pstate_expect_token_type(pstate, TOKEN_TYPE_INTEGER_LITERAL)) {
      return pstate_propagate_error(pstate, saved_position);
    }
    value = pstate_get_result_token(pstate);
  }

  if (!pstate_expect_token_string(pstate, ",")) {
    pstate_ignore_error(pstate);
  }

  enum_element_t* result = malloc_enum_element();
  result->name = name;
  result->value = value;

  return pstate_set_result_node(pstate, to_node(result));
}

/**
 * @function parse_typedef_node
 *
 * Parses a C/C++ typedef node.
 *
 * typedef TYPE NAME;
 * TODO(jawilson): typedef int (*StringToInt)(const char*);
 */
pstatus_t parse_typedef_node(pstate_t* pstate) {
  uint64_t saved_position = pstate->position;
  if (!pstate_expect_token_string(pstate, "typedef")) {
    return pstate_propagate_error(pstate, saved_position);
  }
  if (!parse_type_node(pstate)) {
    return pstate_propagate_error(pstate, saved_position);
  }
  type_node_t* type_node = to_type_node(pstate_get_result_node(pstate));
  if (!pstate_expect_token_type(pstate, TOKEN_TYPE_IDENTIFIER)) {
    return pstate_propagate_error(pstate, saved_position);
  }
  token_t* name = pstate_get_result_token(pstate);
  if (!pstate_expect_token_string(pstate, ";")) {
    return pstate_propagate_error(pstate, saved_position);
  }

  typedef_node_t* result = malloc_typedef_node();
  result->type_node = type_node;
  result->name = name;
  return pstate_set_result_node(pstate, to_node(result));
}

/**
 * @function parse_global_variable_node
 *
 * Parses a C/C++ global variable.
 */
pstatus_t parse_global_variable_node(pstate_t* pstate) {
  uint64_t saved_position = pstate->position;
  token_t* storage_class_specifier = NULL;

  while (pstate_expect_token_string(pstate, "static")
         || pstate_expect_token_string(pstate_ignore_error(pstate), "extern")) {
    if (storage_class_specifier != NULL) {
      return pstate_error(pstate, saved_position,
                          PARSE_ERROR_CONFLICTING_STORAGE_CLASS_SPECIFIER);
    }
    storage_class_specifier = pstate_get_result_token(pstate);
  }
  pstate_ignore_error(pstate);
  if (!parse_type_node(pstate)) {
    return pstate_propagate_error(pstate, saved_position);
  }
  type_node_t* type_node = to_type_node(pstate_get_result_node(pstate));
  if (!pstate_expect_token_type(pstate, TOKEN_TYPE_IDENTIFIER)) {
    return pstate_error(pstate, saved_position,
                        PARSE_ERROR_IDENTIFIER_EXPECTED);
  }
  token_t* name = pstate_get_result_token(pstate);

  global_variable_node_t* result = malloc_global_variable_node();
  result->type = type_node;
  result->name = name;

  while (pstate_match_token_string(pstate, "[")) {
    if (!parse_balanced_construct(pstate)) {
      return pstate_propagate_error(pstate, saved_position);
    }
    if (result->suffixes == NULL) {
      result->suffixes = make_value_array(1);
    }
    value_array_add(result->suffixes,
                    ptr_to_value(pstate_get_result_node(pstate)));
  }

  if (pstate_expect_token_string(pstate, "=")) {
    if (!parse_literal_node(pstate)) {
      return pstate_propagate_error(pstate, saved_position);
    } else {
      result->value = pstate_get_result_node(pstate);
    }
  } else {
    pstate_ignore_error(pstate);
  }

  if (!pstate_expect_token_string(pstate, ";")) {
    return pstate_propagate_error(pstate, saved_position);
  }

  return pstate_set_result_node(pstate, to_node(result));
}

pstatus_t parse_literal_node(pstate_t* pstate) {
  uint64_t saved_position = pstate->position;
  if (pstate_expect_token_type(pstate, TOKEN_TYPE_STRING_LITERAL)) {
    literal_node_t* result = malloc_literal_node();
    result->tokens = make_value_array(1);
    value_array_add(result->tokens,
                    ptr_to_value(pstate_get_result_token(pstate)));
    while (pstate_expect_token_type(pstate, TOKEN_TYPE_STRING_LITERAL)) {
      value_array_add(result->tokens,
                      ptr_to_value(pstate_get_result_token(pstate)));
    }
    pstate_ignore_error(pstate);
    return pstate_set_result_node(pstate, to_node(result));
  } else {
    pstate_ignore_error(pstate);
  }

  if (pstate_match_token_string(pstate, "{")) {
    if (!parse_balanced_construct(pstate)) {
      return pstate_propagate_error(pstate, saved_position);
    }
    literal_node_t* result = malloc_literal_node();
    result->initializer_node
        = to_function_body_node(pstate_get_result_node(pstate));
    return pstate_set_result_node(pstate, to_node(result));
  }

  if (pstate_expect_token_string(pstate, "NULL")
      || pstate_expect_token_string(pstate_ignore_error(pstate), "nullptr")
      || pstate_expect_token_string(pstate_ignore_error(pstate), "true")
      || pstate_expect_token_string(pstate_ignore_error(pstate), "false")
      || pstate_expect_token_type(pstate_ignore_error(pstate),
                                  TOKEN_TYPE_INTEGER_LITERAL)
      || pstate_expect_token_type(pstate_ignore_error(pstate),
                                  TOKEN_TYPE_FLOAT_LITERAL)
      || pstate_expect_token_type(pstate_ignore_error(pstate),
                                  TOKEN_TYPE_CHARACTER_LITERAL)) {
    literal_node_t* result = malloc_literal_node();
    result->token = pstate_get_result_token(pstate);
    return pstate_set_result_node(pstate, to_node(result));
  }

  return pstate_error(pstate, saved_position, PARSE_ERROR_NOT_LITERAL_NODE);
}

/**
 * @function parse_balanced_construct
 *
 * This is hack which allows us to parse just about anything like
 * function bodies, etc. without having written a full parser.
 */
pstatus_t parse_balanced_construct(pstate_t* pstate) {
  uint64_t saved_position = pstate->position;

  int open_parens = 0;
  int open_brackets = 0;
  int open_braces = 0;
  do {
    if (pstate_expect_token_string(pstate, "(")) {
      open_parens++;
    } else if (pstate_expect_token_string(pstate_ignore_error(pstate), "[")) {
      open_brackets++;
    } else if (pstate_expect_token_string(pstate_ignore_error(pstate), "{")) {
      open_braces++;
    } else if (pstate_expect_token_string(pstate_ignore_error(pstate), ")")) {
      open_parens--;
    } else if (pstate_expect_token_string(pstate_ignore_error(pstate), "]")) {
      open_brackets--;
    } else if (pstate_expect_token_string(pstate_ignore_error(pstate), "}")) {
      open_braces--;
    } else {
      pstate_advance(pstate_ignore_error(pstate));
    }
  } while (open_parens + open_brackets + open_braces > 0);

  if (pstate->position == saved_position + 1) {
    // FIXME error type
    return pstate_error(pstate, saved_position,
                        PARSE_ERROR_OPEN_BRACE_EXPECTED);
  }

  function_body_node_t* result = malloc_function_body_node();
  result->open_brace_token = token_at(pstate->tokens, saved_position);
  result->close_brace_token = pstate_peek(pstate, -1);

  return pstate_set_result_node(pstate, to_node(result));
}
