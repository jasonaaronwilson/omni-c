#line 2 "sourcer-to-source.c"
#ifndef _SOURCE_TO_SOURCE_H_
#define _SOURCE_TO_SOURCE_H_

#include "parser.h"
#include "source-to-source.c.generated.h"
#include "symbol-table.h"
#include <c-armyknife-lib.h>

#endif /* _SOURCE_TO_SOURCE_H_ */

/**
 * @file source-to-source.c
 *
 * These are a collection of small source to source transformations
 * that make other anaylsis or other transformation a little
 * simpler. While the order of transformation are not always
 * important, the order they appear in this file should be
 * appropriate.
 */

/**
 * @function split_structure_typedefs
 *
 * Given a fairly typical (at least in this code base) construct like:
 *
 * ```
 *   typedef struct foo_S {} foo_t;
 * ```
 *
 * Convert this into two independent pieces:
 *
 * ```
 *  typedef struct foo_S foo_t;
 *  struct foo_S {};
 * ```
 */

// token_t* make_token...(buffer_t token_buffer) {
//
// }

void split_structure_typedefs(symbol_table_t* symbol_table) {
  for (int i = 0; i < symbol_table->typedefs->ordered_bindings->length; i++) {
    symbol_table_binding_t* binding = cast(
        symbol_table_binding_t*,
        value_array_get(symbol_table->typedefs->ordered_bindings, i).ptr);
    if (binding->definition_nodes->length != 1) {
      fatal_error(ERROR_ILLEGAL_STATE);
    }
    typedef_node_t* node = to_typedef_node(
        cast(parse_node_t*, value_array_get(binding->definition_nodes, 0).ptr));
    if (node->type_node->type_node_kind == TYPE_NODE_KIND_TYPE_EXPRESSION
        && is_struct_node(node->type_node->user_type)) {
      fprintf(stdout, "We should split this node? %s\n",
              token_to_string(node->name));
    }
  }
}
