/**
 * @function generate_header_file_command
 *
 * This command is used to generate a C99 header file to allow
 * C compatible languages to "call into" omni-c code.
 *
 * TODO(jawilson): maintain some kind of psuedo library and pure C
 * code and make sure it all works.
 */
void generate_header_file_command(buffer_t* command_line_overview_comment) {
  generate_c_output_file(OUTPUT_TYPE_C_HEADER_FILE, command_line_overview_comment);
  log_info("Exiting normally.");
  exit(0);
}
