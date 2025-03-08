/**
 * @function archive_command
 *
 * This represents the archive "sub-command" of main.
 *
 * Using already parsed flags, generate an archive, and exit.
 */
void archive_command(void) {
  generate_archive_file();
  log_info("Exiting normally.");
  exit(0);
}

void generate_archive_file(void) {
  if (FLAG_archive_output_file == nullptr) {
    log_fatal("Must specify the archive output file name");
    exit(-1);
  }
  if (FLAG_files == nullptr || FLAG_files->length == 0) {
    log_warn("No archive members specified.");
    exit(-1);
  }

  // TODO(jawilson): what if the file already exists, etc.?
  FILE* out = fopen(FLAG_archive_output_file, "w");
  for (int i = 0; i < FLAG_files->length; i++) {
    oarchive_append_header_and_file_contents(
        out, value_array_get(FLAG_files, i).str);
  }
  fclose(out);
}
