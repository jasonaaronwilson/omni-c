/**
 * @file main.c
 *
 * "main.c" acts as the entry point to the omni-c transpiler. It
 * registers flags for the command line processor library, calls that
 * library to parse the command line, and then dispatch to the
 * top-level entry points for each "git style" sub-command
 * (generate-header-file, generate-library, etc.)
 */

/* ====================================================================== */
// Here are is the combined set of flags we parse.
/* ====================================================================== */

value_array_t* FLAG_files = NULL;
char* FLAG_command = NULL;
boolean_t FLAG_print_command_line = true;
boolean_t FLAG_include_unnamed_nodes = false;
boolean_t FLAG_print_tokens_show_tokens = false;
boolean_t FLAG_print_tokens_include_whitespace = false;
boolean_t FLAG_print_tokens_include_comments = false;
boolean_t FLAG_print_tokens_parse_and_print = true;
boolean_t FLAG_print_tokens_show_appended_tokens = true;
char* FLAG_c_output_file = NULL;
char* FLAG_binary_output_file = NULL;
boolean_t FLAG_generate_enum_convertors = true;
char* FLAG_expression = NULL;
char* FLAG_statement = NULL;
boolean_t FLAG_dump_symbol_table = false;
// TODO(jawilson): make this the default or even remove?
boolean_t FLAG_use_statement_parser = false;
boolean_t FLAG_to_c = false;
boolean_t FLAG_omit_c_armyknife_include = false;
char* FLAG_c_compiler = "clang";

void do_print_tokens(value_array_t* tokens, char* message) {
  if (FLAG_print_tokens_show_tokens) {
    // Experimentally, tokens when debug printed are pretty big no
    // matter the size of the actual token and some tokens like
    // comments can be quite large.
    buffer_t* buffer = make_buffer(tokens->length * 60);
    for (int j = 0; j < tokens->length; j++) {
      token_t* token = token_at(tokens, j);
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
    file_t* file = value_array_get_ptr(files, i, typeof(file_t*));

    fprintf(stdout, "====================================================\n");
    fprintf(stdout, "====> Processing %s\n", file->file_name);
    fprintf(stdout, "====================================================\n");

    tokenizer_result_t tokenizer_result = tokenize(file->data);

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
        tokens,
        compound_literal(
            token_transformer_options_t,
            {
                .keep_whitespace = FLAG_print_tokens_include_whitespace,
                .keep_comments = FLAG_print_tokens_include_comments,
                .keep_javadoc_comments = FLAG_print_tokens_include_comments,
                .keep_c_preprocessor_lines = false,
            }));

    do_print_tokens(tokens, "after xform tokens");

    if (FLAG_print_tokens_parse_and_print) {
      pstate_t pstate = compound_literal(
          pstate_t, {.tokens = tokens,
                     .use_statement_parser = FLAG_use_statement_parser});
      ;
      if (!parse_declarations(&pstate)) {
        pstate.error.file_name = file->file_name;
        buffer_t* buffer = make_buffer(1);
        buffer = buffer_append_human_readable_error(buffer, &(pstate.error));
        log_fatal("%s", buffer_to_c_string(buffer));
        fatal_error(ERROR_ILLEGAL_INPUT);
      } else {
        buffer_t* buffer = make_buffer(1024);
        buffer_append_dbg_parse_node(make_cdl_printer(buffer),
                                     pstate_get_result_node(&pstate));
        fprintf(stdout, "** Parse Nodes %s **\n%s\n", file->file_name,
                buffer_to_c_string(buffer));
      }
    }
  }
}

void configure_flags(void) {
  flag_program_name("omni-c");
  flag_description(
      "omni-c is a transpiler for the omni-c language as well as a code "
      "generation tool for ISO C.");

  // I don't think these flags are truly global...
  flag_boolean("--print-command-line", &FLAG_print_command_line);
  flag_boolean("--use-statement-parser", &FLAG_use_statement_parser);

  configure_regular_commands();

  configure_print_tokens_command();
  configure_parse_expression();
  configure_parse_statement();
}

void configure_parse_expression(void) {
  flag_command("parse-expression", &FLAG_command);
  flag_string("--expression", &FLAG_expression);
  flag_boolean("--to-c", &FLAG_to_c);
}

void configure_parse_statement(void) {
  flag_command("parse-statement", &FLAG_command);
  flag_string("--statement", &FLAG_statement);
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

void configure_regular_commands(void) {
  flag_command("generate-header-file", &FLAG_command);
  flag_string("--c-output-file", &FLAG_c_output_file);
  flag_boolean("--generate-enum-convertors", &FLAG_generate_enum_convertors);
  flag_boolean("--dump-symbol-table", &FLAG_dump_symbol_table);
  flag_boolean("--use-statement-parser", &FLAG_use_statement_parser);
  flag_boolean("--omit-c-armyknife-include", &FLAG_omit_c_armyknife_include);

  flag_file_args(&FLAG_files);

  flag_command("generate-library", &FLAG_command);
  flag_string("--c-output-file", &FLAG_c_output_file);
  flag_boolean("--generate-enum-convertors", &FLAG_generate_enum_convertors);
  flag_boolean("--dump-symbol-table", &FLAG_dump_symbol_table);
  flag_boolean("--use-statement-parser", &FLAG_use_statement_parser);
  flag_boolean("--omit-c-armyknife-include", &FLAG_omit_c_armyknife_include);
  flag_file_args(&FLAG_files);

  flag_command("build", &FLAG_command);
  flag_string("--c-output-file", &FLAG_c_output_file);
  flag_string("--binary-output-file", &FLAG_binary_output_file);
  flag_boolean("--generate-enum-convertors", &FLAG_generate_enum_convertors);
  flag_boolean("--dump-symbol-table", &FLAG_dump_symbol_table);
  flag_boolean("--use-statement-parser", &FLAG_use_statement_parser);
  flag_boolean("--omit-c-armyknife-include", &FLAG_omit_c_armyknife_include);
  flag_string("--c-compiler", &FLAG_c_compiler);
  flag_file_args(&FLAG_files);
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
    buffer_append_dgb_symbol_table(buffer, symbol_table);

    fprintf(stderr, "%s", buffer_to_c_string(buffer));
  }
}

buffer_t* get_reflection_header_buffer(void);

char* include_node_to_string(cpp_include_node_t* node) {
  buffer_t* buffer = make_buffer(32);
  printer_t* printer = make_printer(buffer, 2);
  append_cpp_include_node(printer, node);
  char* include_statement = buffer_to_c_string(buffer);
  // free buffer and printer...
  return include_statement;
}

/**
 * @function generate_c_output_file
 *
 * This is effectively the "main" routine of the Omni C to C
 * transpiler.
 */
void generate_c_output_file(boolean_t is_library,
                            buffer_t* command_line_overview_comment) {

  boolean_t is_header_file = !is_library;

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
  printer_t* printer = make_printer(buffer, 2);

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
    if (struct_node == NULL) {
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
        is_library);
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

  boolean_t append_newline_after_functions = false;
  if (is_library) {
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
            && function_node->body != NULL) {
          append_newline_after_functions = true;
          if (false) {
            buffer_printf(buffer, "/* i=%d j=%d */\n", i, j);
          }
          append_c_function_node_and_body(printer, function_node);
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

  buffer_append_buffer(buffer, command_line_overview_comment);

  if (FLAG_c_output_file == NULL) {
    fprintf(stdout, "%s\n", buffer_to_c_string(buffer));
  } else {
    log_info("Attempting to write buffer to %s", FLAG_c_output_file);
    buffer_write_file(buffer, FLAG_c_output_file);
  }
}

void parse_expression_string_and_print_parse_tree(char* expression) {
  if (expression == NULL) {
    log_fatal("Expression not specified!");
    fatal_error(ERROR_ILLEGAL_INPUT);
  }
  tokenizer_result_t tokenizer_result
      = tokenize(buffer_append_string(make_buffer(1), expression));
  if (tokenizer_result.tokenizer_error_code) {
    fatal_error(ERROR_ILLEGAL_INPUT);
  }
  value_array_t* tokens = tokenizer_result.tokens;
  tokens = transform_tokens(
      tokens, compound_literal(token_transformer_options_t,
                               {
                                   .keep_whitespace = false,
                                   .keep_comments = false,
                                   .keep_javadoc_comments = false,
                                   .keep_c_preprocessor_lines = false,
                               }));

  pstate_t pstate = compound_literal(pstate_t, {0});
  pstate.use_statement_parser = true;
  pstate.tokens = tokens;
  if (!parse_expression(&pstate)) {
    fprintf(stderr, "FAIL\n");
    exit(1);
  }
  parse_node_t* node = pstate_get_result_node(&pstate);
  if (!pstate_expect_token_string(&pstate, ";")) {
    fprintf(stderr, "FAIL (expected ';')\n");
    exit(1);
  }
  buffer_t* output = make_buffer(1);
  buffer_append_dbg_parse_node(make_cdl_printer(output), node);
  if (FLAG_to_c) {
    buffer_append_string(output, "\n// C Output\n");
    printer_t* printer = make_printer(output, 2);
    append_parse_node(printer, node);
  }
  fprintf(stdout, "%s\n", buffer_to_c_string(output));
}

void parse_statement_string_and_print_parse_tree(char* expression) {
  tokenizer_result_t tokenizer_result
      = tokenize(buffer_append_string(make_buffer(1), expression));
  if (tokenizer_result.tokenizer_error_code) {
    fatal_error(ERROR_ILLEGAL_INPUT);
  }
  value_array_t* tokens = tokenizer_result.tokens;
  tokens = transform_tokens(
      tokens, compound_literal(token_transformer_options_t,
                               {
                                   .keep_whitespace = false,
                                   .keep_comments = false,
                                   .keep_javadoc_comments = false,
                                   .keep_c_preprocessor_lines = false,
                               }));
  pstate_t state = compound_literal(pstate_t, {0});
  state.use_statement_parser = true;
  state.tokens = tokens;
  pstatus_t status = parse_statement(&state);
  if (!status) {
    fprintf(stderr, "FAIL");
    exit(1);
  }
  parse_node_t* node = pstate_get_result_node(&state);
  buffer_t* output = make_buffer(1);
  buffer_append_dbg_parse_node(make_cdl_printer(output), node);
  buffer_append_string(output, "\n// C Output\n");
  printer_t* printer = make_printer(output, 2);
  append_parse_node(printer, node);
  fprintf(stdout, "%s\n", buffer_to_c_string(output));
}

/**
 * @function git_hash_object
 *
 * Hash the contents of a source file the way git would do it so that
 * `git cat-file -p HASH` will generally find the appropriate source
 * file contents. There should be other benefits like looking through
 * logs to find the exact file using these hashes.
 *
 * This may seem a bit like "signing", and while it may be a step in
 * that direction, it shouldn't been seen as so. There may be
 * interesting forensic steps could be done, based only on this data
 * and "official" repositories, that could help trace back issues.
 *
 * For files that come from other repositories (our builds depend on
 * c-armyknife-lib which IS in a different repo), I'm not sure what
 * will happen attempting to use these hashes. git hash-object seems
 * to be willing to hash anything even if not under git control and it
 * *probably* defaults (or always) uses the hash algorithm of the repo
 * it sees as "cwd" rather than the file it comes from though I could
 * be very wrong.
 */
buffer_t* git_hash_object(char* filename) {
  value_array_t* argv = make_value_array(2);
  value_array_add(argv, str_to_value("git"));
  value_array_add(argv, str_to_value("hash-object"));
  value_array_add(argv, str_to_value(filename));

  sub_process_t* sub_process = make_sub_process(argv);
  sub_process_launch(sub_process);

  buffer_t* buffer = make_buffer(1);
  do {
    sub_process_read(sub_process, buffer, NULL);
    usleep(5);
  } while (is_sub_process_running(sub_process));
  sub_process_read(sub_process, buffer, NULL);
  sub_process_wait(sub_process);

  return buffer;
}

/**
 * @function command_line_args_to_buffer
 *
 * The basic idea is to capture enough information to be able to
 * reproduce builds for both debugging purposes and to show that
 * omni-c-stable.c didn't come out of thin air. Folks might want to
 * look "forensically backwards", and this could help a bit.
 */
buffer_t* command_line_args_to_buffer(int argc, char** argv) {
  buffer_t* output = make_buffer(argc * 5);

  buffer_printf(output, "// Full Compiler Command Line:\n//\n");
  for (int i = 0; i < argc; i++) {
    buffer_printf(output, "//%s%s\n", i > 0 ? "    " : " ", argv[i]);
  }

  buffer_append_string(output, "\n");
  buffer_append_string(
      output,
      "// These checksums are currently easy to fake for example by using a\n");
  buffer_append_string(
      output, "// hacked git in the PATH at the time this compile was run.\n");
  buffer_append_string(output, "//\n");

  for (int i = 0; i < FLAG_files->length; i++) {
    char* filename = value_array_get(FLAG_files, i).str;
    // Strip the trailing newline!
    buffer_t* git_hash = git_hash_object(filename);
    buffer_replace_all(git_hash, "\n", "");
    buffer_append_string(output, "// git cat-file -p ");
    buffer_append_buffer(output, git_hash);
    buffer_printf(output, " > %s\n", filename);
  }

  return output;
}

value_array_t* c_compiler_command_line(char* input_file, char* output_file) {
  if (string_equal("clang", FLAG_c_compiler)
      || string_equal("gcc", FLAG_c_compiler)
      || string_equal("tcc", FLAG_c_compiler)) {
    value_array_t* argv = make_value_array(2);
    value_array_add(argv, str_to_value(FLAG_c_compiler));
    value_array_add(argv, str_to_value("-g"));
    value_array_add(argv, str_to_value("-rdynamic"));
    value_array_add(argv, str_to_value("-O3"));
    value_array_add(argv, str_to_value("-std=gnu99"));
    value_array_add(argv, str_to_value("-o"));
    value_array_add(argv, str_to_value(output_file));
    value_array_add(argv, str_to_value(input_file));
    value_array_add(argv, str_to_value("-lgc"));
    return argv;
  } else {
    log_fatal("Unknown C compiler %s\n", FLAG_c_compiler);
    fatal_error(ERROR_ILLEGAL_INPUT);
  }
}

int invoke_c_compiler(char* input_file, char* output_file) {
  value_array_t* argv = c_compiler_command_line(input_file, output_file);

  log_warn("Invoking C compiler with these arguments: %s",
           buffer_to_c_string(join_array_of_strings(argv, " ")));
  sub_process_t* sub_process = make_sub_process(argv);
  sub_process_launch(sub_process);

  buffer_t* buffer = make_buffer(1);
  do {
    sub_process_read(sub_process, buffer, buffer);
    usleep(5);
  } while (is_sub_process_running(sub_process));
  sub_process_read(sub_process, buffer, buffer);
  sub_process_wait(sub_process);

  log_warn(">>> Exit Status %d <<<\n%s", sub_process->exit_code,
           buffer_to_c_string(buffer));

  return sub_process->exit_code;
}

int main(int argc, char** argv) {
  configure_fatal_errors(
      compound_literal(fatal_error_config_t, {
                                                 .catch_sigsegv = true,
                                             }));
  logger_init();

  configure_flags();

  char* error = flag_parse_command_line(argc, argv);
  if (error) {
    flag_print_help(stderr, error);
    exit(1);
  }

  if (FLAG_print_command_line) {
    fprintf(stderr, "Command Line:");
    for (int i = 0; i < argc; i++) {
      fprintf(stderr, " %s", argv[i]);
    }
    fprintf(stderr, "\n");
  }

  if (FLAG_command == NULL) {
    // Technically this should not be reached because once a command
    // is defined, a missing or wrong command should trigger an error
    // and caught above. Note sure why this is still happening but
    // paranoid code can be easier to debug.
    fatal_error(ERROR_BAD_COMMAND_LINE);
  } else if (string_equal("generate-header-file", FLAG_command)) {
    generate_c_output_file(false, command_line_args_to_buffer(argc, argv));
    log_info("Exiting normally.");
    exit(0);
  } else if (string_equal("generate-library", FLAG_command)) {
    generate_c_output_file(true, command_line_args_to_buffer(argc, argv));
    log_info("Exiting normally.");
    exit(0);
  } else if (string_equal("build", FLAG_command)) {
    generate_c_output_file(true, command_line_args_to_buffer(argc, argv));
    int status = invoke_c_compiler(FLAG_c_output_file, FLAG_binary_output_file);
    if (status == 0) {
      log_info("Exiting normally.");
      exit(0);
    } else {
      log_warn("Exiting abnormally.");
      exit(status);
    }
  } else if (string_equal("parse-expression", FLAG_command)) {
    parse_expression_string_and_print_parse_tree(FLAG_expression);
  } else if (string_equal("parse-statement", FLAG_command)) {
    parse_statement_string_and_print_parse_tree(FLAG_statement);
  } else if (string_equal("print-tokens", FLAG_command)) {
    print_tokens();
  } else {
    fprintf(stderr, "Unknown command: %s\n", FLAG_command);
  }

  exit(0);
}
