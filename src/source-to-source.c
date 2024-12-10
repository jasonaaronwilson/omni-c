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

token_t* generate_struct_name_from_typedef_name(token_t* name) {
  token_t* generated = make_derived_token(name);
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

        log_debug("Splitting %s off from %s", token_to_string(node->name),
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
  log_debug("processing binding %s", binding->key_string);
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

    log_debug("adding binding %s at position %d", binding->key_string,
              reordered_bindings->length);
    // TODO(jawilson): actually try to process dependencies first!
    value_array_add(reordered_bindings, ptr_to_value(binding));
    binding->visited = true;
  }
}

// ======================================================================
// reorder_symbol_table_structures
// ======================================================================

struct_node_t*
    get_full_structure_definition_node(symbol_table_binding_t* binding) {
  for (uint64_t i = 0; i < binding->definition_nodes->length; i++) {
    parse_node_t* node = cast(
        parse_node_t*, value_array_get(binding->definition_nodes, i).ptr);
    struct_node_t* structure_node = to_struct_node(node);
    if (!structure_node->partial_definition) {
      return structure_node;
    }
  }
  return NULL;
}

symbol_table_binding_t*
    resolve_typename_to_structure_binding(symbol_table_t* symbol_table,
                                          type_node_t* type_node) {
  if (type_node->type_node_kind == TYPE_NODE_KIND_POINTER) {
    log_debug(
        "resolve_typename_to_structure_binding -- not looking through pointers "
        "%p",
        type_node);
    return NULL;
  }

  if (type_node->type_node_kind == TYPE_NODE_KIND_TYPE_EXPRESSION) {
    parse_node_t* user_type = type_node->user_type;
    if (is_struct_node(user_type)) {
      struct_node_t* struct_node = to_struct_node(user_type);
      if (struct_node->name != NULL) {
        char* key_name = token_to_string(struct_node->name);
        symbol_table_binding_t* binding
            = symbol_table_map_get(symbol_table->structures, key_name);
        char* dbg_binding = buffer_to_c_string(
            buffer_append_dgb_binding(make_buffer(10), binding));
        log_debug(
            "resolve_typename_to_structure_binding -- returning binding %p %s",
            binding, dbg_binding);
        return binding;
      }
    }
    // ignore enums, they are already processed...
    return NULL;
  }

  // TODO(jawilson): recurse on any ARRAY type.
  char* key_string = token_to_string(type_node->type_name);
  log_debug("resolve_typename_to_structure_binding -- %d %s", type_node->tag,
            key_string);
  symbol_table_binding_t* typedef_binding
      = symbol_table_map_get(symbol_table->typedefs, key_string);
  if (typedef_binding != NULL) {
    if (typedef_binding->definition_nodes->length != 1) {
      fatal_error(ERROR_ILLEGAL_STATE);
    }
    typedef_node_t* typedef_node = to_typedef_node(value_array_get_ptr(
        typedef_binding->definition_nodes, 0, typeof(parse_node_t*)));
    return resolve_typename_to_structure_binding(symbol_table,
                                                 typedef_node->type_node);
  } else {
    return symbol_table_map_get(symbol_table->structures, key_string);
  }
}

void reorder_symbol_table_structures_process_binding(
    symbol_table_t* symbol_table, symbol_table_binding_t* binding,
    value_array_t* reordered_bindings) {
  log_debug("processing %s", binding->key_string);
  if (!binding->visited) {
    binding->visited = true;
    struct_node_t* structure_node = get_full_structure_definition_node(binding);
    if (structure_node == NULL) {
      // We only have partial definitions which is perfectly fine as
      // long as we only ever use poitners to those nodes.
      return;
    }
    uint64_t length = node_list_length(structure_node->fields);
    for (uint64_t i = 0; i < length; i++) {
      field_node_t* field
          = to_field_node(node_list_get(structure_node->fields, i));
      type_node_t* type_node = field->type;
      if (type_node != NULL) {
        symbol_table_binding_t* field_type_binding
            = resolve_typename_to_structure_binding(symbol_table, type_node);
        if (field_type_binding != NULL && !field_type_binding->visited) {
          reorder_symbol_table_structures_process_binding(
              symbol_table, field_type_binding, reordered_bindings);
        }
      }
    }
    value_array_add(reordered_bindings, ptr_to_value(binding));
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

/**
 * @function convert_nullptr_to_null
 *
 * This function runs before parsing and simply replaces each instance
 * of "nullptr" to NULL so that the input can use nullptr and still
 * compile with c99 without worrying about header files, etc.
 *
 * TODO(jawilson): this isn't going to work right until we fully parse
 * without the shortcuts like function body parsing. One work around
 * is to print the tokens to a buffer and lex them again.
 */
void convert_nullptr_to_null(value_array_t* tokens) {
  // easy way to break-point... __asm("int3");
  buffer_t* null_token = buffer_append_string(make_buffer(1), "NULL");
  for (int i = 0; i < tokens->length; i++) {
    token_t* token = token_at(tokens, i);
    if (token_matches(token, "nullptr")) {
      token->start = 0;
      token->end = null_token->length;
      token->buffer = null_token;
    }
  }
}
