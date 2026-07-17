/**
 * @function
 *
 * Run the roci interpreter on a set of files
 */
void roci_command(void) {
  log_info("roci_command()");

  if (FLAG_roci_script == nullptr) {
    roci_env_t* env = roci_new_env(nullptr);
    roci_add_primitives_to_env(env);
    roci_repl(env);
    
    log_info("Exiting normally.");
    exit(0);
  }

  roci_compiler_state_t* state = malloc_struct(roci_compiler_state_t);
  state->bblocks = make_value_array(16);

  file_t* file = read_file(FLAG_roci_script);
  state->buffer_number = roci_register_buffer(file->data, file->file_name);
  roci_compile_buffer(state, file->file_name, file->data);
  value_array_t* bblocks = build_bblocks(state->bblocks);

  if (FLAG_roci_print_bbs) {
    buffer_t* buffer = make_buffer(1);
    disassemble_bblocks(bblocks, buffer);
    fprintf(stderr, buffer_to_c_string(buffer));
  }

  if (true) {
    roci_env_t* env = roci_new_env(nullptr);
    roci_add_primitives_to_env(env);
    roci_bb_t* entry_point
      = value_array_get_ptr(bblocks, 0, typeof(roci_bb_t*));
    roci_execute(env, entry_point);
    if (false) {
      buffer_t* buffer = make_buffer(10);
      buffer_printf(buffer, "\n");
      roci_dump_env(env, buffer);
      fprintf(stdout, "%s", buffer_to_c_string(buffer));
    }
  }

  log_info("Exiting normally.");
  exit(0);
}
