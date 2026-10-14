/**
 * @function
 *
 * Run the roci interpreter on a set of files
 */
void roci_command(void) {
  log_info("roci_command()");

  // Don't try to use full expressions for the roci library yet.
  // boolean_t saved_use_full_expression = FLAG_roci_use_full_expressions;
  // FLAG_roci_use_full_expressions = false;

  roci_env_t* env = roci_new_env(nullptr);
  roci_add_primitives_to_env(env);
  roci_eval_buffer(env, "src/roci/roci-lib.roci", get_roci_lib_buffer(), true);

  // FLAG_roci_use_full_expressions = saved_use_full_expression;

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

typedef roci_eval_result_t = struct {
  roci_compiler_state_t* compiler_state;
};

roci_eval_result_t roci_eval_buffer(roci_env_t* env, char* file_name,
                                    buffer_t* buffer,
                                    boolean_t exit_on_failure) {
  roci_eval_result_t result = {0};
  error_t* error = nullptr;
  try_call(roci_eval_buffer_impl(env, file_name, buffer, &result), error);
  if (error != nullptr) {
    log_warn("An internal error was encountered evaluating the buffer.");
    return result;
  }
  return result;
}

void roci_eval_buffer_impl(roci_env_t* env, char* file_name,
			   buffer_t* buffer,
			   roci_eval_result_t* result) {

  roci_compiler_state_t* state = malloc_struct(roci_compiler_state_t);
  state->bblocks = make_value_array(16);
  state->buffer_number = roci_register_buffer(buffer, file_name);
  result->compiler_state = state;

  roci_compile_buffer(state, file_name, buffer);
  if (state->compiler_error != ERROR_NONE) {
    return;
  }

  value_array_t* bblocks = build_bblocks(state->bblocks);

  if (FLAG_roci_print_bbs) {
    buffer_t* buffer = make_buffer(1);
    disassemble_bblocks(bblocks, buffer);
    fprintf(stderr, "%s", buffer_to_c_string(buffer));
  }

  roci_bb_t* entry_point = value_array_get_ptr(bblocks, 0, typeof(roci_bb_t*));
  roci_vm_state_t* vm_state = roci_make_vm_state(env);
  roci_execute(vm_state, entry_point);
}
