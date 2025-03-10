/**
 * @file main.c
 *
 * "main.c" acts as the entry point to the omni-c transpiler. It
 * registers flags for the command line processor library, calls that
 * library to parse the command line, and then dispatch to the
 * top-level entry points for each "git style" sub-command
 * (generate-header-file, generate-library, etc.)
 */

/**
 * @function main
 *
 * This is the main entry point to the omni-c compiler.
 */
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

  if (FLAG_command == nullptr) {
    // Technically this should not be reached because once a command
    // is defined, a missing or wrong command should trigger an error
    // and caught above. Note sure why this is still happening but
    // paranoid code can be easier to debug.
    fatal_error(ERROR_BAD_COMMAND_LINE);
  } else if (string_equal("archive", FLAG_command)) {
    archive_command();
  } else if (string_equal("generate-header-file", FLAG_command)) {
    generate_header_file_command(command_line_args_to_buffer(argc, argv));
  } else if (string_equal("generate-library", FLAG_command)) {
    generate_c_output_file(true, command_line_args_to_buffer(argc, argv));
    log_info("Exiting normally.");
    exit(0);
  } else if (string_equal("build", FLAG_command)) {
    if (string_is_null_or_empty(FLAG_c_output_file)) {
      log_fatal("Must supply --c-output-file");
      fatal_error(ERROR_ILLEGAL_INPUT);
    }
    if (string_is_null_or_empty(FLAG_binary_output_file)) {
      log_fatal("Must supply --binary-output-file");
      fatal_error(ERROR_ILLEGAL_INPUT);
    }
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

void parse_expression_string_and_print_parse_tree(char* expression) {
  if (expression == nullptr) {
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
    printer_t* printer = make_printer(output, make_symbol_table(), 2);
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
  printer_t* printer = make_printer(output, make_symbol_table(), 2);
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
    sub_process_read(sub_process, buffer, nullptr);
    usleep(5);
  } while (is_sub_process_running(sub_process));
  sub_process_read(sub_process, buffer, nullptr);
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
      || string_equal("tcc", FLAG_c_compiler)
      || string_equal("zig", FLAG_c_compiler)) {
    value_array_t* argv = make_value_array(2);
    value_array_add(argv, str_to_value(FLAG_c_compiler));
    if (string_equal("zig", FLAG_c_compiler)) {
      value_array_add(argv, str_to_value("cc"));
    }
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
