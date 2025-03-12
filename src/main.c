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
    buffer_t* command_line_comment = command_line_args_to_buffer(argc, argv);
    generate_header_file_command(command_line_comment);
  } else if (string_equal("generate-library", FLAG_command)) {
    buffer_t* command_line_comment = command_line_args_to_buffer(argc, argv);
    generate_library_command(command_line_comment);
  } else if (string_equal("build", FLAG_command)) {
    buffer_t* command_line_comment = command_line_args_to_buffer(argc, argv);
    build_command(command_line_comment);
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
