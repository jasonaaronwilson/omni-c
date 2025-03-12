/**
 * @function test_command
 *
 * This command is used to run unit tests. The source files are read
 * and processed like an ordinary build but an additional main()
 * routine is automatically generated for all functions that have a
 * signature like `void test_<*>(void);`.
 *
 * The special file extensions .expr and .stmt are treated specially
 * to faciltate "unit testing" the lexer, parser, the debug printer,
 * and the C appender.
 */
void test_command(buffer_t* command_line_comment) {
  handle_if_internal_test();

  if (FLAG_files->length == 0) {
    log_fatal("At least one file is required when using the test command.");
    exit(1);
  }

  // FIXME(jawilson):
  char* rand_binary_file_name = "/tmp/foo";

  if (string_is_null_or_empty(FLAG_binary_output_file)) {
    FLAG_binary_output_file = rand_binary_file_name;
  }
  if (string_is_null_or_empty(FLAG_c_output_file)) {
    FLAG_c_output_file = string_printf("%s.test.c", FLAG_binary_output_file);
  }

  // TODO(jawilson): maybe append (--binary-output-file and
  // --c-output-file to command_line_comment).

  /* ----- We need to be able to hook into generate_c_output_file ----- */
  generate_c_output_file(true, command_line_comment);

  int error_status
      = invoke_c_compiler(FLAG_c_output_file, FLAG_binary_output_file);
  if (error_status) {
    log_fatal(
        "The underlying C compiler produced an error compiling the test(s).");
    // TODO(jawilson): maybe a test banner? We should get a count back
    // of how many tests we didn't run? I guessing a rolling banner?
    exit(error_status);
  }

  run_test_binary(rand_binary_file_name);
}

void run_test_binary(char* rand_binary_file_name) {
  log_fatal("RUN TEST BINARY!");
  exit(1);
}

void handle_if_internal_test(void) {
  for (int i = 0; i < FLAG_files->length; i++) {
    char* filename = value_array_get(FLAG_files, i).str;
    if (string_ends_with(filename, ".expr")) {
      // Will not return.
      handle_expression_test(filename);
    } else if (string_ends_with(filename, ".stmt")) {
      // Will not return.
      handle_statement_test(filename);
    }
  }
}

void handle_statement_test(char* file_name) {
  log_fatal("IMPLEMENT ME!");
  exit(1);
}

void handle_expression_test(char* file_name) {
  parse_expression_string_and_print_parse_tree_from_buffer(
      buffer_read_file(file_name));
  exit(0);
}
