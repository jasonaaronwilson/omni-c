#line 2 "source-to-source.c"
#ifndef _SOURCE_TO_SOURCE_H_
#define _SOURCE_TO_SOURCE_H_

#include "parser.h"
#include "symbol-table.h"
#include <c-armyknife-lib.h>

// Make sure this include comes after symbol-table.h. We've gotten
// lucky so far because most files define something between the
// includes and the inclusion of the generated header file.

#include "source-to-source.c.generated.h"

#endif /* _SOURCE_TO_SOURCE_H_ */

/**
 * @file source-to-source.c
 *
 * These are a collection of small source to source transformations
 * aimed at being able to output "C" (to various standards, eventually
 * I want to transpile to older standards for retro enthusiasts) from
 * our input language "Omni C" - itself is a super-set of "ISO C" with
 * the "good" parts of non OOP C++ thrown in (templates and function
 * overloading). The key differentiation we are originally aiming for
 * versus C is not having to write header files which means you also
 * don't have to worry about the order of declarations.
 *
 * If we ever produce assembly language, these transformation may not
 * be necessary but are also not hamrful.
 */

oc_token_t* generate_struct_name_from_typedef_name(oc_token_t* name) {
  oc_token_t* generated = make_derived_token(name);
  buffer_append_string(generated->buffer, "__generated_S");
  generated->end = generated->buffer->length;
  return generated;
}

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

        // Below the few lines that do the actual transformation I've
        // left kind of like what the transformation looks like in
        // practice..
        //
        // We want to take the PARSE_NODE_STRUCT and add it into the
        // symbol table as it's own independent entry (we made sure it
        // wasn't anonymous above) and replace it with a "partial
        // definition" struct node. We've now made compilation (to ISO
        // C) slightly easier by just needing to properly sort
        // typedefs and independently properly sorting structures.

        struct_node_t* partial_definition = malloc_struct_node();
        partial_definition->partial_definition = true;
        partial_definition->name = struct_node->name;

        node->type_node->user_type = cast(parse_node_t*, partial_definition);
        symbol_table_add_declaration_node(symbol_table->structures,
                                          token_to_string(struct_node->name),
                                          cast(parse_node_t*, struct_node));

        // declaration[4]:
        //     tag: PARSE_NODE_TYPEDEF
        //     name: before_split_t
        //     type_node:
        //         tag: PARSE_NODE_TYPE
        //         type_node_kind: TYPE_NODE_KIND_TYPE_EXPRESSION
        //         user_type:
        //             tag: PARSE_NODE_STRUCT
        //             name: before_split_S
        //             partial_definition: false
        //             field[0]:
        //                 tag: PARSE_NODE_FIELD
        //                 name: int64_field
        //                 type:
        //                     tag: PARSE_NODE_TYPE
        //                     type_node_kind: TYPE_NODE_KIND_TYPENAME
        //                     type_name: uint64_t
        // declaration[5]:
        //     tag: PARSE_NODE_STRUCT
        //     name: after_split_S
        //     partial_definition: false
        //     field[0]:
        //         tag: PARSE_NODE_FIELD
        //         name: int64_field
        //         type:
        //             tag: PARSE_NODE_TYPE
        //             type_node_kind: TYPE_NODE_KIND_TYPENAME
        //             type_name: uint64_t
        // declaration[6]:
        //     tag: PARSE_NODE_TYPEDEF
        //     name: after_split_t
        //     type_node:
        //         tag: PARSE_NODE_TYPE
        //         type_node_kind: TYPE_NODE_KIND_TYPE_EXPRESSION
        //         user_type:
        //             tag: PARSE_NODE_STRUCT
        //             name: after_split_S
        //             partial_definition: true
      }
    }
  }
}

// ======================================================================

// TODO(jawilson): split typedef enums

// ======================================================================

/**
 * @function reorder_symbol_table_typedefs
 *
 * Given simplified typedefs, reorder them in the symbol table when
 * there is a dependency between them.
 *
 * BTW, my plan is that Omni C will eventually allow "=" with
 * typedef's which honestly makes this a bit easier to describe so
 * let's go ahead and use that syntax:
 *
 * ```
 * typedef B = A; // #1
 * typedef A = W; // #2
 *```
 *
 * And just like that, certain programmers will know that #2 should
 * come before #1 when we are going to compile to C.
 *
 * Since we've already split structure typedefs, they don't have
 * dependencies but other typedefs may depend on them. Enums (unless
 * they specify a type which I think might eventually be allowed) are
 * similar. Same goes to a typedef to a native type (or a typedef
 * where we simply don't know about because it comes from a header
 * type we didn't process). However we do need to check typedefs like
 * this:
 *
 * ```
 * typedef A B;
 * ```
 *
 * Just in case A is itself the result of a typedef.
 *
 * For a function typedef, we may need to look for the return type and
 * all of the argument types to make sure that all of those appear
 * first.
 */
void reorder_symbol_table_typedefs(symbol_table_t* symbol_table) {
  value_array_t* bindings = symbol_table->typedefs->ordered_bindings;
  value_array_t* reordered_bindings = make_value_array(bindings->length);
  for (int i = 0; i < bindings->length; i++) {
    symbol_table_binding_t* binding
        = cast(symbol_table_binding_t*, value_array_get(bindings, i).ptr);
    reorder_symbol_table_typedefs__process_binding(symbol_table->typedefs,
                                                   binding, reordered_bindings);
  }
  symbol_table->typedefs->ordered_bindings = reordered_bindings;
}

void reorder_symbol_table_typedefs__process_binding(
    symbol_table_map_t* typedefs, symbol_table_binding_t* binding,
    value_array_t* reordered_bindings) {
  log_warn("JASON looking at %s", binding->key_string);
  if (!binding->visited) {
    if (binding->definition_nodes->length != 1) {
      fatal_error(ERROR_ILLEGAL_STATE);
    }
    parse_node_t* node = cast(
        parse_node_t*, value_array_get(binding->definition_nodes, 0).ptr);
    typedef_node_t* typedef_node = to_typedef_node(node);
    type_node_t* type_node = typedef_node->type_node;
    // TODO(jawilson): ARRAY, SIZED_ARRAY, VARIABLE_SIZED_ARRAY
    while (type_node->type_node_kind == TYPE_NODE_KIND_POINTER) {
      type_node = to_type_node(node_list_get(type_node->type_args, 0));
    }
    if (type_node->type_node_kind == TYPE_NODE_KIND_TYPENAME) {
      char* type_name = token_to_string(type_node->type_name);
      symbol_table_binding_t* dependent_binding
          = symbol_table_map_get(typedefs, type_name);
      if (dependent_binding != NULL) {
        reorder_symbol_table_typedefs__process_binding(
            typedefs, dependent_binding, reordered_bindings);
      }
    } else if (type_node->type_node_kind == TYPE_NODE_KIND_TYPE_EXPRESSION) {
      // I think there is nothing to do for this case since either
      // enum, struct, or union. enum's will come first anyways and
      // struct or unions should have been split by
      // split_structure_typedefs and can thus be handled below all of
      // the typedefs.
    } else if (type_node->type_node_kind != TYPE_NODE_KIND_PRIMITIVE_TYPENAME) {
      fatal_error(ERROR_ILLEGAL_STATE);
    }

    log_warn("JASON adding %s at position %d", binding->key_string,
             reordered_bindings->length);
    // TODO(jawilson): actually try to process dependencies first!
    value_array_add(reordered_bindings, ptr_to_value(binding));
    binding->visited = true;
  }
}

// ======================================================================
// reorder_symbol_table_structures
// ======================================================================

void reorder_symbol_table_structures_process_binding(
    symbol_table_t* symbol_table, symbol_table_binding_t* binding,
    value_array_t* reordered_bindings) {
  log_warn("JASON looking at %s", binding->key_string);
  if (!binding->visited) {
    if (binding->definition_nodes->length != 1) {
      fatal_error(ERROR_ILLEGAL_STATE);
    }
    parse_node_t* node = cast(
        parse_node_t*, value_array_get(binding->definition_nodes, 0).ptr);
    struct_node_t* structure_node = to_struct_node(node);

    uint64_t length = node_list_length(structure_node->fields);
    for (uint64_t i = 0; i < length; i++) {
      field_node_t* field
          = to_field_node(node_list_get(structure_node->fields, i));
      type_node_t* type_node = field->type;

      if (type_node->type_node_kind == TYPE_NODE_KIND_TYPENAME) {
        // TODO(jawwilson): write and call resolve typedef.
      } else if (type_node->type_node_kind == TYPE_NODE_KIND_TYPE_EXPRESSION
                 && is_struct_node(type_node->user_type)) {
        struct_node_t* struct_node = to_struct_node(type_node->user_type);
        if (!struct_node->partial_definition || struct_node->name == NULL) {
          fatal_error(ERROR_ILLEGAL_STATE);
        }
        char* key_string = token_to_string(struct_node->name);
        symbol_table_binding_t* dependent_structure
            = symbol_table_map_get(symbol_table->structures, key_string);
        if (dependent_structure != NULL) {
          if (dependent_structure == binding) {
            // Trivial self-recursive case. It's OK to have a field
            // that is a pointer to your own structure but not a full
            // value since it would be an infinite regress...
            fatal_error(ERROR_ILLEGAL_STATE);
          }

          reorder_symbol_table_structures_process_binding(
              symbol_table, dependent_structure, reordered_bindings);
        }
      }
    }

    // TODO(jawilson): recursively visit each field. We probably need
    // to step into typenames..

    value_array_add(reordered_bindings, ptr_to_value(binding));
    binding->visited = true;
  }
}

/**
 * @function reorder_symbol_table_structures
 *
 * This will be similar to reorder_symbol_table_typedefs but for
 * structures.
 *
 * Unlike reorder_symbol_table_typedefs__process_binding, we don't
 * need to worry about pointers, only arrays and either typenames
 * (that resolve to non-pointers or arrays) or non pointed to struct
 * fields.
 */
void reorder_symbol_table_structures(symbol_table_t* symbol_table) {
  value_array_t* bindings = symbol_table->structures->ordered_bindings;
  value_array_t* reordered_bindings = make_value_array(bindings->length);
  for (int i = 0; i < bindings->length; i++) {
    symbol_table_binding_t* binding
        = cast(symbol_table_binding_t*, value_array_get(bindings, i).ptr);
    reorder_symbol_table_structures_process_binding(symbol_table, binding,
                                                    reordered_bindings);
  }
  symbol_table->structures->ordered_bindings = reordered_bindings;
}
