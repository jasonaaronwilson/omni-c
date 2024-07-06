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

// #include "symbol-table.c.generated.h"

#endif /* _SYMBOL_TABLE_H_ */
