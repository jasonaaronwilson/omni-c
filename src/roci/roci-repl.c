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
    buffer_t* buffer = roci_repl_read();
    roci_compiler_state_t* state = malloc_struct(roci_compiler_state_t);
    state->bblocks = make_value_array(16);
    roci_compile_buffer(state, "*repl*", buffer);
    value_array_t* bblocks = build_bblocks(state->bblocks);
    roci_bb_t* entry_point
        = value_array_get_ptr(bblocks, 0, typeof(roci_bb_t*));
    roci_execute(env, entry_point);
  }
}

buffer_t* roci_repl_read(void) {
  fprintf(stdout, "roci> ");
  fflush(stdout);
  buffer_t* buffer = make_buffer(10);
  while (true) {
    buffer_read_ready_bytes(buffer, stdin, 0xffffffff);
    if (buffer_index_of(buffer, "#exit") >= 0) {
      exit(1);
    }
    if (buffer_index_of(buffer, "\n\n") >= 0) {
      return buffer;
    }
  }
}
