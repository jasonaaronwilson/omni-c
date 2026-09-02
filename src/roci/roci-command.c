/**
 * @function
 *
 * Run the roci interpreter on a set of files
 */
void roci_command(void) {
  log_info("roci_command()");

  // Don't try to use full expressions for the roci library yet.
  boolean_t saved_use_full_expression = FLAG_roci_use_full_expressions;
  FLAG_roci_use_full_expressions = false;

  roci_env_t* env = roci_new_env(nullptr);
  roci_add_primitives_to_env(env);
  roci_eval_buffer(env, "src/roci/roci-lib.roci", get_roci_lib_buffer(), true);

  FLAG_roci_use_full_expressions = saved_use_full_expression;

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

  roci_compiler_state_t* state = malloc_struct(roci_compiler_state_t);
  state->bblocks = make_value_array(16);
  state->buffer_number = roci_register_buffer(buffer, file_name);
  result.compiler_state = state;

  roci_compile_buffer(state, file_name, buffer);
  if (state->compiler_error != ROCI_COMPILE_TIME_ERROR_NONE) {
    if (exit_on_failure) {
      fatal_error(ERROR_ILLEGAL_STATE);
      // NOT REACHED.
      return result;
    } else {
      log_warn("A compilation error occurred!");
      return result;
    }
  }

  value_array_t* bblocks = build_bblocks(state->bblocks);

  if (FLAG_roci_print_bbs) {
    buffer_t* buffer = make_buffer(1);
    disassemble_bblocks(bblocks, buffer);
    fprintf(stderr, buffer_to_c_string(buffer));
  }

  roci_bb_t* entry_point = value_array_get_ptr(bblocks, 0, typeof(roci_bb_t*));
  roci_vm_state_t* vm_state = roci_make_vm_state(env);
  set_fatal_error_callback(&roci_eval_error_callback,
                           make_eval_error_callback_data(
                               vm_state, file_name, buffer, exit_on_failure));
  roci_execute(vm_state, entry_point);
  set_fatal_error_callback(nullptr, nullptr);

  return result;
}

typedef roci_eval_error_callback_data_t = struct {
  roci_vm_state_t* state;
  char* file_name;
  buffer_t* buffer;
  boolean_t exit_on_failure;
};

roci_eval_error_callback_data_t*
    make_eval_error_callback_data(roci_vm_state_t* state, char* file_name,
                                  buffer_t* buffer, boolean_t exit_on_failure) {
  roci_eval_error_callback_data_t* result
      = malloc_struct(roci_eval_error_callback_data_t);
  result->state = state;
  result->file_name = file_name;
  result->buffer = buffer;
  result->exit_on_failure = exit_on_failure;
  return result;
}

void roci_eval_error_callback(char* file, int line, int error_code,
                              void* data_in) {
  roci_eval_error_callback_data_t* data
      = cast(roci_eval_error_callback_data_t*, data_in);
  log_fatal("An internal error occurred evaluating %s", data->file_name);
  if (!data->exit_on_failure) {
    // Prevent recursive entry though we might want to clear this...
    data->exit_on_failure = true;
    roci_debug_error(
        data->state,
        "Entering roci debugger after a fatal error was signaled.");
  }
}
