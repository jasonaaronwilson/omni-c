/**
 * @function
 *
 * Run the roci interpreter on a set of files
 */
void roci_command(void) {
  log_info("roci_command()");

  roci_env_t* env = roci_new_env(nullptr);
  roci_add_primitives_to_env(env);
  roci_eval_buffer(env, "src/roci/roci-lib.roci", get_roci_lib_buffer(), true);

  if (FLAG_roci_script == nullptr) {
    roci_repl(env);
    log_info("Exiting normally.");
    exit(0);
  }

  file_t* file = read_file(FLAG_roci_script);
  roci_eval_buffer(env, file->file_name, file->data, true);

  log_info("Exiting normally.");
  exit(0);
}

void roci_eval_buffer(roci_env_t* env, 
		      char* file_name,
		      buffer_t* buffer, 
		      boolean_t exit_on_failure) {

  roci_compiler_state_t* state = malloc_struct(roci_compiler_state_t);
  state->bblocks = make_value_array(16);
  state->buffer_number = roci_register_buffer(buffer, file_name);

  roci_compile_buffer(state, file_name, buffer);
  if (state->compiler_error != ROCI_COMPILE_TIME_ERROR_NONE) {
    if (exit_on_failure) {
      fatal_error(ERROR_ILLEGAL_STATE);
    } else {
      log_warn("A compilation error occurred!");
      return;
    }
  }

  value_array_t* bblocks = build_bblocks(state->bblocks);

  if (FLAG_roci_print_bbs) {
    buffer_t* buffer = make_buffer(1);
    disassemble_bblocks(bblocks, buffer);
    fprintf(stderr, buffer_to_c_string(buffer));
  }

  roci_bb_t* entry_point = value_array_get_ptr(bblocks, 0, typeof(roci_bb_t*));
  roci_execute(env, entry_point);
}
