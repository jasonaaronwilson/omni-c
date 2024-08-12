#line 2 "parser.c"
#ifndef _PARSER_H_
#define _PARSER_H_

#include "compiler-errors.h"
#include "lexer.h"
#include "node-list.h"
#include <c-armyknife-lib.h>
#include <ctype.h>

/**
 * @file parser.c
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
  PARSE_NODE_VARIABLE_DEFINITION,
  PARSE_NODE_LIST_OF_NODES,
  PARSE_NODE_STRUCT,
  PARSE_NODE_UNION,
  PARSE_NODE_TYPE,
  PARSE_NODE_LITERAL,
  PARSE_NODE_FUNCTION,
  PARSE_NODE_FUNCTION_ARGUMENT,
  PARSE_NODE_TYPEDEF,
  PARSE_NODE_GLOBAL_VARIABLE_DEFINITION,
  PARSE_NODE_UNPARSED_EXPRESSION,
  PARSE_NODE_ATTRIBUTE,
  PARSE_NODE_CPP_INCLUDE,
  PARSE_NODE_CPP_DEFINE,
  PARSE_NODE_OPERATOR,
  PARSE_NODE_IDENTIFIER,
  PARSE_NODE_IF_STATEMENT,
  PARSE_NODE_FOR_STATEMENT,
  PARSE_NODE_DO_STATEMENT,
  PARSE_NODE_WHILE_STATEMENT,
  PARSE_NODE_EMPTY_STATEMENT,
  PARSE_NODE_BLOCK,
  PARSE_NODE_RETURN_STATEMENT,
  PARSE_NODE_SWITCH_STATEMENT,
  PARSE_NODE_CASE_LABEL,
  PARSE_NODE_DEFAULT_LABEL,
  PARSE_NODE_GOTO_STATEMENT,
  PARSE_NODE_BREAK_STATEMENT,
  PARSE_NODE_CONTINUE_STATEMENT,
  PARSE_NODE_LABEL_STATEMENT,
  PARSE_NODE_VARIABLE_STATEMENT,
  PARSE_NODE_EXPRESSION_STATEMENT,
  PARSE_NODE_BALANCED_CONSTRUCT,
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
typedef struct parse_node_S {
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
/* Inlined helpers for parse_result_t implementation */
/* ====================================================================== */

static inline parse_result_t parse_result_empty(void) {
  return (parse_result_t){0};
}

static inline boolean_t is_empty_result(parse_result_t result) {
  return (result.parse_error.parse_error_code == PARSE_ERROR_UNKNOWN)
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
                                                token_t* error_token) {
  return (parse_result_t){.parse_error = (compiler_error_t){
                              .parse_error_code = error_code,
                              .error_token = error_token,
                          }};
}

static inline boolean_t is_error_result(parse_result_t result) {
  return result.parse_error.parse_error_code != 0;
}

static inline boolean_t is_valid_result(parse_result_t result) {
  return result.node != NULL;
}

/**
 * @typedef pstatus_t
 *
 * This can be seen as success versus failure for in built-in
 * "pstate_" routines or a user written parse routine. We use
 * pstatus_t when we want to make it clear that the routine modifies
 * the passed in pstate_t* object according to our "calling
 * convention" versus only peeking at it's state.
 *
 * Whenever pstatus_t is false, the caller should either immediately
 * propagate the error to the caller or else explictly ignore the
 * error.
 *
 * Note that certain conditions can still cause a fatal error which
 * implies either we ran out of memory or have a bug in the parser.
 */

typedef boolean_t pstatus_t;

/**
 * @struct pstate_t
 *
 * After writing some of the parser by hand, one observation is that
 * the "calling convention" itself is kind of clunky, especially the
 * error path which there are many. Another observation is that while
 * we like the freedom of a random access token array (especially
 * because this makes "roll-back" simpler versus an actual stream) we
 * mostly really do what something that operates more like a stream so
 * that we manage the stream position in a more predictable/natural
 * way.
 *
 * The new "calling convention" is inspired by the old one but:
 *
 * 1. makes it about as easy to *return* a result but much easier to
 * "handle" the failure case. This is because we return a boolean
 * derived type which can be used directly in an if statement. The
 * actual error information is "passed back" by modifying the passed
 * in pstate_t structure. It also means we can easily add other error
 * fields.
 *
 * 2. allows for "returning" either a token or a parse node where as
 * previously we mostly focused on returning parse nodes.
 *
 * The consequence of #1 is that we can now use "||" (or "&&") which
 * makes certain things much denser (so it will feel easier).
 *
 * In term of efficieny, we will still be doing lots of string
 * comparisons to match terminal tokens. In the future we hope to use
 * "hashing" to provide token identifiers which will allow us to use
 * switch which should be much cheaper in certain circumstances
 * without radically changing the basic structure.
 */
typedef struct {
  value_array_t* tokens;
  uint64_t position;
  boolean_t use_statement_parser;
  parse_node_t* result_node;
  token_t* result_token;
  compiler_error_t error;
} pstate_t;

/* ====================================================================== */
/* Forward declarations all the other routines. */
/* ====================================================================== */

#include "parser.c.generated.h"

#endif /* _PARSER_H_ */

char* parse_error_code_to_string_1(parse_error_code_t value) {
  switch (value) {
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
  case PARSE_ERROR_CLOSE_BRACKET_EXPECTED:
    return "PARSE_ERROR_CLOSE_BRACKET_EXPECTED";
  case PARSE_ERROR_UNRECOGNIZED_TOP_LEVEL_DECLARATION:
    return "PARSE_ERROR_UNRECOGNIZED_TOP_LEVEL_DECLARATION";
  case PARSE_ERROR_SEMICOLON_EXPECTED:
    return "PARSE_ERROR_SEMICOLON_EXPECTED";
  case PARSE_ERROR_CONFLICTING_STORAGE_CLASS_SPECIFIER:
    return "PARSE_ERROR_CONFLICTING_STORAGE_CLASS_SPECIFIER";
  case PARSE_ERROR_CONFLICTING_FUNCTION_SPECIFIER:
    return "PARSE_ERROR_CONFLICTING_FUNCTION_SPECIFIER";
  case PARSE_ERROR_EXPECTED_OPEN_PAREN_AFTER_UNDERSCORE_ATTRIBUTE:
    return "PARSE_ERROR_EXPECTED_OPEN_PAREN_AFTER_UNDERSCORE_ATTRIBUTE";
  case PARSE_ERROR_EXPECTED_MATCHING_CLOSE_PAREN_AFTER_UNDERSCORE_ATTRIBUTE:
    return "PARSE_ERROR_EXPECTED_MATCHING_CLOSE_PAREN_AFTER_UNDERSCORE_"
           "ATTRIBUTE";
  case PARSE_ERROR_EXPECTED_PREFIX_OPERATOR_OR_TERMINAL:
    return "PARSE_ERROR_EXPECTED_PREFIX_OPERATOR_OR_TERMINAL";
  default:
    return "<<unknown-parse_error_code>>";
  }
}
