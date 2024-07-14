#line 2 "sourcer-to-source.c"
#ifndef _SOURCE_TO_SOURCE_H_
#define _SOURCE_TO_SOURCE_H_

#include "parser.h"
#include "symbol-table.h"
#include <c-armyknife-lib.h>

// Make sure this comes after symbol-table.h. We've gotten lucky so
// far because most files define something between the includes and
// the inclusion of the generated header file.

#include "source-to-source.c.generated.h"

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

oc_token_t* generate_struct_name_from_typedef_name(oc_token_t* name) {
  oc_token_t* generated = make_derived_token(name);
  buffer_append_string(generated->buffer, "__generated_S");
  generated->end = generated->buffer->length;
  return generated;
}

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
      struct_node_t* struct_node = to_struct_node(node->type_node->user_type);
      if (!struct_node->partial_definition) {
        if (struct_node->name == NULL) {
          struct_node->name
              = generate_struct_name_from_typedef_name(node->name);
        }
        log_info("Splitting %s off from %s", token_to_string(node->name),
                 token_to_string(struct_node->name));
      }
    }
  }
}
