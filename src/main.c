/**
 * @file main.c
 *
 * This is the main routine for the omni-c C header-file extractor,
 * the eventual omni-c language transpiler, and other tools helpful to
 * C and omni-c users.
 */

#include <stdio.h>
#include <stdlib.h>

#include "c-file-printer.h"
#include "debug-printer.h"
#include "file-reader.h"
#include "lexer.h"
#include "parser.h"
#include "source-to-source.h"
#include "srcgen.h"
#include "symbol-table-builder.h"
#include "symbol-table.h"
#include "token-transformer.h"

#include <c-armyknife-lib.h>

#include "header-file-extractor.c.generated.h"
#include "main.c.generated.h"

value_array_t* FLAG_files = NULL;
char* FLAG_command = NULL;
boolean_t FLAG_include_unnamed_nodes = false;
boolean_t FLAG_print_tokens_show_tokens = false;
boolean_t FLAG_print_tokens_include_whitespace = false;
boolean_t FLAG_print_tokens_include_comments = false;
boolean_t FLAG_print_tokens_parse_and_print = true;
boolean_t FLAG_print_tokens_show_appended_tokens = true;
char* FLAG_ouput_file = NULL;
boolean_t FLAG_generate_enum_convertors = true;

void do_print_tokens(value_array_t* tokens, char* message) {
  if (FLAG_print_tokens_show_tokens) {
    // Experimentally, tokens when debug printed are pretty big no
    // matter the size of the actual token and some tokens like
    // comments can be quite large.
    buffer_t* buffer = make_buffer(tokens->length * 60);
    for (int j = 0; j < tokens->length; j++) {
      oc_token_t* token = token_at(tokens, j);
      buffer = append_token_debug_string(buffer, *token);
      buffer = buffer_append_string(buffer, "\n");
    }
    fprintf(stdout, "** %s **\n%s\n", message, buffer_to_c_string(buffer));
  }
}

void print_tokens(void) {
  log_info("print_tokens()");

  value_array_t* files = read_files(FLAG_files);
  for (int i = 0; i < FLAG_files->length; i++) {
    oc_file_t* file = value_array_get_ptr(files, i, oc_file_t*);

    fprintf(stdout, "====================================================\n");
    fprintf(stdout, "====> Processing %s\n", file->file_name);
    fprintf(stdout, "====================================================\n");

    oc_tokenizer_result_t tokenizer_result = tokenize(file->data);

    if (tokenizer_result.tokenizer_error_code) {
      log_warn("Tokenizer error: \"%s\"::%d -- %d",
               value_array_get(FLAG_files, i).str,
               tokenizer_result.tokenizer_error_position,
               tokenizer_result.tokenizer_error_code);
      continue;
    }

    value_array_t* tokens = tokenizer_result.tokens;

    convert_nullptr_to_null(tokens);

    if (FLAG_print_tokens_show_appended_tokens) {
      buffer_t* appended_tokens = make_buffer(1);
      debug_append_tokens(appended_tokens, tokens);
      fprintf(stdout, "%s", buffer_to_c_string(appended_tokens));
    }

    do_print_tokens(tokens, "before xform tokens");

    tokens = transform_tokens(
        tokens, (token_transformer_options_t){
                    .keep_whitespace = FLAG_print_tokens_include_whitespace,
                    .keep_comments = FLAG_print_tokens_include_comments,
                    .keep_javadoc_comments = FLAG_print_tokens_include_comments,
                    .keep_c_preprocessor_lines = false,
                });

    do_print_tokens(tokens, "after xform tokens");

    if (FLAG_print_tokens_parse_and_print) {
      parse_result_t declarations = parse_declarations(tokens, 0);
      if (is_error_result(declarations)) {
        declarations.parse_error.file_name = file->file_name;
        buffer_t* buffer = make_buffer(1);
        buffer = buffer_append_human_readable_error(
            buffer, &(declarations.parse_error));
        log_fatal("%s", buffer_to_c_string(buffer));
        fatal_error(ERROR_ILLEGAL_STATE);
      } else {
        buffer_t* buffer = make_buffer(1024);
        buffer = buffer_append_dbg_parse_node(buffer, declarations.node, 0);
        fprintf(stdout, "** Parse Nodes %s **\n%s\n", file->file_name,
                buffer_to_c_string(buffer));
      }
    }
  }
}

boolean_t FLAG_unique_prototype_header_files = false;

void extract_command(char* command) {
  log_info("extract_prototypes(%s)", command);

  buffer_t* prototype_outputs = make_buffer(16 * 1024);
  buffer_t* enum_outputs = make_buffer(16 * 1024);

  value_array_t* files = read_files(FLAG_files);
  for (int i = 0; i < FLAG_files->length; i++) {
    if (FLAG_unique_prototype_header_files) {
      buffer_clear(prototype_outputs);
    }

    oc_file_t* file = value_array_get_ptr(files, i, oc_file_t*);
    prototype_outputs
        = buffer_printf(prototype_outputs,
                        "/* Automatically extracted prototypes from %s */\n\n",
                        file->file_name);

    fprintf(stdout, "====================================================\n");
    fprintf(stdout, "====> Processing %s\n", file->file_name);
    fprintf(stdout, "====================================================\n");

    oc_tokenizer_result_t tokenizer_result = tokenize(file->data);

    if (tokenizer_result.tokenizer_error_code) {
      log_warn("Tokenizer error: \"%s\"::%d -- %d",
               value_array_get(FLAG_files, i).str,
               tokenizer_result.tokenizer_error_position,
               tokenizer_result.tokenizer_error_code);
      fatal_error(ERROR_ILLEGAL_INPUT);
    }

    value_array_t* tokens = tokenizer_result.tokens;

    symbol_table_t* symbol_table = make_symbol_table();

    handle_c_preprocessor_directives(
        (c_preprocess_options_t){
            .keep_system_includes = false,
            .keep_user_includes = false,
        },
        symbol_table, tokens);

    tokens = transform_tokens(tokens, (token_transformer_options_t){
                                          .keep_whitespace = false,
                                          .keep_comments = false,
                                          .keep_javadoc_comments = false,
                                          .keep_c_preprocessor_lines = false,
                                      });

    parse_result_t declarations_result = parse_declarations(tokens, 0);
    if (is_error_result(declarations_result)) {
      declarations_result.parse_error.file_name = file->file_name;
      buffer_t* buffer = make_buffer(1);
      buffer = buffer_append_human_readable_error(
          buffer, &(declarations_result.parse_error));
      log_fatal("%s", buffer_to_c_string(buffer));
      fatal_error(ERROR_ILLEGAL_INPUT);
    }

    declarations_node_t* root = to_declarations_node(declarations_result.node);

    if (string_equal("extract-prototypes", command)) {
      prototype_outputs
          = extract_prototypes_process_declarations(prototype_outputs, root);
    } else if (string_equal("extract-enums", command)) {
      enum_outputs = extract_enums_process_declarations(enum_outputs, root);
    }

    if (FLAG_unique_prototype_header_files) {
      if (string_equal("extract-prototypes", command)) {
        char* prototype_outputs_file_name
            = string_printf("%s.generated.h", file->file_name);
        buffer_write_file(prototype_outputs, prototype_outputs_file_name);
        free_bytes(prototype_outputs_file_name);
      } else if (string_equal("extract-enums", command)) {
        char* enum_outputs_file_name
            = string_printf("%s.enum-generated.h", file->file_name);
        buffer_write_file(enum_outputs, enum_outputs_file_name);
        free_bytes(enum_outputs_file_name);
      }
    }
  }

  if (!FLAG_unique_prototype_header_files) {
    // TODO(jawilson): prototype_outputs to --prototype_outputs...
    fprintf(stdout, "%s", buffer_to_c_string(prototype_outputs));
  }
}

void configure_flags(void) {
  flag_program_name("omni-c");
  flag_description(
      "omni-c is a transpiler for the omni-c language as well as a code "
      "generation tool for ISO C.");

  configure_print_tokens_command();
  configure_extract_prototypes_command();
  configure_extract_enums_command();
  configure_test_symbol_table_command();
  configure_generate_c_output_file();
}

void configure_print_tokens_command(void) {
  flag_command("print-tokens", &FLAG_command);
  flag_boolean("--show-tokens", &FLAG_print_tokens_show_tokens);
  flag_boolean("--include-whitespace", &FLAG_print_tokens_include_whitespace);
  flag_boolean("--include-comments", &FLAG_print_tokens_include_comments);
  flag_boolean("--parse-and-print", &FLAG_print_tokens_parse_and_print);
  flag_boolean("--show-appended-tokens",
               &FLAG_print_tokens_show_appended_tokens);
  flag_file_args(&FLAG_files);
}

void configure_extract_enums_command(void) {
  flag_command("extract-enums", &FLAG_command);
  flag_string("--output-file", &FLAG_ouput_file);
  flag_boolean("--unique-prototype-header-files",
               &FLAG_unique_prototype_header_files);
  flag_file_args(&FLAG_files);
}

void configure_extract_prototypes_command(void) {
  flag_command("extract-prototypes", &FLAG_command);
  flag_string("--output-file", &FLAG_ouput_file);
  flag_boolean("--unique-prototype-header-files",
               &FLAG_unique_prototype_header_files);
  flag_file_args(&FLAG_files);
}

void configure_test_symbol_table_command(void) {
  flag_command("test-symbol-table", &FLAG_command);
  flag_file_args(&FLAG_files);
}

void configure_generate_c_output_file(void) {
  flag_command("generate-header-file", &FLAG_command);
  flag_string("--output-file", &FLAG_ouput_file);
  flag_boolean("--generate_enum_convertors", &FLAG_generate_enum_convertors);
  flag_file_args(&FLAG_files);

  flag_command("generate-library", &FLAG_command);
  flag_string("--output-file", &FLAG_ouput_file);
  flag_boolean("--generate_enum_convertors", &FLAG_generate_enum_convertors);
  flag_file_args(&FLAG_files);
}

boolean_t is_inlined_function(function_node_t* node) {
  return token_matches(node->storage_class_specifier, "static")
         && token_matches(node->function_specifier, "inline");
}

/**
 * @function generate_c_output_file
 */
void generate_c_output_file(boolean_t is_library) {

  boolean_t is_header_file = !is_library;

  symbol_table_t* symbol_table = make_symbol_table();
  parse_and_add_top_level_definitions(symbol_table, FLAG_files);
  if (FLAG_generate_enum_convertors) {
    srcgen_enum_to_string_converters(symbol_table);
  }
  split_structure_typedefs(symbol_table);
  reorder_symbol_table_typedefs(symbol_table);
  reorder_symbol_table_structures(symbol_table);
  buffer_t* buffer = make_buffer(1024 * 8);

  char* guard_name = "_HEADER_FILE_GUARD_";

  if (is_header_file) {
    buffer_printf(buffer, "#ifndef %s\n#define %s\n\n", guard_name, guard_name);
  }

  boolean_t append_newline_after_system_includes = false;
  buffer_append_string(buffer, "// ========== system includes ==========\n\n");
  string_hashtable_t* system_includes_set = make_string_hashtable(19);
  for (uint64_t i = 0; i < symbol_table->system_includes->length; i++) {
    append_newline_after_system_includes = true;
    cpp_include_node_t* node = value_array_get_ptr(
        symbol_table->system_includes, i, cpp_include_node_t*);
    char* include_statement = buffer_to_c_string(
        buffer_append_cpp_include_node(make_buffer(32), node));
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
    cpp_define_node_t* node
        = value_array_get_ptr(symbol_table->defines, i, cpp_define_node_t*);
    buffer_append_cpp_define_node(buffer, node);
    buffer_append_string(buffer, "\n");
  }

  buffer_append_string(buffer, "// ========== enums ==========\n\n");

  for (int i = 0; i < symbol_table->enums->ordered_bindings->length; i++) {
    symbol_table_binding_t* binding = value_array_get_ptr(
        symbol_table->enums->ordered_bindings, i, symbol_table_binding_t*);
    enum_node_t* enum_node = to_enum_node(
        value_array_get_ptr(binding->definition_nodes, 0, parse_node_t*));
    buffer_append_enum_node(buffer, enum_node);
    buffer_append_string(buffer, ";\n\n");
  }

  buffer_append_string(buffer, "// ========== typedefs ==========\n\n");
  for (int i = 0; i < symbol_table->typedefs->ordered_bindings->length; i++) {
    symbol_table_binding_t* binding = value_array_get_ptr(
        symbol_table->typedefs->ordered_bindings, i, symbol_table_binding_t*);
    typedef_node_t* typedef_node = to_typedef_node(
        cast(parse_node_t*, value_array_get(binding->definition_nodes, 0).ptr));
    buffer_append_typedef_node(buffer, typedef_node);
    buffer_append_string(buffer, "\n");
  }

  buffer_append_string(buffer, "// ========== stuctures/unions ==========\n\n");
  for (int i = 0; i < symbol_table->structures->ordered_bindings->length; i++) {
    symbol_table_binding_t* binding = value_array_get_ptr(
        symbol_table->structures->ordered_bindings, i, symbol_table_binding_t*);
    struct_node_t* struct_node = get_full_structure_definition_node(binding);
    buffer_append_struct_node(buffer, struct_node);
    buffer_append_string(buffer, ";\n\n");
  }

  boolean_t append_newline_after_variables = false;
  buffer_append_string(buffer, "// ========== global variables ==========\n\n");
  for (int i = 0; i < symbol_table->variables->ordered_bindings->length; i++) {
    symbol_table_binding_t* binding = value_array_get_ptr(
        symbol_table->variables->ordered_bindings, i, symbol_table_binding_t*);
    buffer_append_global_variable_node(
        buffer,
        value_array_get_ptr(binding->definition_nodes, 0,
                            global_variable_node_t*),
        is_library);
    buffer_append_string(buffer, "\n");
  }

  if (append_newline_after_variables) {
    buffer_append_string(buffer, "\n");
  }

  boolean_t append_newline_after_prototypes = false;
  buffer_append_string(buffer,
                       "// ========== function prototypes ==========\n\n");
  for (int i = 0; i < symbol_table->functions->ordered_bindings->length; i++) {
    symbol_table_binding_t* binding = value_array_get_ptr(
        symbol_table->functions->ordered_bindings, i, symbol_table_binding_t*);
    function_node_t* function_node = to_function_node(
        cast(parse_node_t*, value_array_get(binding->definition_nodes, 0).ptr));
    if (!is_inlined_function(function_node)) {
      append_newline_after_prototypes = true;
      buffer_append_c_function_node_prototype(buffer, function_node);
    }
  }
  if (append_newline_after_prototypes) {
    buffer_append_string(buffer, "\n");
  }

  boolean_t append_newline_after_inlines = false;
  buffer_append_string(buffer,
                       "// ========== inlined functions ==========\n\n");
  for (int i = 0; i < symbol_table->functions->ordered_bindings->length; i++) {
    symbol_table_binding_t* binding = value_array_get_ptr(
        symbol_table->functions->ordered_bindings, i, symbol_table_binding_t*);
    function_node_t* function_node = to_function_node(
        cast(parse_node_t*, value_array_get(binding->definition_nodes, 0).ptr));
    if (is_inlined_function(function_node)) {
      append_newline_after_inlines = true;
      buffer_append_c_function_node_and_body(buffer, function_node);
    }
  }

  if (append_newline_after_inlines) {
    buffer_append_string(buffer, "\n");
  }

  boolean_t append_newline_after_functions = false;
  if (is_library) {
    buffer_append_string(buffer, "// ========== functions ==========\n\n");
    for (int i = 0; i < symbol_table->functions->ordered_bindings->length;
         i++) {
      symbol_table_binding_t* binding
          = value_array_get_ptr(symbol_table->functions->ordered_bindings, i,
                                symbol_table_binding_t*);
      for (int j = 0; j < binding->definition_nodes->length; j++) {
        function_node_t* function_node = to_function_node(cast(
            parse_node_t*, value_array_get(binding->definition_nodes, j).ptr));
        if (!is_inlined_function(function_node)
            && function_node->body != NULL) {
          append_newline_after_functions = true;
          buffer_append_c_function_node_and_body(buffer, function_node);
        }
      }
    }
  }
  if (append_newline_after_functions) {
    buffer_append_string(buffer, "\n");
  }

  if (is_header_file) {
    buffer_printf(buffer, "\n#endif /* %s */\n", guard_name);
  }

  if (FLAG_ouput_file == NULL) {
    fprintf(stdout, "%s\n", buffer_to_c_string(buffer));
  } else {
    log_info("Attempting to write buffer to %s", FLAG_ouput_file);
    buffer_write_file(buffer, FLAG_ouput_file);
  }
}

int main(int argc, char** argv) {
  configure_fatal_errors((fatal_error_config_t){
      .catch_sigsegv = true,
  });
  logger_init();

  configure_flags();

  char* error = flag_parse_command_line(argc, argv);
  if (error) {
    flag_print_help(stderr, error);
    exit(1);
  }

  if (FLAG_command == NULL) {
    // Technically this should not be reached because once a command
    // is defined, a missing or wrong command should trigger an error
    // and caught above. Note sure why this is still happening but
    // paranoid code can be easier to debug.
    fatal_error(ERROR_BAD_COMMAND_LINE);
    ////   } else if (string_equal("build", FLAG_command)) {
    ////     translate_and_build();
    ////   } else if (string_equal("extract-header-file", FLAG_command)) {
    ////     extract_header_file();
    ////   } else if (string_equal("print-parse-trees", FLAG_command)) {
    ////     print_parse_trees();
  } else if (string_equal("generate-header-file", FLAG_command)) {
    generate_c_output_file(false);
    log_info("Exiting normally!");
    exit(0);
  } else if (string_equal("generate-library", FLAG_command)) {
    generate_c_output_file(true);
    log_info("Exiting normally!");
    exit(0);
  } else if (string_equal("print-tokens", FLAG_command)) {
    print_tokens();
  } else if (string_equal("extract-enums", FLAG_command)
             || string_equal("extract-prototypes", FLAG_command)) {
    extract_command(FLAG_command);
  } else if (string_equal("test-symbol-table", FLAG_command)) {
    symbol_table_t* symbol_table = make_symbol_table();
    parse_and_add_top_level_definitions(symbol_table, FLAG_files);

    buffer_t* buffer = make_buffer(128);
    buffer = symbol_table_stats(buffer, symbol_table);
    buffer_append_dgb_symbol_table(buffer, symbol_table);
    fprintf(stdout, "%s", buffer_to_c_string(buffer));

    split_structure_typedefs(symbol_table);
    buffer_clear(buffer);
    buffer = symbol_table_stats(buffer, symbol_table);
    buffer_append_dgb_symbol_table(buffer, symbol_table);
    fprintf(stdout, "%s", buffer_to_c_string(buffer));

    reorder_symbol_table_typedefs(symbol_table);
    buffer_clear(buffer);
    buffer = symbol_table_stats(buffer, symbol_table);
    buffer_append_dgb_symbol_table(buffer, symbol_table);
    fprintf(stdout, "%s", buffer_to_c_string(buffer));

    reorder_symbol_table_structures(symbol_table);
    buffer_clear(buffer);
    buffer = symbol_table_stats(buffer, symbol_table);
    buffer_append_dgb_symbol_table(buffer, symbol_table);
    fprintf(stdout, "%s", buffer_to_c_string(buffer));
  } else {
    fprintf(stderr, "Unknown command: %s\n", FLAG_command);
  }

  exit(0);
}
