/**
 * @file roci-repl.c
 *
 * This is the roci repl.
 */

void roci_repl(void) {
  fprintf(stdout, "/// Wecome to the roci read-eval-print-loop\n");

  roci_env_t* env = roci_new_env(nullptr);
  roci_add_primitives_to_env(env);
  while (true) {
    buffer_t* buffer = roci_repl_read(env);
    roci_compiler_state_t* state = malloc_struct(roci_compiler_state_t);
    state->bblocks = make_value_array(16);
    roci_compile_buffer(state, "*repl*", buffer);
    value_array_t* bblocks = build_bblocks(state->bblocks);
    roci_bb_t* entry_point
        = value_array_get_ptr(bblocks, 0, typeof(roci_bb_t*));
    roci_execute(env, entry_point);
  }
}

buffer_t* roci_repl_read(roci_env_t* env) {
  uint64_t last_length = 0xffffffff;
  buffer_t* buffer = make_buffer(10);
  while (true) {

    if (last_length != buffer->length) {
      if (last_length == 0xffffffff) {
        fprintf(stdout, "roci> ");
      } else {
        fprintf(stdout, "....> ");
      }
      fflush(stdout);
      last_length = buffer->length;
    }

    buffer_read_ready_bytes(buffer, stdin, 0xffffffff);
    if (buffer_equal(buffer, "#exit\n")) {
      exit(1);
    } else if (buffer_equal(buffer, "#env\n")) {
      buffer_t* env_buffer = make_buffer(10);
      roci_dump_env(env, env_buffer);
      fprintf(stdout, "----- Environment -----\n");
      fprintf(stdout, "%s", buffer_to_c_string(env_buffer));
      buffer_clear(buffer);
      last_length = 0xffffffff;
    } else {
      if (buffer_index_of(buffer, "\n\n") >= 0) {
        return buffer;
      }
    }
  }
}
