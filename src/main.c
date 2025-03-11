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
