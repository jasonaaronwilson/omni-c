void test_assembler_command() {
  log_info("test_assembler_command()\n");
  value_array_t* files = read_files(FLAG_files);
  for (int i = 0; i < FLAG_files->length; i++) {
    log_info("test_assembler_command(%d)\n", i);
    file_t* file = value_array_get_ptr(files, i, typeof(file_t*));
    roci_bb_builder_array_t* builders
        = roci_assemble(buffer_to_c_string(file->data));
    roci_bb_t* entry_bb = build_bblocks(builders);
    fprintf(stderr, "num_data = %d, num_opcodes = %d\n", entry_bb->num_data,
            entry_bb->num_opcodes);
    buffer_t* buffer = make_buffer(1);
    bblock_to_buffer(buffer, entry_bb);
    fprintf(stderr, buffer_to_c_string(buffer));
  }
}
