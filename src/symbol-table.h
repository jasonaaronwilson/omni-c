// SSCF generated file from: symbol-table.c

#line 2 "symbol-table.c"
#ifndef _SYMBOL_TABLE_H_
#define _SYMBOL_TABLE_H_

#include "parser.h"

/**
 * @struct symbol_table_binding_t
 *
 * A binding contains multiple pieces of data about a symbol and are
 * what are stored in symbol_table_maps's which in turn comprise the
 * symbol table.
 *
 * TODO(jawilson): the unrelated_bindings is a strawman design to
 * allow us to have multiple bindings with the same name that are
 * logically different such as over-loaded function names. These
 * logical lists may occasionally become quite large for example
 * overloaded to_string() functions but it's still better than not
 * using a symbol table at all and having to search through say ALL
 * top-level nodes to to determine a match, etc.
 */
typedef struct symbol_table_binding_S {
  // Unrelated bindings with the same name. I believe only over-loaded
  // functions will use unrelated bindings.
  //
  // struct symbol_table_binding_S* unrelated_bindings;

  // Typically the shortest name (C identifier) associated with a
  // binding such as the function name or the name of a structure.
  char* key_string;

  // This contains all related declaration nodes for something like a
  // function or structure. For example we may have the same prototype
  // one or more times and the actual full definition of a
  // function. Currently the order of these is simply how they are
  // encountered (so the full definition of a function probably comes
  // after the prototype).
  value_array_t* definition_nodes;
} symbol_table_binding_t;

/**
 * @struct symbol_table_map_t
 *
 * A symbol_table_map uses string_hashtable to efficiently find a
 * binding given it's key string (normally C identifiers) but also
 * keeps track of the insertion order so that we can deterministically
 * iterate over the bindings in the symbol_table_map instead of the
 * "random" order that we would get using string_hashtable_foreach.
 */
typedef struct symbol_table_map_S {
  string_hashtable_t* ht;
  // This array may contain NULL instead of a symbol_table_binding
  // once deletion is supported making it slightly sparse.
  value_array_t* ordered_bindings;
} symbol_table_map_t;

/**
 * @struct symbol_table_t
 *
 * This contains the symbols for a program divided up by semantic
 * categories.
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
