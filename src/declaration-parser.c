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

  parse_node_t* body;
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

/* ====================================================================== */
/* These feel like they can be automatically generated or become a C
   preprocessor macro - but that wouldn't work with our "javadoc"
   tool. Hmmm. */
/* ====================================================================== */

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

static inline typedef_node_t* malloc_typedef_node(void) {
  typedef_node_t* result = malloc_struct(typedef_node_t);
  result->tag = PARSE_NODE_TYPEDEF;
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
/* Forward declarations all the other routines. */
/* ====================================================================== */

#include "declaration-parser.c.generated.h"

#endif /* _DECLARATION_PARSER_H_ */

#include "statement-parser.h"

/* ====================================================================== */
/* Implementation of non-inlined functions */
/* ====================================================================== */

/**
 * @function parse_declarations
 *
 * This is the main entry-point to the parser. Keep parsing
 * declarations until the end of the file is reached. By this point in
 * time we have removed all of the c-preprocessor instructions.
 */
pstatus_t parse_declarations(pstate_t* pstate) {
  uint64_t saved_position = pstate->position;
  declarations_node_t* result = malloc_declarations();
  while (pstate->position < pstate->tokens->length) {
    if (!parse_declaration(pstate)) {
      return pstate_propagate_error(pstate, saved_position);
    }
    node_list_add_node(&result->declarations, pstate_get_result_node(pstate));
  }
  return pstate_set_result_node(pstate, to_node(result));
}

/**
 * @function parse_declaration
 */
pstatus_t parse_declaration(pstate_t* pstate) {
  uint64_t saved_position = pstate->position;
  if (parse_function_node(pstate)
      || parse_typedef_node(pstate_ignore_error(pstate))
      || parse_enum_node_declaration(pstate_ignore_error(pstate))
      || parse_variable_definition_node(pstate_ignore_error(pstate))
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
    fn_node->body = pstate_get_result_node(pstate);
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
  if (pstate->use_statement_parser) {
    log_info("USING STATEMENT PARSER");
    return parse_block(pstate);
  } else {
    return parse_balanced_construct(pstate);
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
