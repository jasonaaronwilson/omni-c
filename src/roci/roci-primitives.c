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
  char* arg = roci_pop_string(state);
  fprintf(stdout, "%s", arg);
  roci_push_false(state);
}
