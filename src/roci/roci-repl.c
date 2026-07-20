/**
 * @file roci-repl.c
 *
 * This is the roci repl.
 */

void roci_repl(roci_env_t* env) {
  fprintf(stdout, "/// Wecome to the roci read-eval-print-loop\n");
  fprintf(stdout, "///\n");
  fprintf(stdout, "/// Special commands:\n");
  fprintf(stdout, "/// #exit to exit this repl\n");
  fprintf(stdout, "/// #env to dump the environment\n");
  fprintf(stdout, "///\n");

  while (true) {
    buffer_t* buffer = roci_repl_read(env);
    if (buffer == nullptr) {
      return;
    }
    roci_compiler_state_t* state = malloc_struct(roci_compiler_state_t);
    state->bblocks = make_value_array(16);
    roci_compile_buffer(state, "*repl*", buffer);
    if (state->compiler_error == ROCI_COMPILE_TIME_ERROR_NONE) {
      value_array_t* bblocks = build_bblocks(state->bblocks);
      roci_bb_t* entry_point
        = value_array_get_ptr(bblocks, 0, typeof(roci_bb_t*));
      roci_execute(env, entry_point);
    } else {
      log_warn("last input ignored because of a compiler error...");
      state->compiler_error = ROCI_COMPILE_TIME_ERROR_NONE;
    }
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
    if ((buffer_ends_with(buffer, ";\n") || buffer_ends_with(buffer, "}\n"))
        && roci_is_balanced(buffer)) {
      return buffer;
    } else if (buffer_equal(buffer, "#exit\n")) {
      return nullptr;
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

/**
 * @function roci_is_balanced
 *
 * Return true if we are looking at a balanced expression. This can
 * get messed up with comments, strings, etc., but it a good enough
 * approximation for now.
 */
boolean_t roci_is_balanced(buffer_t* buffer) {
  int num_open_parens = 0;
  int num_open_braces = 0;
  for (uint64_t i = 0; i < buffer->length; i++) {
    uint8_t ch = buffer_get(buffer, i);
    switch (ch) {
    case '(':
      num_open_parens += 1;
      break;
    case ')':
      num_open_parens -= 1;
      break;
    case '{':
      num_open_braces += 1;
      break;
    case '}':
      num_open_braces -= 1;
      break;
    default:
      break;
    }
  }
  return (num_open_parens == 0) && (num_open_braces == 0);
}
