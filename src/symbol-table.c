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

symbol_table_map_t* make_symbol_table_map(void) {
  symbol_table_map_t* result = malloc_struct(symbol_table_map_t);
  result->ht = make_string_hashtable(16);
  result->ordered_bindings = make_value_array(16);
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

void symbol_table_add_declaration_node(symbol_table_map_t* map,
                                       char* key_string, parse_node_t* node) {
  value_result_t previous_binding = string_ht_find(map->ht, key_string);
  if (is_ok(previous_binding)) {
    symbol_table_binding_t* binding
        = cast(symbol_table_binding_t*, previous_binding.ptr);
    value_array_add(binding->definition_nodes, ptr_to_value(node));
    return;
  }
  symbol_table_binding_t* binding = malloc_struct(symbol_table_binding_t);
  binding->key_string = key_string;
  // This is overkill for a large number of top level forms but it's
  // not as terrible per se as using an array when we could use a
  // linked list (not available in c-armyknife-lib). Let's get the
  // code to work right first any value_arrays are simple enough that
  // I trust them.
  binding->definition_nodes = make_value_array(2);
  value_array_add(binding->definition_nodes, ptr_to_value(node));
  map->ht
      = string_ht_insert(map->ht, binding->key_string, ptr_to_value(binding));
  value_array_add(map->ordered_bindings, ptr_to_value(binding));
}

void symbol_table_add_declartions(symbol_table_t* symbol_table,
                                  declarations_node_t* root) {
  uint64_t length = node_list_length(root->declarations);
  for (uint64_t i = 0; i < length; i++) {
    parse_node_t* node = node_list_get(root->declarations, i);
    switch (node->tag) {
    case PARSE_NODE_ENUM:
      symbol_table_add_declaration_node(
          symbol_table->enums, token_to_string(*(to_enum_node(node)->name)),
          node);
      break;
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
