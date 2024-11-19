#line 2 "type-parser.c"
#ifndef _TYPE_PARSER_H_
#define _TYPE_PARSER_H_

#include <c-armyknife-lib.h>
#include <ctype.h>

#include "compiler-errors.h"
#include "literal-parser.h"
#include "parser.h"
#include "pstate.h"

/**
 * @enum type_qualifier_t
 *
 * Represents the set of qualifiers that can be assigned to a type.
 *
 * TODO(jawilson): maybe _Atomic is a qualifier? Gemni claims that
 * typeof_unqual strips this from typeof so it's probably going to pop
 * up.
 */
typedef enum {
  TYPE_QUALIFIER_NONE = 0,
  TYPE_QUALIFIER_CONST = 1,
  TYPE_QUALIFIER_VOLATILE = 2,
  TYPE_QUALIFIER_RESTRICT = 4,
} type_qualifier_t;

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

  type_qualifier_t qualifiers;

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
 * @function malloc_type_node
 */
static inline type_node_t* malloc_type_node(void) {
  type_node_t* result = malloc_struct(type_node_t);
  result->tag = PARSE_NODE_TYPE;
  return result;
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

typedef struct canonical_type_result_s {
  token_t* canonical_type;
  int consumed_tokens;
} canonical_type_result_t;

#include "type-parser.c.generated.h"

pstatus_t parse_user_type_node(pstate_t* pstate);
pstatus_t parse_enum_node(pstate_t* pstate);
pstatus_t parse_structure_node(pstate_t* pstate);
pstatus_t parse_union_node(pstate_t* pstate);

#endif /* _TYPE_PARSER_H_ */

/**
 * @function parse_type_node
 *
 * Parses any C type.
 */
pstatus_t parse_type_node(pstate_t* pstate) {
  uint64_t saved_position = pstate->position;

  if (parse_function_type(pstate)) {
    return true;
  }

  type_node_t* result = malloc_type_node();

  type_qualifier_t qualifiers = TYPE_QUALIFIER_NONE;

  // TODO(jawilson): volatile and restrict all in any order.
  if (pstate_match_token_string(pstate, "const")) {
    pstate_advance(pstate);
    qualifiers |= TYPE_QUALIFIER_CONST;
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
  result->qualifiers = qualifiers;
  return pstate_set_result_node(pstate, to_node(result));
}

canonical_type_result_t make_type_token_result(char* str, int consumed_tokens) {
  token_t* canonical_token = malloc_struct(typeof(token_t));
  canonical_token->type = TOKEN_TYPE_IDENTIFIER;
  canonical_token->buffer = buffer_from_string(str);
  canonical_token->start = 0;
  canonical_token->end = strlen(str);
  return compound_literal(
      canonical_type_result_t,
      {.canonical_type = canonical_token, .consumed_tokens = consumed_tokens});
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

  return compound_literal(canonical_type_result_t,
                          {.canonical_type = NULL, .consumed_tokens = 0});
}

/**
 * @function parse_function_type
 *
 * Parses a special kind of function pointer type that might look
 * like:
 *
 * ```
 * fn_t(void, int var_name_1, int*)
 * ```
 *
 * (where var_name is an optional (currently ignored) identifier).
 */
pstatus_t parse_function_type(pstate_t* pstate) {
  uint64_t saved_position = pstate->position;

  token_t* fn_t_token = pstate_peek(pstate, 0);

  if (!pstate_expect_token_string(pstate, "fn_t")
      || !pstate_expect_token_string(pstate, "(")) {
    return pstate_propagate_error(pstate, saved_position);
  }

  type_node_t* result = malloc_type_node();
  result->type_node_kind = TYPE_NODE_KIND_TYPE_EXPRESSION;
  result->type_name = fn_t_token;

  // Parse the return type
  if (!parse_type_node(pstate)) {
    return pstate_propagate_error(pstate, saved_position);
  }
  node_list_add_node(&result->type_args, pstate_get_result_node(pstate));

  // Parse all of the arguments
  while (parse_function_type_argument(pstate)) {
    node_list_add_node(&result->type_args, pstate_get_result_node(pstate));
  }

  if (!pstate_expect_token_string(pstate, ")")) {
    return pstate_propagate_error(pstate, saved_position);
  }

  return pstate_set_result_node(pstate, to_node(result));
}


/**
 * @function parse_function_type_argument
 *
 * Parse one of the arguments types for a function pointer type. Since
 * these always appear after a return type, we expect to be looking at
 * either a "," which we eat. After parsing the type after the ",", we
 * parse a type and then there is possibly an argumenent name which we
 * eat.
 */
pstatus_t parse_function_type_argument(pstate_t* pstate) {
  uint64_t saved_position = pstate->position;

  // First eat the initial comma
  if (!pstate_expect_token_string(pstate, ",")) {
    return pstate_propagate_error(pstate, saved_position);
  }

  // Now parse the type
  if (!parse_type_node(pstate)) {
    return pstate_propagate_error(pstate, saved_position);
  }

  // Optionally parse an identifier that is thrown away.
  token_t* suffix = pstate_peek(pstate, 0);
  if (suffix->type == TOKEN_TYPE_IDENTIFIER) {
    pstate_advance(pstate);
  }

  return true;
}
