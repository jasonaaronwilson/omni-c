/**
 * @file roci-primitives.c
 *
 * Roci has a minimal set of primitives. Primitives consume their
 * arguments from the stack and *always* push a single value back onto
 * the stack. A set of helper functions help with manipulating the
 * stack.
 */

void roci_add_primitives_to_env(roci_env_t* env) {
  roci_add_primitive(env, &roci_primitive_print_env, "debug_print_env");
  roci_add_primitive(env, &roci_primitive_print_string, "print_string");
  roci_add_primitive(env, &roci_primitive_string_append, "string_append");
  roci_add_primitive(env, &roci_primitive_string_equal, "string_equal");
  roci_add_primitive(env, &roci_primitive_string_starts_with,
                     "string_starts_with");
  roci_add_primitive(env, &roci_primitive_string_ends_with, "string_ends_with");
}

void roci_add_primitive(roci_env_t* env, roci_c_primitive_t primitive,
                        char* name) {
  roci_define_var(env, name, u64_to_value(cast(uint64_t, primitive)),
                  ROCI_TAG_C_PRIMITIVE);
}

void roci_primitive_print_env(roci_vm_state_t* state) {
  buffer_t* buffer = make_buffer(10);
  roci_dump_env(state->env, buffer);
  fprintf(stdout, "%s", buffer_to_c_string(buffer));
}

void roci_primitive_print_string(roci_vm_state_t* state) {
  if (state->n_args != 1) {
    roci_debug_error(state, "print_string expects a single string argument");
  }
  char* arg = roci_pop_string(state);
  fprintf(stdout, "%s", arg);
  roci_push_false(state);
}

void roci_primitive_string_equal(roci_vm_state_t* state) {
  if (state->n_args != 2) {
    roci_debug_error(state, "string_equal expects two string arguments");
  }
  char* arg1 = roci_pop_string(state);
  char* arg0 = roci_pop_string(state);
  if (string_equal(arg0, arg1)) {
    roci_push_true(state);
  } else {
    roci_push_false(state);
  }
}

void roci_primitive_string_starts_with(roci_vm_state_t* state) {
  if (state->n_args != 2) {
    roci_debug_error(state, "string_starts_with expects two string arguments");
  }
  char* arg1 = roci_pop_string(state);
  char* arg0 = roci_pop_string(state);
  if (string_starts_with(arg0, arg1)) {
    roci_push_true(state);
  } else {
    roci_push_false(state);
  }
}

void roci_primitive_string_ends_with(roci_vm_state_t* state) {
  if (state->n_args != 2) {
    roci_debug_error(state, "string_ends_with expects two string arguments");
  }
  char* arg1 = roci_pop_string(state);
  char* arg0 = roci_pop_string(state);
  if (string_ends_with(arg0, arg1)) {
    roci_push_true(state);
  } else {
    roci_push_false(state);
  }
}

void roci_primitive_string_append(roci_vm_state_t* state) {
  buffer_t* buffer = make_buffer(10);
  for (int64_t arg_num = 0; arg_num < state->n_args; arg_num++) {
    roci_value_t value = roci_debug_peek_value(state, state->n_args - arg_num);
    if (value.tag != ROCI_TAG_STRING) {
      roci_debug_error(state, "string_append expects string arguments");
    }
    buffer_append_string(buffer, cast(char*, value.raw));
  }
  roci_push_string(state, buffer_to_c_string(buffer));
}
