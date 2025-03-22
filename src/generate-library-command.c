/**
 * @function generate_library_command
 *
 * This command is used to generate a C99 file which contains an
 * entire library. This is similar to build except that C compiler is
 * not invoked, only the C file is produced.
 */
void generate_library_command(buffer_t* command_line_overview_comment) {
  generate_c_output_file(OUTPUT_TYPE_C_LIBRARY_FILE,
                         command_line_overview_comment);
  log_info("Exiting normally.");
  exit(0);
}
