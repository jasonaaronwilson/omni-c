/**
 * @function build_command
 *
 * This command is used to generate a C99 file which contains an
 * entire library. This is similar to build except that C compiler is
 * not invoked, only the C file is produced.
 */
void build_command(buffer_t* command_line_comment) {
  if (string_is_null_or_empty(FLAG_c_output_file)) {
    log_fatal("Must supply --c-output-file");
    fatal_error(ERROR_ILLEGAL_INPUT);
  }
  if (string_is_null_or_empty(FLAG_binary_output_file)) {
    log_fatal("Must supply --binary-output-file");
    fatal_error(ERROR_ILLEGAL_INPUT);
  }
  generate_c_output_file(true, command_line_comment);
  int status = invoke_c_compiler(FLAG_c_output_file, FLAG_binary_output_file);
  if (status == 0) {
    log_info("Exiting normally.");
    exit(0);
  } else {
    log_warn("Exiting abnormally.");
    exit(status);
  }
}
