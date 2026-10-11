void handle_extended_enums(symbol_table_t* symbol_table) {
  for (int i = 0; i < symbol_table->enums->ordered_bindings->length; i++) {
    symbol_table_binding_t* binding
        = value_array_get_ptr(symbol_table->enums->ordered_bindings, i,
                              typeof(symbol_table_binding_t*));
    for (int k = 0; k < binding->definition_nodes->length; k++) {
      enum_node_t* enum_node = to_enum_node(value_array_get_ptr(
          binding->definition_nodes, k, typeof(parse_node_t*)));
      if (token_matches(enum_node->keyword, "extend_enum")) {
        log_warn("extend_enum node found!");
        char* enum_node_name = token_to_string(enum_node->name);
        enum_node_t* base_enum_node
            = find_enum_node(symbol_table, enum_node_name);
        if (base_enum_node == nullptr) {
          log_fatal("Can't find base enum for %s", enum_node_name);
          exit(1);
        }
        int len = node_list_length(enum_node->elements);
        if (len == 0) {
          log_fatal("No elements in extend_enum node %s",
                    token_to_string(enum_node->name));
          exit(1);
        } else {
          log_warn("enum_node has %d elements", len);
        }
        for (int j = 0; j < node_list_length(enum_node->elements); j++) {
          log_warn("Index j = %d", j);
          parse_node_t* jth = node_list_get(enum_node->elements, j);
          log_warn("jth is found.");
          enum_element_t* element
              = to_enum_element_node(node_list_get(enum_node->elements, j));
          log_warn("HERE #1");
          node_list_add_node(&base_enum_node->elements, to_node(element));
          log_warn("HERE #5");
        }
        log_warn("HERE #6");
        value_array_delete_at(binding->definition_nodes, k);
        log_warn("HERE #7");
        k -= 1;
      }
    }
  }
}

enum_node_t* find_enum_node(symbol_table_t* symbol_table, char* name) {

  for (int i = 0; i < symbol_table->typedefs->ordered_bindings->length; i++) {
    symbol_table_binding_t* binding
        = value_array_get_ptr(symbol_table->typedefs->ordered_bindings, i,
                              typeof(symbol_table_binding_t*));
    typedef_node_t* typedef_node = to_typedef_node(value_array_get_ptr(
        binding->definition_nodes, 0, typeof(parse_node_t*)));
    if (is_enum_node(typedef_node->type_node->user_type)) {
      enum_node_t* enum_node = to_enum_node(typedef_node->type_node->user_type);
      if (token_matches(typedef_node->name, name)) {
        log_warn("Found a typedef node named %s", name);
        return enum_node;
      }
    }
  }

  for (int i = 0; i < symbol_table->enums->ordered_bindings->length; i++) {
    symbol_table_binding_t* binding
        = value_array_get_ptr(symbol_table->enums->ordered_bindings, i,
                              typeof(symbol_table_binding_t*));
    enum_node_t* enum_node = to_enum_node(value_array_get_ptr(
        binding->definition_nodes, 0, typeof(parse_node_t*)));
    if (token_matches(enum_node->keyword, "enum")
        && token_matches(enum_node->name, name)) {
      return enum_node;
    }
  }
  return nullptr;
}
