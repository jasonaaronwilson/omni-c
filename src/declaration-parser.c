#line 2 "declaration-parser.c"
#ifndef _DECLARATION_PARSER_H_
#define _DECLARATION_PARSER_H_

#include "compiler-errors.h"
#include "lexer.h"
#include "node-list.h"
#include "parser.h"
#include <c-armyknife-lib.h>
#include <ctype.h>

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
  token_t* function_specifier;      // inline

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
  token_t* canonical_type;
  int consumed_tokens;
} canonical_type_result_t;

#include "declaration-parser.c.generated.h"

#endif /* _DECLARATION_PARSER_H_ */

/* ====================================================================== */
/* Implementation of non-inlined functions */
/* ====================================================================== */

parse_result_t parse_declarations(value_array_t* tokens, uint64_t position) {
  log_info("parse_declarations(_, %d)", position & 0xffffffff);
  declarations_node_t* result = malloc_declarations();
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
  token_t* token = token_at(tokens, position);
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
  } else {
    result->partial_definition = true;
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
  token_t* field_name = token_at(tokens, position++);
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
 * @function parse_attribute_node
 *
 * Parses an attribute like __attribute__((warn_unsed_result)), etc.
 */
parse_result_t parse_attribute_node(value_array_t* tokens, uint64_t position) {
  token_t* token = token_at(tokens, position);
  if (!token_matches(token, "__attribute__")) {
    return parse_result_empty();
  }
  position++;

  // We need to match two open parens
  for (int i = 0; i < 2; i++) {
    token = token_at(tokens, position);
    if (!token_matches(token, "(")) {
      return parse_error_result(
          PARSE_ERROR_EXPECTED_OPEN_PAREN_AFTER_UNDERSCORE_ATTRIBUTE, token);
    }
    position++;
  }

  token_t* inner_start_token = token_at(tokens, position);
  token_t* inner_end_token = token_at(tokens, position);
  int unclosed_paren_count = 2;
  do {
    // TODO(jawilson): handle reaching the end of the token stream
    // more gracefully.
    token_t* token = token_at(tokens, position);
    // If this were to happen immediately after the first two parens,
    // that would probably be illegal.
    if (token_matches(token, "(")) {
      unclosed_paren_count++;
    } else if (token_matches(token, ")")) {
      unclosed_paren_count--;
      if (unclosed_paren_count == 0) {
        break;
      }
    } else {
      inner_end_token = token;
    }
    position++;
  } while (1);

  attribute_node_t* result = malloc_attribute_node();
  result->inner_start_token = inner_start_token;
  result->inner_end_token = inner_end_token;
  return parse_result(to_node(result), position + 1);
}

/**
 * @function parse_function_node
 *
 * Parses a function node (either a prototype or full function definition).
 */
parse_result_t parse_function_node(value_array_t* tokens, uint64_t position) {

  token_t* storage_class_specifier = NULL;
  token_t* function_specifier = NULL;
  node_list_t attributes = {0};

  while (1) {
    token_t* token = token_at(tokens, position);
    if (token_matches(token, "static")
        || token_matches(token, "extern")
        // This isn't really appropriate for a function as a real
        // storage class but C23 is using it to denote a deduced type,
        // i.e., let the compiler figure out the return type. If this
        // is already true or might happen, then "auto" isn't really
        // appropriate here. One step at a time!
        || token_matches(token, "auto")
        // A version of "chatgpt" suggested register is a valid
        // storage class for a function (not a function pointer) and
        // even if this is true, it seems unlikely this was
        // accomplished in practice (though I've had a little fun
        // thinking about it - likely a bad idea for highly pipelined
        // processors but it might have been a way to avoid
        // self-modifying code in early "bit-blit" routines.)
        || token_matches(token, "register")) {
      if (storage_class_specifier == NULL) {
        storage_class_specifier = token;
        position++;
      } else {
        return parse_error_result(
            PARSE_ERROR_CONFLICTING_STORAGE_CLASS_SPECIFIER, token);
      }
    } else if (token_matches(token, "inline")) {
      if (function_specifier == NULL) {
        function_specifier = token;
        position++;
      } else {
        return parse_error_result(PARSE_ERROR_CONFLICTING_FUNCTION_SPECIFIER,
                                  token);
      }
    } else if (token_matches(token, "__attribute__")) {
      parse_result_t attribute_result = parse_attribute_node(tokens, position);
      if (is_valid_result(attribute_result)) {
        node_list_add_node(&attributes, attribute_result.node);
        position = attribute_result.next_token_position;
      } else {
        return attribute_result;
      }
    } else {
      break;
    }
  }

  parse_result_t return_type = parse_type_node(tokens, position);
  if (is_empty_result(return_type)) {
    return parse_result_empty();
  }
  position = return_type.next_token_position;
  token_t* fn_name = token_at(tokens, position++);
  if (fn_name->type != TOKEN_TYPE_IDENTIFIER) {
    return parse_result_empty();
  }
  token_t* open_paren = token_at(tokens, position++);
  if (!token_matches(open_paren, "(")) {
    return parse_result_empty();
  }

  function_node_t* fn_node = malloc_function_node();
  fn_node->attributes = attributes;
  fn_node->storage_class_specifier = storage_class_specifier;
  fn_node->function_specifier = function_specifier;
  fn_node->return_type = to_type_node(return_type.node);
  fn_node->function_name = fn_name;

  token_t* next = token_at(tokens, position);
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
  token_t* next = token_at(tokens, position);
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
    token_t* token = token_at(tokens, position++);
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
canonical_type_result_t parse_canonical_type(value_array_t* tokens,
                                             uint64_t position) {
  log_info("parse_canonical_type(_, %d)", position & 0xffffffff);

  token_t* a = token_at(tokens, position);
  token_t* b = token_at(tokens, position + 1);
  token_t* c = token_at(tokens, position + 2);

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

  // First see if we have a canonical type result...
  canonical_type_result_t canonical_type_result
      = parse_canonical_type(tokens, position);
  token_t* type_name = canonical_type_result.canonical_type;
  position += canonical_type_result.consumed_tokens;

  // If it's not canonical, then it could be a struct/union/or enum
  if (type_name == NULL) {
    parse_result_t user_type_result = parse_user_type_node(tokens, position);
    if (is_valid_result(user_type_result)) {
      // TODO(jawilson): we need a kind for a user type
      result->type_node_kind = TYPE_NODE_KIND_TYPE_EXPRESSION;
      result->user_type = user_type_result.node;
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
    token_t* next = token_at(tokens, position);
    if (token_matches(next, "*")) {
      position++;
      type_node_t* ptr_result = malloc_type_node();
      ptr_result->type_node_kind = TYPE_NODE_KIND_POINTER;
      node_list_add_node(&ptr_result->type_args, to_node(result));
      result = ptr_result;
    } else if (token_matches(next, "[")) {
      position++;
      token_t* open = next;
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
  token_t* keyword_token = token_at(tokens, position++);
  if (!token_matches(keyword_token, "enum")) {
    return parse_result_empty();
  }
  enum_node_t* result = malloc_enum_node();
  token_t* token = token_at(tokens, position);
  if (token->type == TOKEN_TYPE_IDENTIFIER) {
    result->name = token;
    position += 1;
    token = token_at(tokens, position);
  }
  if (!token_matches(token, "{")) {
    result->partial_definition = true;
    return parse_result(to_node(result), position);
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
  token_t* name = token_at(tokens, position++);
  result->name = name;
  if (name->type != TOKEN_TYPE_IDENTIFIER) {
    return parse_error_result(PARSE_ERROR_IDENTIFIER_EXPECTED, name);
  }

  token_t* next = token_at(tokens, position);
  if (token_matches(next, "=")) {
    position++;
    token_t* value = token_at(tokens, position);
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
  token_t* typedef_token = token_at(tokens, position++);
  if (!token_matches(typedef_token, "typedef")) {
    return parse_result_empty();
  }

  parse_result_t type = parse_type_node(tokens, position);
  if (!is_valid_result(type)) {
    // ERROR
    return parse_result_empty();
  }
  position = type.next_token_position;
  token_t* name = token_at(tokens, position++);
  if (name->type != TOKEN_TYPE_IDENTIFIER) {
    parse_error_result(PARSE_ERROR_IDENTIFIER_EXPECTED, name);
  }

  typedef_node_t* result = malloc_typedef_node();
  result->type_node = to_type_node(type.node);
  result->name = name;

  token_t* semi = token_at(tokens, position++);
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

  token_t* storage_class_specifier = NULL;

  while (1) {
    token_t* token = token_at(tokens, position);
    if (token_matches(token, "static") || token_matches(token, "extern")) {
      if (storage_class_specifier == NULL) {
        storage_class_specifier = token;
        position++;
      } else {
        return parse_error_result(
            PARSE_ERROR_CONFLICTING_STORAGE_CLASS_SPECIFIER, token);
      }
    } else {
      break;
    }
  }

  parse_result_t type = parse_type_node(tokens, position);
  if (!is_valid_result(type)) {
    // ERROR?
    return parse_result_empty();
  }
  position = type.next_token_position;
  token_t* name = token_at(tokens, position++);
  if (name->type != TOKEN_TYPE_IDENTIFIER) {
    parse_error_result(PARSE_ERROR_IDENTIFIER_EXPECTED, name);
  }

  global_variable_node_t* result = malloc_global_variable_node();
  result->type = to_type_node(type.node);
  result->name = name;

  while (true) {
    // TODO(jawilson): capture array sizes!
    token_t* open_bracket_token = token_at(tokens, position);
    if (token_matches(open_bracket_token, "[")) {
      position++;
      token_t* close_bracket_token = token_at(tokens, position);
      if (!token_matches(close_bracket_token, "]")) {
        return parse_error_result(PARSE_ERROR_CLOSE_BRACKET_EXPECTED,
                                  close_bracket_token);
      }
      position++;
      result->number_of_array_suffixes++;
    } else {
      break;
    }
  }

  token_t* equal_token = token_at(tokens, position);
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

  token_t* semi = token_at(tokens, position++);
  if (!token_matches(semi, ";")) {
    return parse_error_result(PARSE_ERROR_SEMICOLON_EXPECTED, name);
  }

  return parse_result(to_node(result), position);
}

parse_result_t parse_literal_node(value_array_t* tokens, uint64_t position) {
  log_info("parse_literal_node(_, %lld)", position);

  token_t* token = token_at(tokens, position);

  if (token->type == TOKEN_TYPE_STRING_LITERAL) {
    literal_node_t* result = malloc_literal_node();
    result->tokens = make_value_array(1);
    while (true) {
      token_t* token = token_at(tokens, position);
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