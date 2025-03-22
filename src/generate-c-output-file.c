typedef output_file_type_t = enum {
  OUTPUT_TYPE_UNKNOWN,
  OUTPUT_TYPE_C_HEADER_FILE,
  OUTPUT_TYPE_C_LIBRARY_FILE,
  OUTPUT_TYPE_C_UNIT_TEST_FILE,
};

/**
 * @function generate_c_output_file
 *
 * This is effectively the real entery-point of the Omni C to C99
 * transpiler. It still references FLAG_ variables even though it
 * isn't just a simple "command".
 *
 * At some point I'd like to experiment with NON C output. This
 * large procedure doesn't help us in that goal.
 *
 * TODO(jawilson): use a struct to get rid of FLAG_ usage in this file
 * by taking in a struct OR better yet, first refactor into
 * self-standing logical parts and THEN refactor.
 */
void generate_c_output_file(output_file_type_t output_type,
                            buffer_t* command_line_overview_comment) {

  boolean_t is_header_file = output_type == OUTPUT_TYPE_C_HEADER_FILE;

  symbol_table_t* symbol_table = make_symbol_table();
  parse_and_add_top_level_definitions(symbol_table, FLAG_files,
                                      FLAG_use_statement_parser);
  dump_symbol_table("initial parse", symbol_table);
  if (FLAG_generate_enum_convertors) {
    srcgen_enum_to_string_converters(symbol_table);
    dump_symbol_table("enum to string generation", symbol_table);
  }
  split_structure_typedefs(symbol_table);
  dump_symbol_table("split structures", symbol_table);
  reorder_symbol_table_typedefs(symbol_table);
  dump_symbol_table("reorder typedefs", symbol_table);
  reorder_symbol_table_structures(symbol_table);
  dump_symbol_table("reorder structures", symbol_table);

  buffer_t* buffer = make_buffer(1024 * 8);
  printer_t* printer = make_printer(buffer, symbol_table, 2);

  add_generated_c_file_header(buffer);

  char* guard_name = "_HEADER_FILE_GUARD_";

  if (is_header_file) {
    buffer_printf(buffer, "#ifndef %s\n#define %s\n\n", guard_name, guard_name);
  }

  buffer_append_buffer(buffer, get_reflection_header_buffer());

  boolean_t append_newline_after_system_includes = false;
  buffer_append_string(buffer, "// ========== system includes ==========\n\n");
  string_hashtable_t* system_includes_set = make_string_hashtable(19);
  for (uint64_t i = 0; i < symbol_table->system_includes->length; i++) {
    append_newline_after_system_includes = true;
    cpp_include_node_t* node = value_array_get_ptr(
        symbol_table->system_includes, i, typeof(cpp_include_node_t*));
    char* include_statement = include_node_to_string(node);
    if (FLAG_omit_c_armyknife_include
        && string_starts_with(include_statement, "#include <c-armyknife-lib")) {
      continue;
    }
    if (!is_ok(string_ht_find(system_includes_set, include_statement))) {
      system_includes_set = string_ht_insert(
          system_includes_set, include_statement, boolean_to_value(true));
      buffer_append_string(buffer, include_statement);
    }
  }
  if (append_newline_after_system_includes) {
    buffer_append_string(buffer, "\n");
  }

  // result->user_includes

  buffer_append_string(buffer, "// ========== defines ==========\n\n");
  for (uint64_t i = 0; i < symbol_table->defines->length; i++) {
    cpp_define_node_t* node = value_array_get_ptr(symbol_table->defines, i,
                                                  typeof(cpp_define_node_t*));
    append_cpp_define_node(printer, node);
    append_string(printer, "\n");
  }

  buffer_append_string(buffer, "// ========== enums ==========\n\n");

  for (int i = 0; i < symbol_table->enums->ordered_bindings->length; i++) {
    symbol_table_binding_t* binding
        = value_array_get_ptr(symbol_table->enums->ordered_bindings, i,
                              typeof(symbol_table_binding_t*));
    enum_node_t* enum_node = to_enum_node(value_array_get_ptr(
        binding->definition_nodes, 0, typeof(parse_node_t*)));
    append_enum_node(printer, enum_node);
    append_string(printer, ";\n\n");
  }

  buffer_append_string(buffer, "// ========== typedefs ==========\n\n");
  for (int i = 0; i < symbol_table->typedefs->ordered_bindings->length; i++) {
    symbol_table_binding_t* binding
        = value_array_get_ptr(symbol_table->typedefs->ordered_bindings, i,
                              typeof(symbol_table_binding_t*));
    typedef_node_t* typedef_node = to_typedef_node(
        cast(parse_node_t*, value_array_get(binding->definition_nodes, 0).ptr));
    append_typedef_node(printer, typedef_node);
    append_string(printer, "\n");
  }

  buffer_append_string(buffer, "// ========== stuctures/unions ==========\n\n");
  for (int i = 0; i < symbol_table->structures->ordered_bindings->length; i++) {
    symbol_table_binding_t* binding
        = value_array_get_ptr(symbol_table->structures->ordered_bindings, i,
                              typeof(symbol_table_binding_t*));
    struct_node_t* struct_node = get_full_structure_definition_node(binding);
    if (struct_node == nullptr) {
      struct_node = value_array_get_ptr(binding->definition_nodes, 0,
                                        typeof(struct_node_t*));
    }
    append_struct_node(printer, struct_node);
    append_string(printer, ";\n\n");
  }

  boolean_t append_newline_after_variables = false;
  buffer_append_string(buffer, "// ========== global variables ==========\n\n");
  for (int i = 0; i < symbol_table->variables->ordered_bindings->length; i++) {
    symbol_table_binding_t* binding
        = value_array_get_ptr(symbol_table->variables->ordered_bindings, i,
                              typeof(symbol_table_binding_t*));
    append_variable_definition_node(
        printer,
        value_array_get_ptr(binding->definition_nodes, 0,
                            typeof(variable_definition_node_t*)),
        !is_header_file);
    append_string(printer, "\n");
  }

  if (append_newline_after_variables) {
    append_string(printer, "\n");
  }

  boolean_t append_newline_after_prototypes = false;
  buffer_append_string(buffer,
                       "// ========== function prototypes ==========\n\n");
  for (int i = 0; i < symbol_table->functions->ordered_bindings->length; i++) {
    symbol_table_binding_t* binding
        = value_array_get_ptr(symbol_table->functions->ordered_bindings, i,
                              typeof(symbol_table_binding_t*));
    function_node_t* function_node = to_function_node(
        cast(parse_node_t*, value_array_get(binding->definition_nodes, 0).ptr));
    if (!is_inlined_function(function_node)) {
      append_newline_after_prototypes = true;
      append_c_function_node_prototype(printer, function_node);
    }
  }
  if (append_newline_after_prototypes) {
    append_string(printer, "\n");
  }

  boolean_t append_newline_after_inlines = false;
  buffer_append_string(buffer,
                       "// ========== inlined functions ==========\n\n");
  for (int i = 0; i < symbol_table->functions->ordered_bindings->length; i++) {
    symbol_table_binding_t* binding
        = value_array_get_ptr(symbol_table->functions->ordered_bindings, i,
                              typeof(symbol_table_binding_t*));
    function_node_t* function_node = to_function_node(
        cast(parse_node_t*, value_array_get(binding->definition_nodes, 0).ptr));
    if (is_inlined_function(function_node)) {
      append_newline_after_inlines = true;
      append_c_function_node_and_body(printer, function_node);
    }
  }

  if (append_newline_after_inlines) {
    buffer_append_string(buffer, "\n");
  }

  /* ================================================================================
   */
  buffer_t* test_main_function_buffer = make_buffer(1);
  buffer_append_string(test_main_function_buffer,
                       "void do_start_test(char* fn_name) {\n    "
                       "fprintf(stderr, \"Testing %s...\\n\", fn_name);\n}\n");

  buffer_printf(test_main_function_buffer,
                "int main(int argc, char** argv) {\n");
  /* ================================================================================
   */

  boolean_t append_newline_after_functions = false;
  if (!is_header_file) {
    buffer_append_string(buffer, "// ========== functions ==========\n\n");
    for (int i = 0; i < symbol_table->functions->ordered_bindings->length;
         i++) {
      symbol_table_binding_t* binding
          = value_array_get_ptr(symbol_table->functions->ordered_bindings, i,
                                typeof(symbol_table_binding_t*));
      for (int j = 0; j < binding->definition_nodes->length; j++) {
        function_node_t* function_node = to_function_node(cast(
            parse_node_t*, value_array_get(binding->definition_nodes, j).ptr));
        if (!is_inlined_function(function_node)
            && function_node->body != nullptr) {
          append_newline_after_functions = true;
          append_c_function_node_and_body(printer, function_node);
          if (is_unit_test_function(function_node)) {
            char* fn_name = token_to_string(function_node->function_name);
            buffer_printf(test_main_function_buffer,
                          "    do_start_test(\"%s\");\n", fn_name);
            buffer_printf(test_main_function_buffer, "    %s();\n", fn_name);
          }
        }
      }
    }
  }
  if (append_newline_after_functions) {
    buffer_append_string(buffer, "\n");
  }
  buffer_printf(test_main_function_buffer, "    exit(0);\n");
  buffer_printf(test_main_function_buffer, "}\n\n");

  if (is_header_file) {
    buffer_printf(buffer, "\n#endif /* %s */\n", guard_name);
  }

  if (output_type == OUTPUT_TYPE_C_UNIT_TEST_FILE) {
    buffer_append_buffer(buffer, test_main_function_buffer);
  }

  buffer_append_buffer(buffer, command_line_overview_comment);

  if (FLAG_c_output_file == nullptr) {
    fprintf(stdout, "%s\n", buffer_to_c_string(buffer));
  } else {
    log_info("Attempting to write buffer to %s", FLAG_c_output_file);
    // make_writable_if_exists(FLAG_c_output_file);
    buffer_write_file(buffer, FLAG_c_output_file);
    // make_file_read_only(FLAG_c_output_file);
  }
}

// This could be improved by just looking at the signature of the
// function.
boolean_t is_unit_test_function(function_node_t* node) {
  char* function_name = token_to_string(node->function_name);
  return string_starts_with(function_name, "test_")
         && !string_equal(function_name, "test_fail_and_exit");
}

boolean_t is_inlined_function(function_node_t* node) {
  return token_matches(node->storage_class_specifier, "static")
         && token_list_contains(node->function_specifiers, "inline");
}

void dump_symbol_table(char* phase_name, symbol_table_t* symbol_table) {
  if (FLAG_dump_symbol_table) {
    fprintf(stderr,
            "=================================================================="
            "====\n");
    fprintf(stderr, "%s\n", phase_name);
    fprintf(stderr,
            "=================================================================="
            "====\n\n");
    buffer_t* buffer = make_buffer(128);
    buffer = symbol_table_stats(buffer, symbol_table);
    buffer_append_dgb_symbol_table(make_cdl_printer(buffer), symbol_table);

    fprintf(stderr, "%s", buffer_to_c_string(buffer));
  }
}

char* include_node_to_string(cpp_include_node_t* node) {
  buffer_t* buffer = make_buffer(32);
  printer_t* printer = make_printer(buffer, make_symbol_table(), 2);
  append_cpp_include_node(printer, node);
  char* include_statement = buffer_to_c_string(buffer);
  return include_statement;
}

void add_generated_c_file_header(buffer_t* buffer) {
  buffer_printf(buffer, "// -*- buffer-read-only: t -*-\n//\n");
  buffer_printf(
      buffer,
      "// This is a generated file, so you generally don't want to edit it!\n");
  buffer_printf(buffer,
                "// The bottom of the file has more information about it's "
                "creation.\n\n\n");
}
