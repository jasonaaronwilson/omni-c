/**
 * @function
 *
 * Run the roci interpreter on a set of files
 */
void roci_command(void) {
  log_warn("roci_command()");

  roci_compiler_state_t* state = malloc_struct(roci_compiler_state_t);
  state->bblocks = make_value_array(16);

  value_array_t* files = read_files(FLAG_files);
  for (int i = 0; i < files->length; i++) {
    file_t* file = value_array_get_ptr(files, i, typeof(file_t*));
    fprintf(stdout, "====================================================\n");
    fprintf(stdout, "====> roci compiling %s\n", file->file_name);
    fprintf(stdout, "====================================================\n");
    roci_compile_buffer(state, file->file_name, file->data);
    value_array_t* bblocks = build_bblocks(state->bblocks);

    buffer_t* buffer = make_buffer(1);
    disassemble_bblocks(bblocks, buffer);
    fprintf(stderr, buffer_to_c_string(buffer));
    if (true) {
      roci_env_t* env = roci_new_env(nullptr);
      roci_add_primitives_to_env(env);
      roci_bb_t* entry_point
          = value_array_get_ptr(bblocks, 0, typeof(roci_bb_t*));
      roci_execute(env, entry_point);
      if (true) {
        buffer_t* buffer = make_buffer(10);
        buffer_printf(buffer, "\n");
        roci_dump_env(env, buffer);
        fprintf(stdout, "%s", buffer_to_c_string(buffer));
      }
    }
    state->bblocks = make_value_array(16);
  }

  log_info("Exiting normally.");
  exit(0);
}
