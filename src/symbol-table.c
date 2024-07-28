#line 2 "symbol-table.c"
#ifndef _SYMBOL_TABLE_H_
#define _SYMBOL_TABLE_H_

#include "debug-printer.h"
#include "file-reader.h"
#include "parser.h"
#include "token-transformer.h"
#include <c-armyknife-lib.h>

/**
 * @struct symbol_table_binding_t
 *
 * A symbol_table_binding holds information related to user
 * definitions, most notably one or more definition parse nodes (aka
 * declarations). This allows them to contain both a prototype and a
 * function definition or even a repeated definition.
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

  // Additional bindings with the same name but a different
  // definition. I believe only over-loaded functions will need to use
  // unrelated bindings since where C allows two things of the same
  // name, such as a type and a function name, this is handled by
  // having multiple hashtables by definition type.
  //
  // struct symbol_table_binding_S* unrelated_bindings;

  // This is used when reordering bindings using DFS. See
  // source-to-source.c.
  boolean_t visited;

} symbol_table_binding_t;

/**
 * @struct symbol_table_map_t
 *
 * Our goal is O(1) lookup given a "name" as well as allowing a
 * well-determined iteration order (in this case based on the source
 * code order).
 */
typedef struct symbol_table_map_S {
  string_hashtable_t* ht;
  // This array *may* eventually contain NULLs instead of a
  // symbol_table_binding once deletion is supported making it
  // "sparse". We will hopefully keep this abstract via macros like
  // for_each_symbol_table_binding.
  value_array_t* ordered_bindings;
} symbol_table_map_t;

/**
 * @struct symbol_table_t
 *
 * This contains the symbols for a program divided up by semantic
 * categories. The parent field will allow symbol_table's to represent
 * "local" variables and such (though perhaps only variables need to
 * be looked up this way...)
 */
typedef struct symbol_table_S {
  struct symbol_table_S* parent;
  value_array_t* system_includes;
  value_array_t* user_includes;
  value_array_t* defines;
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
  result->system_includes = make_value_array(16);
  result->user_includes = make_value_array(16);
  result->defines = make_value_array(16);
  result->enums = make_symbol_table_map();
  result->typedefs = make_symbol_table_map();
  result->structures = make_symbol_table_map();
  result->variables = make_symbol_table_map();
  result->functions = make_symbol_table_map();
  return result;
}

/**
 * @function symbol_table_map_get
 *
 * Get an existing binding or return NULL.
 */
symbol_table_binding_t* symbol_table_map_get(symbol_table_map_t* map,
                                             char* key_string) {
  value_result_t result = string_ht_find(map->ht, key_string);
  if (is_ok(result)) {
    return cast(symbol_table_binding_t*, result.ptr);
  }
  return NULL;
}

/**
 * @function symbol_table_map_get_only_definition
 *
 * Our data-structures support more than one definition for a given
 * name because we want to support partial (incremental) definition of
 * structures as well as eventually overloaded functions. In other
 * cases, this is more concise.
 */
parse_node_t* symbol_table_map_get_only_definition(symbol_table_map_t* map,
                                                   char* key_string) {
  value_result_t result = string_ht_find(map->ht, key_string);
  if (is_ok(result)) {
    symbol_table_binding_t* binding = cast(symbol_table_binding_t*, result.ptr);
    if (binding->definition_nodes->length != 1) {
      fatal_error(ERROR_ILLEGAL_STATE);
    }
    return value_array_get_ptr(binding->definition_nodes, 0, parse_node_t*);
  }
  return NULL;
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
  // The choice of initial elements is overkill for a large number of
  // top level forms. A linked list might not be a bad choice here but
  // c-armyknife-lib doesn't implement simple linked-lists yet. Let's
  // get the code to work right first.
  binding->definition_nodes = make_value_array(2);
  value_array_add(binding->definition_nodes, ptr_to_value(node));
  map->ht
      = string_ht_insert(map->ht, binding->key_string, ptr_to_value(binding));
  value_array_add(map->ordered_bindings, ptr_to_value(binding));
}

/**
 * @function symbol_table_add_declartions
 *
 * Given a top-level declarations_node, insert all of it's children
 * into the symbol table.
 */
void symbol_table_add_declartions(symbol_table_t* symbol_table,
                                  declarations_node_t* root) {
  uint64_t length = node_list_length(root->declarations);
  for (uint64_t i = 0; i < length; i++) {
    parse_node_t* node = node_list_get(root->declarations, i);
    switch (node->tag) {

    case PARSE_NODE_ENUM:
      symbol_table_add_declaration_node(
          symbol_table->enums, token_to_string(to_enum_node(node)->name), node);
      break;

    case PARSE_NODE_FUNCTION:
      symbol_table_add_declaration_node(
          symbol_table->functions,
          token_to_string(to_function_node(node)->function_name), node);
      break;

    case PARSE_NODE_GLOBAL_VARIABLE_DEFINITION:
      symbol_table_add_declaration_node(
          symbol_table->variables,
          token_to_string(to_global_variable_node(node)->name), node);
      break;

    case PARSE_NODE_STRUCT:
      symbol_table_add_declaration_node(
          symbol_table->structures, token_to_string(to_struct_node(node)->name),
          node);
      break;

    case PARSE_NODE_TYPEDEF:
      symbol_table_add_declaration_node(
          symbol_table->typedefs, token_to_string(to_typedef_node(node)->name),
          node);
      break;

    default:
      fatal_error(ERROR_ILLEGAL_STATE);
      break;
    }
  }
}

buffer_t* symbol_table_stats(buffer_t* buffer, symbol_table_t* symbol_table) {
  buffer = buffer_printf(buffer, "Symbol Table Stats\n");
  buffer = buffer_printf(buffer, "#enums %d\n",
                         string_ht_num_entries(symbol_table->enums->ht));
  buffer = buffer_printf(buffer, "#typedefs %d\n",
                         string_ht_num_entries(symbol_table->typedefs->ht));
  buffer = buffer_printf(buffer, "#structures %d\n",
                         string_ht_num_entries(symbol_table->structures->ht));
  buffer = buffer_printf(buffer, "#variables %d\n",
                         string_ht_num_entries(symbol_table->variables->ht));
  buffer = buffer_printf(buffer, "#functions %d\n",
                         string_ht_num_entries(symbol_table->functions->ht));
  return buffer;
}

buffer_t* buffer_append_dgb_binding(buffer_t* buffer,
                                    symbol_table_binding_t* binding) {
  buffer_printf(buffer, "%s:\n", binding->key_string);
  // Cheat for now. just the first parse_node_t
  buffer = buffer_append_dbg_parse_node(
      buffer, value_array_get_ptr(binding->definition_nodes, 0, parse_node_t*),
      1);
  return buffer;
}

buffer_t*
    buffer_appennd_dbg_symbol_table_map(buffer_t* buffer,
                                        symbol_table_map_t* symbol_table_map) {
  for (int i = 0; i < symbol_table_map->ordered_bindings->length; i++) {
    if (i > 0) {
      buffer = buffer_printf(buffer, "\n");
    }
    symbol_table_binding_t* binding = value_array_get_ptr(
        symbol_table_map->ordered_bindings, i, symbol_table_binding_t*);
    buffer_append_dgb_binding(buffer, binding);
  }
  return buffer;
}

buffer_t* buffer_append_dgb_symbol_table(buffer_t* buffer,
                                         symbol_table_t* symbol_table) {
  buffer = buffer_printf(buffer,
                         "\n========================= Begin Symbol Table "
                         "=========================\n");
  buffer = buffer_printf(buffer, "*** Symbol Table Enumerations ***\n");
  buffer = buffer_appennd_dbg_symbol_table_map(buffer, symbol_table->enums);
  buffer = buffer_printf(buffer, "\n*** Symbol Table Typedefs ***\n");
  buffer = buffer_appennd_dbg_symbol_table_map(buffer, symbol_table->typedefs);
  buffer = buffer_printf(buffer, "\n*** Symbol Table Structures ***\n");
  buffer
      = buffer_appennd_dbg_symbol_table_map(buffer, symbol_table->structures);
  buffer = buffer_printf(buffer, "\n*** Symbol Table Variables ***\n");
  buffer = buffer_appennd_dbg_symbol_table_map(buffer, symbol_table->variables);
  buffer = buffer_printf(buffer, "\n*** Symbol Table Functions ***\n");
  buffer = buffer_appennd_dbg_symbol_table_map(buffer, symbol_table->functions);
  buffer = buffer_printf(buffer,
                         "========================= End Symbol Table "
                         "=========================\n\n");
  return buffer;
}
