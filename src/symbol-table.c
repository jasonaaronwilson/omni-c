#line 2 "symbol-table.c"
#ifndef _SYMBOL_TABLE_H_
#define _SYMBOL_TABLE_H_

#include "parser.h"

/**
 * @struct symbol_table_binding_t
 *
 * These hold multiple pieces of data about a symbol in one the the
 * maps below.
 *
 * The next_binding field is only meant to be used by
 * symbol_table_map_t which should not be dependeded upon.
 */
typedef struct symbol_table_binding_S {
  struct symbol_table_binding_S* next_binding;
  parse_node_t* definition_node;
} symbol_table_binding_t;

/**
 * @struct symbol_table_map_t
 *
 * A poor man's (insert only) "linked-hash-map".
 *
 * While string_hashtable_t can be iteratated over, this iteration
 * order is not fixed and could lead to more unintentional deltas in
 * generated code so we remember the order at which bindings are added
 * to each map.
 */
typedef struct symbol_table_map_S {
  string_hashtable_t* map;
  symbol_table_binding_t* first_binding;
  symbol_table_binding_t* last_binding;
} symbol_table_map_t;

/**
 * @struct symbol_table_t
 *
 * This contains the symbols for a program divided up by categories
 * for easy traversal.
 */
typedef struct symbol_table_S {
  symbol_table_map_t* enums;
  symbol_table_map_t* typedefs;
  symbol_table_map_t* structures;
  symbol_table_map_t* variables;
  symbol_table_map_t* functions;
} symbol_table_t;

#include "symbol-table.c.generated.h"

#endif /* _SYMBOL_TABLE_H_ */

symbol_table_map_t* make_symbol_table_map(void) {
  symbol_table_map_t* result = malloc_struct(symbol_table_map_t);
  result->map = make_string_hashtable(16);
  return result;
}

symbol_table_t* make_symbol_table(void) {
  symbol_table_t* result = malloc_struct(symbol_table_t);
  result->enums = make_symbol_table_map();
  result->typedefs = make_symbol_table_map();
  result->structures = make_symbol_table_map();
  result->variables = make_symbol_table_map();
  result->functions = make_symbol_table_map();
  return result;
}

void symbol_table_add_children(declarations_node_t* root) {
  uint64_t length = node_list_length(root->declarations);
  for (uint64_t i = 0; i < length; i++) {
    parse_node_t* node = node_list_get(root->declarations, i);
    switch (node->tag) {
    case PARSE_NODE_ENUM:
    case PARSE_NODE_FUNCTION:
    case PARSE_NODE_GLOBAL_VARIABLE_DEFINITION:
    case PARSE_NODE_STRUCT:
    case PARSE_NODE_TYPEDEF:
      break;

    default:
      fatal_error(ERROR_ILLEGAL_STATE);
      break;
    }
  }
}
