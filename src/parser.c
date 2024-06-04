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
  TYPE_NODE_KIND_VARIABLE_SIZED_ARRAY,
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
  oc_token_t* function_name;
  type_node_t* return_type;
  node_list_t function_args;
  // Until we learn how to fully parse expressions, this contains all
  // of the tokens between "{" and "}" (and also those tokens).
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
  oc_token_t* name;
  type_node_t* type;
  parse_node_t* value;
  boolean_t number_array_suffixes;
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
  oc_token_t* first_token;
  oc_token_t* last_token;
} unparsed_expression_t;


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
 * Helper to determine if a node is a struct node before trying to
 * cast it.
 */
static inline boolean_t is_struct_node(parse_node_t* ptr) {
  return (ptr != NULL) && (ptr->tag == PARSE_NODE_STRUCT);
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
static inline declarations_t* to_declarations_node(parse_node_t* ptr) {
  if (ptr == NULL || ptr->tag != PARSE_NODE_DECLARATIONS) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return cast(declarations_t*, ptr);
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
 * Safely cast a generic node to a function argument node after
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
 * Safely cast a generic node to a function body node after examining
 * it's tag.
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
 * Safely cast a generic node to a tyedef node after examining it's
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
 * Safely cast a generic node to a tyedef node after examining it's
 * tag.
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
 * Safely cast a generic node to a tyedef node after examining it's
 * tag.
 */
static inline unparsed_expression_t* to_unparsed_expression(parse_node_t* ptr) {
  if (ptr == NULL || ptr->tag != PARSE_NODE_UNPARSED_EXPRESSION) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return cast(unparsed_expression_t*, ptr);
}


/* ====================================================================== */
/* Inlined helpers for parse_result_t implementation */
/* ====================================================================== */

static inline parse_result_t parse_result_empty() {
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

static inline literal_node_t* malloc_literal_node() {
  literal_node_t* result = malloc_struct(literal_node_t);
  result->tag = PARSE_NODE_LITERAL;
  return result;
}

static inline function_node_t* malloc_function_node() {
  function_node_t* result = malloc_struct(function_node_t);
  result->tag = PARSE_NODE_FUNCTION;
  return result;
}

static inline function_argument_node_t* malloc_function_argument_node() {
  function_argument_node_t* result = malloc_struct(function_argument_node_t);
  result->tag = PARSE_NODE_FUNCTION_ARGUMENT;
  return result;
}

static inline function_body_node_t* malloc_function_body_node() {
  function_body_node_t* result = malloc_struct(function_body_node_t);
  result->tag = PARSE_NODE_FUNCTION_BODY;
  return result;
}

static inline typedef_node_t* malloc_typedef_node() {
  typedef_node_t* result = malloc_struct(typedef_node_t);
  result->tag = PARSE_NODE_TYPEDEF;
  return result;
}

static inline global_variable_node_t* malloc_global_variable_node() {
  global_variable_node_t* result = malloc_struct(global_variable_node_t);
  result->tag = PARSE_NODE_GLOBAL_VARIABLE_DEFINITION;
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

  parse_result_t fn_result = parse_function_node(tokens, position);
  if (is_valid_result(fn_result)) {
    return parse_result(fn_result.node, fn_result.next_token_position);
  }

  parse_result_t tdef_result = parse_typedef_node(tokens, position);
  if (is_valid_result(tdef_result)) {
    return parse_result(tdef_result.node, tdef_result.next_token_position);
  }

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

  parse_result_t glb_var_result = parse_global_variable_node(tokens, position);
  if (is_valid_result(glb_var_result)) {
    return glb_var_result;
  }

  return parse_error_result(PARSE_ERROR_UNRECOGNIZED_TOP_LEVEL_DECLARATION,
                            token_at(tokens, position));
}

parse_result_t parse_structure_node(value_array_t* tokens, uint64_t position) {
  log_info("parse_structure_node(_, %d)", position & 0xffffffff);
  oc_token_t* token = token_at(tokens, position);
  if (!token_matches(token, "struct")) {
    return parse_result_empty();
  }
  struct_node_t* result = malloc_struct_node();

  position++;
  token = token_at(tokens, position);
  if (token->type == TOKEN_TYPE_IDENTIFIER) {
    result->name = token;
    position++;
    token = token_at(tokens, position);
  }

  log_info("-- parse_structure_node(_, %d)", position & 0xffffffff);

  if (token_matches(token, "{")) {
    position++;
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
  // TODO(jawilson): make field_name is an identifier.
  log_info("parse_field_node(_, %d)", position & 0xffffffff);

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

/**
 * @function parse_function_node
 *
 * Parses a function node (either a prototype or full function definition).
 */
parse_result_t parse_function_node(value_array_t* tokens, uint64_t position) {
  parse_result_t return_type = parse_type_node(tokens, position);
  if (is_empty_result(return_type)) {
    return parse_result_empty();
  }
  position = return_type.next_token_position;
  oc_token_t* fn_name = token_at(tokens, position++);
  if (fn_name->type != TOKEN_TYPE_IDENTIFIER) {
    return parse_result_empty();
  }
  oc_token_t* open_paren = token_at(tokens, position++);
  if (!token_matches(open_paren, "(")) {
    return parse_result_empty();
  }

  function_node_t* fn_node = malloc_function_node();
  fn_node->function_name = fn_name;
  fn_node->return_type = to_type_node(return_type.node);

  oc_token_t* next = token_at(tokens, position);
  if (token_matches(next, ")")) {
    position++;
  } else {
    while (1) {
      parse_result_t arg = parse_function_argument_node(tokens, position);
      if (!is_valid_result(arg)) {
        return parse_result_empty();
      }
      node_list_add_node(&fn_node->function_args, arg.node);
      position = arg.next_token_position;
      next = token_at(tokens, position++);
      if (token_matches(next, ")")) {
        break;
      } else if (!token_matches(next, ",")) {
        // ERROR instead?
        return parse_result_empty();
      }
    }
  }

  next = token_at(tokens, position);
  if (token_matches(next, "{")) {
    parse_result_t body_result = parse_function_body_node(tokens, position);
    if (!is_valid_result(body_result)) {
      return body_result;
    }
    fn_node->body = to_function_body_node(body_result.node);
    position = body_result.next_token_position;
  } else if (token_matches(next, ";")) {
    position++;
  } else {
    // ERROR?
    return parse_result_empty();
  }

  return parse_result(to_node(fn_node), position);
}

/**
 * @function parse_function_argument_node
 *
 * Parses either just a type, or a type plus an identifier.
 */
parse_result_t parse_function_argument_node(value_array_t* tokens,
                                            uint64_t position) {
  parse_result_t type = parse_type_node(tokens, position);
  if (!is_valid_result(type)) {
    // ERROR
    return parse_result_empty();
  }
  position = type.next_token_position;
  function_argument_node_t* result = malloc_function_argument_node();
  result->arg_type = to_type_node(type.node);
  oc_token_t* next = token_at(tokens, position);
  if (next->type == TOKEN_TYPE_IDENTIFIER) {
    result->arg_name = next;
    position++;
  } else if (token_matches(next, ",") || token_matches(next, ")")) {
    // ERROR
  }

  return parse_result(to_node(result), position);
}

/**
 * @function parse_function_body_node
 *
 * "Parses" the body of a function by treating it as simply a token
 * list that must balance all of it's delimiters (except "<>" since
 * they appear as non-delimiters in other contexts).
 */
parse_result_t parse_function_body_node(value_array_t* tokens,
                                        uint64_t position) {
  function_body_node_t* result = malloc_function_body_node();

  // TODO(jawilson): use a stack instead so we can match up different
  // delimiters.
  int count = 0;
  do {
    oc_token_t* token = token_at(tokens, position++);
    if (count == 0) {
      result->open_brace_token = token;
    } else {
      result->close_brace_token = token;
    }
    // TODO(jawilson): ERROR if NULL
    if (token_matches(token, "{")) {
      count++;
    } else if (token_matches(token, "}")) {
      count--;
    }
  } while (count > 0);

  return parse_result(to_node(result), position);
}

typedef struct canonical_type_result_s {
  oc_token_t* canonical_type;
  int consumed_tokens;
} canonical_type_result_t;

canonical_type_result_t make_type_token_result(char* str, int consumed_tokens) {
  oc_token_t* canonical_token = malloc_struct(oc_token_t);
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
canonical_type_result_t parse_canonical_type(value_array_t* tokens,
                                             uint64_t position) {
  log_info("parse_canonical_type(_, %d)", position & 0xffffffff);

  oc_token_t* a = token_at(tokens, position);
  oc_token_t* b = token_at(tokens, position + 1);
  oc_token_t* c = token_at(tokens, position + 2);

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

parse_result_t parse_user_type_node(value_array_t* tokens, uint64_t position) {
  log_info("parse_user_type_node(_, %d)", position & 0xffffffff);

  parse_result_t result = parse_enum_node(tokens, position);
  if (is_valid_result(result)) {
    return result;
  }

  result = parse_structure_node(tokens, position);
  if (is_valid_result(result)) {
    return result;
  }

  // TODO(jawilson): unions!

  return parse_result_empty();
}


parse_result_t parse_type_node(value_array_t* tokens, uint64_t position) {
  log_info("parse_type_node(_, %d)", position & 0xffffffff);
  type_node_t* result = malloc_type_node();
  parse_node_t* user_type = NULL;

  // First see if we have a canonical type result...
  canonical_type_result_t canonical_type_result
      = parse_canonical_type(tokens, position);
  oc_token_t* type_name = canonical_type_result.canonical_type;
  position += canonical_type_result.consumed_tokens;

  // If it's not canonical, then it could be a struct/union/or enum
  if (type_name == NULL) {
    parse_result_t user_type_result = parse_user_type_node(tokens, position);
    if (is_valid_result(user_type_result)) {
      // TODO(jawilson): we need a kind for a user type
      result->type_node_kind = TYPE_NODE_KIND_TYPENAME;
      user_type = user_type_result.node;
      position = user_type_result.next_token_position;
    } else {
      result->type_node_kind = TYPE_NODE_KIND_TYPENAME;
      type_name = token_at(tokens, position++);
      if (type_name->type != TOKEN_TYPE_IDENTIFIER) {
        return parse_error_result(PARSE_ERROR_IDENTIFIER_EXPECTED, type_name);
      }
    }
  } else {
    result->type_node_kind = TYPE_NODE_KIND_PRIMITIVE_TYPENAME;
  }

  // TODO(jawilson): allow parens for more complicated types...

  result->type_name = type_name;
  while (true) {
    oc_token_t* next = token_at(tokens, position);
    if (token_matches(next, "*")) {
      position++;
      type_node_t* ptr_result = malloc_type_node();
      ptr_result->type_node_kind = TYPE_NODE_KIND_POINTER;
      node_list_add_node(&ptr_result->type_args, to_node(result));
      result = ptr_result;
    } else if (token_matches(next, "[")) {
      position++;
      oc_token_t* open = next;
      next = token_at(tokens, position++);
      type_node_t* array_result = malloc_type_node();
      array_result->type_node_kind = TYPE_NODE_KIND_ARRAY;
      node_list_add_node(&array_result->type_args, to_node(result));
      if (next->type == TOKEN_TYPE_INTEGER_LITERAL) {
        literal_node_t* literal = malloc_literal_node();
        literal->token = next;
        array_result->type_node_kind = TYPE_NODE_KIND_SIZED_ARRAY;
        next = token_at(tokens, position++);
        node_list_add_node(&array_result->type_args, to_node(literal));
      }
      // TODO(jawilson): parse VLA
      if (!token_matches(next, "]")) {
        return parse_error_result(PARSE_ERROR_CLOSE_BRACKET_EXPECTED, next);
      }
      result = array_result;
    } else {
      break;
    }
  }
  return parse_result(to_node(result), position);
}


/**
 * Parse a node representing something that starts with the "enum"
 * keyword. This can be used as part of a typedef or else part of an
 * enum declaration.
 */
parse_result_t parse_enum_node(value_array_t* tokens, uint64_t position) {
  log_info("parse_enum_node(_, %d)", position & 0xffffffff);
  oc_token_t* keyword_token = token_at(tokens, position++);
  if (!token_matches(keyword_token, "enum")) {
    return parse_result_empty();
  }
  enum_node_t* result = malloc_enum_node();
  oc_token_t* token = token_at(tokens, position);
  if (token->type == TOKEN_TYPE_IDENTIFIER) {
    result->name = token;
    position += 1;
    token = token_at(tokens, position);
  }
  if (!token_matches(token, "{")) {
    // TODO(jawilson): this is actually not an error example "enum
    // foo".
    parse_error_result(PARSE_ERROR_OPEN_BRACE_EXPECTED, token);
  }
  position++;
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

/**
 * @function parse_typedef_node
 *
 * Parses a C/C++ typedef node.
 *
 * typedef TYPE NAME;
 * TODO(jawilson): typedef int (*StringToInt)(const char*);
 */
parse_result_t parse_typedef_node(value_array_t* tokens, uint64_t position) {
  log_info("parse_typedef_node(_, %d)", position & 0xffffffff);
  oc_token_t* typedef_token = token_at(tokens, position++);
  if (!token_matches(typedef_token, "typedef")) {
    return parse_result_empty();
  }

  parse_result_t type = parse_type_node(tokens, position);
  if (!is_valid_result(type)) {
    // ERROR
    return parse_result_empty();
  }
  position = type.next_token_position;
  oc_token_t* name = token_at(tokens, position++);
  if (name->type != TOKEN_TYPE_IDENTIFIER) {
    parse_error_result(PARSE_ERROR_IDENTIFIER_EXPECTED, name);
  }

  typedef_node_t* result = malloc_typedef_node();
  result->type_node = to_type_node(type.node);
  result->name = name;

  oc_token_t* semi = token_at(tokens, position++);
  if (!token_matches(semi, ";")) {
    return parse_error_result(PARSE_ERROR_SEMICOLON_EXPECTED, name);
  }


  return parse_result(to_node(result), position);
}

/**
 * @function parse_global_variable_node
 *
 * Parses a C/C++ global variable.
 */
parse_result_t parse_global_variable_node(value_array_t* tokens,
                                          uint64_t position) {
  log_info("parse_global_variable_node(_, %d)", position & 0xffffffff);
  parse_result_t type = parse_type_node(tokens, position);
  if (!is_valid_result(type)) {
    // ERROR?
    return parse_result_empty();
  }
  position = type.next_token_position;
  oc_token_t* name = token_at(tokens, position++);
  if (name->type != TOKEN_TYPE_IDENTIFIER) {
    parse_error_result(PARSE_ERROR_IDENTIFIER_EXPECTED, name);
  }

  global_variable_node_t* result = malloc_global_variable_node();
  result->type = to_type_node(type.node);
  result->name = name;

  while (true) {
    // TODO(jawilson): capture array sizes!
    oc_token_t* open_bracket_token = token_at(tokens, position);
    if (token_matches(open_bracket_token, "[")) {
      position++;
      oc_token_t* close_bracket_token = token_at(tokens, position);
      if (!token_matches(close_bracket_token, "]")) {
        return parse_error_result(PARSE_ERROR_CLOSE_BRACKET_EXPECTED,
                                  close_bracket_token);
      }
      position++;
      result->number_array_suffixes++;
    } else {
      break;
    }
  }

  oc_token_t* equal_token = token_at(tokens, position);
  if (token_matches(equal_token, "=")) {
    position++;
    parse_result_t literal_result = parse_literal_node(tokens, position);
    if (is_valid_result(literal_result)) {
      result->value = literal_result.node;
      position = literal_result.next_token_position;
    } else {
      return literal_result;
    }
  }

  oc_token_t* semi = token_at(tokens, position++);
  if (!token_matches(semi, ";")) {
    return parse_error_result(PARSE_ERROR_SEMICOLON_EXPECTED, name);
  }

  return parse_result(to_node(result), position);
}

parse_result_t parse_literal_node(value_array_t* tokens, uint64_t position) {
  log_info("parse_literal_node(_, %lld)", position);

  oc_token_t* token = token_at(tokens, position);

  if (token->type == TOKEN_TYPE_STRING_LITERAL) {
    literal_node_t* result = malloc_literal_node();
    result->tokens = make_value_array(1);
    while (true) {
      oc_token_t* token = token_at(tokens, position);
      if (token == NULL || token->type != TOKEN_TYPE_STRING_LITERAL) {
        return parse_result(to_node(result), position);
      }
      value_array_add(result->tokens, ptr_to_value(token));
      position++;
    }
  }

  if (token_matches(token, "{")) {
    parse_result_t fn_body_node_result
        = parse_function_body_node(tokens, position);
    if (is_valid_result(fn_body_node_result)) {
      literal_node_t* result = malloc_literal_node();
      result->initializer_node
          = to_function_body_node(fn_body_node_result.node);
      return parse_result(to_node(result),
                          fn_body_node_result.next_token_position);
    } else {
      return fn_body_node_result;
    }
  }

  if (token_matches(token, "NULL") || token_matches(token, "nullptr")
      || token_matches(token, "true") || token_matches(token, "false")) {
    literal_node_t* result = malloc_literal_node();
    result->token = token;
    return parse_result(to_node(result), position + 1);
  }

  if (token->type == TOKEN_TYPE_INTEGER_LITERAL
      || token->type == TOKEN_TYPE_STRING_LITERAL
      || token->type == TOKEN_TYPE_FLOAT_LITERAL
      || token->type == TOKEN_TYPE_CHARACTER_LITERAL) {
    literal_node_t* result = malloc_literal_node();
    result->token = token;
    return parse_result(to_node(result), position + 1);
  }

  return parse_result_empty();
}
