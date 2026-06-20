/**
 * @file roci-primitives.c
 *
 * Roci has a minimal set of primitives. Primitives consume their
 * arguments from the stack and *always* push a single value back onto
 * the stack. A set of helper functions help with manipulating the
 * stack.
 */

void roci_add_primitives_to_env(roci_env_t* env) {
  roci_add_primitive(env, &roci_primitive_load, "load");
  roci_add_primitive(env, &roci_primitive_print_env, "debug_print_env");
  roci_add_primitive(env, &roci_primitive_to_string, "to_string");
  roci_add_primitive(env, &roci_primitive_print_string, "print_string");
  roci_add_primitive(env, &roci_primitive_string_append, "string_append");
  roci_add_primitive(env, &roci_primitive_string_equal, "string_equal");
  roci_add_primitive(env, &roci_primitive_string_starts_with,
                     "string_starts_with");
  roci_add_primitive(env, &roci_primitive_string_ends_with, "string_ends_with");
  roci_add_primitive(env, &roci_primitive_string_substring, "string_substring");
  roci_add_primitive(env, &roci_primitive_make_list, "make_list");
  roci_add_primitive(env, &roci_primitive_list_get, "list_get");
  roci_add_primitive(env, &roci_primitive_list_set, "list_set");
  roci_add_primitive(env, &roci_primitive_list_push, "list_push");
  roci_add_primitive(env, &roci_primitive_list_push, "list_for_each");
}

void roci_add_primitive(roci_env_t* env, roci_c_primitive_t primitive,
                        char* name) {
  roci_define_var(env, name, u64_to_value(cast(uint64_t, primitive)),
                  ROCI_TAG_C_PRIMITIVE);
}

void roci_primitive_load(roci_vm_state_t* state) {
  if (state->n_args != 1) {
    roci_debug_error(state, "roci_load expects 1 argument");
  }
  char* filename = roci_pop_string(state);
  file_t* file = read_file(filename);
  roci_compiler_state_t* compiler_state = malloc_struct(roci_compiler_state_t);
  compiler_state->bblocks = make_value_array(16);
  roci_compile_buffer(compiler_state, file->file_name, file->data);
  value_array_t* bblocks = build_bblocks(compiler_state->bblocks);
  roci_bb_t* entry_point = value_array_get_ptr(bblocks, 0, typeof(roci_bb_t*));
  roci_execute(state->env, entry_point);
  roci_push_false(state);
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

void roci_primitive_string_substring(roci_vm_state_t* state) {
  if (state->n_args != 3) {
    roci_debug_error(state, "string_substring expectds 3 arguments");
  }
  int64_t end = roci_pop_integer(state);
  int64_t start = roci_pop_integer(state);
  char* str = roci_pop_string(state);
  char* result = string_substring(str, start, end);
  roci_push_string(state, result);
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

void roci_primitive_make_list(roci_vm_state_t* state) {
  value_array_t* list = make_value_array(state->n_args);
  for (int64_t arg_num = 0; arg_num < state->n_args; arg_num++) {
    roci_value_t* value = roci_value_to_heap(
        roci_debug_peek_value(state, state->n_args - arg_num));
    value_array_add(list, ptr_to_value(value));
  }
  roci_push_list(state, list);
}

void roci_primitive_list_get(roci_vm_state_t* state) {
  if (state->n_args != 2) {
    roci_debug_error(state, "list_get expects 2 arguments");
  }
  int64_t position = roci_pop_integer(state);
  value_array_t* list = roci_pop_list(state);
  roci_value_t* element
      = cast(roci_value_t*, value_array_get(list, position).ptr);
  roci_push_value(state, *element);
}

void roci_primitive_list_set(roci_vm_state_t* state) {
  if (state->n_args != 3) {
    roci_debug_error(state, "list_set expects 3 arguments");
  }
  roci_value_t element = roci_pop_value(state);
  int64_t position = roci_pop_integer(state);
  value_array_t* list = roci_pop_list(state);
  value_array_replace(list, position,
                      ptr_to_value(roci_value_to_heap(element)));
  roci_push_false(state);
}

void roci_primitive_list_push(roci_vm_state_t* state) {
  if (state->n_args != 2) {
    roci_debug_error(state, "list_push expects 2 arguments");
  }
  roci_value_t element = roci_pop_value(state);
  value_array_t* list = roci_pop_list(state);
  value_array_push(list, ptr_to_value(roci_value_to_heap(element)));
  roci_push_false(state);
}


void roci_primitive_list_for_each(roci_vm_state_t* state) {
  if (state->n_args != 2) {
    roci_debug_error(state, "list_for_each requires two arguments");
  }
  roci_value_t proc = roci_pop_value(state);
  value_array_t* list = roci_pop_list(state);
  for (int i = 0; i < list->length; i++) {
    roci_value_t* element = cast(roci_value_t*, value_array_get(list, i).ptr);
    roci_push_value(state, *element);
    roci_call(state, proc, 1);
  }
  roci_push_false(state);
}

void roci_primitive_to_string(roci_vm_state_t* state) {
  if (state->n_args != 1) {
    roci_debug_error(state, "to_string expects 1 argument");
  }
  roci_value_t element = roci_pop_value(state);
  buffer_t* buffer = make_buffer(8);
  roci_push_string(state, roci_value_to_c_string(element));
}
