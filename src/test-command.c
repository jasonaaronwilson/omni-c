/**
 * @function test_command
 *
 * The source files are read and processed like an ordinary build but
 * an additional main() * routine is automatically generated for all
 * functions that have a * signature like `void test_*(void);`.
 *
 * If the only input are files with .expr or .stmt, then internal test
 * behavior is used instead (lexing + parsing + parse tree printing +
 * printing as C) is performed instead.
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
  generate_c_output_file(OUTPUT_TYPE_C_UNIT_TEST_FILE, command_line_comment);

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

void run_test_binary(char* binary_file_name) {
  log_info("Running test binary %s", binary_file_name);

  value_array_t* argv = make_value_array(1);
  value_array_add(argv, str_to_value(binary_file_name));

  buffer_t* child_stdin = make_buffer(1);
  buffer_t* child_stdout = make_buffer(1);
  buffer_t* child_stderr = make_buffer(1);

  sub_process_t* sub_process = make_sub_process(argv);
  sub_process_launch_and_wait(sub_process, child_stdin, child_stdout,
                              child_stderr);

  if (sub_process->exit_code) {
    log_warn("The test binary was NOT happy.");
    log_warn("   Child stdout was %s\n", buffer_to_c_string(child_stdin));
    log_warn("   Child stderr was %s\n", buffer_to_c_string(child_stderr));
  } else {
    log_warn("The test binary was happy. :)");
  }

  exit(sub_process->exit_code);
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
  parse_statement_string_and_print_parse_tree_from_buffer(
      buffer_read_file(file_name));
  exit(0);
}

void handle_expression_test(char* file_name) {
  parse_expression_string_and_print_parse_tree_from_buffer(
      buffer_read_file(file_name));
  exit(0);
}
