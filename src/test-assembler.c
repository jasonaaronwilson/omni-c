/**
 * @function
 *
 * Run through a set of files assembling and disassembling them. It
 * will be a bit difficult to test against golden files since the
 * compiled bblock labels could appear anywhere in memory though
 * removing any line ending in ":" maybe work out OK.
 */
void test_assembler_command() {
  log_info("test_assembler_command()\n");
  value_array_t* files = read_files(FLAG_files);
  for (int i = 0; i < FLAG_files->length; i++) {
    log_info("test_assembler_command(%d)\n", i);
    file_t* file = value_array_get_ptr(files, i, typeof(file_t*));
    roci_bb_builder_array_t* builders = roci_assemble(file->data);
    value_array_t* bblocks = build_bblocks(builders);
    buffer_t* buffer = make_buffer(1);
    disassemble_bblocks(bblocks, buffer);
    fprintf(stderr, buffer_to_c_string(buffer));
  }
}
