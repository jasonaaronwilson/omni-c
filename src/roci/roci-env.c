/**
 * @file roci-env.c
 *
 * This is a not particularly clever abstraction for environments used
 * by the roci VM.
 */

typedef roci_env_t = struct {
  roci_env_t* parent;
  string_hashtable_t* bindings;
};

roci_value_t* roci_get_var(roci_env_t* env, char* name) {
  while (env != NULL) {
    value_result_t find_result = string_ht_find(env->bindings, name);
    if (is_ok(find_result)) {
      return cast(roci_value_t*, find_result.ptr);
    }
    env = env->parent;
  }
  return NULL;
}

void roci_set_var(roci_vm_state_t* state, char* name, value_t value,
                  roci_tag_t tag) {
  roci_value_t* binding = roci_get_var(state->env, name);
  if (binding == nullptr) {
    roci_debug_error(state, string_printf("Variable not found '%s'", name));
  }
  binding->raw = value.u64;
  binding->tag = tag;
}

void roci_define_var(roci_env_t* env, char* name, value_t value,
                     roci_tag_t tag) {
  value_result_t find_result = string_ht_find(env->bindings, name);
  if (is_ok(find_result)) {
    log_fatal("redefinition of a variable %s", name);
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  roci_value_t* binding = malloc_struct(roci_value_t);
  binding->raw = value.u64;
  binding->tag = tag;
  string_ht_insert(env->bindings, name, ptr_to_value(binding));
}

roci_env_t* roci_new_env(roci_env_t* parent) {
  roci_env_t* result = malloc_struct(roci_env_t);
  result->parent = parent;
  result->bindings = make_string_hashtable(7);
  return result;
}

void roci_dump_env(roci_env_t* env, buffer_t* buffer) {
  if (env == nullptr) {
    return;
  }

  roci_dump_env(env->parent, buffer);

  int depth = 0;
  roci_env_t* env2 = env;
  while (env2 != nullptr) {
    depth++;
    env2 = env2->parent;
  }

  // clang-format off
  string_ht_foreach(env->bindings, key, value, block_expr({
	buffer_append_repeated_byte(buffer, ' ', depth * 4);
	buffer_printf(buffer, "%s = ", cast(char*, key));
	roci_append_value(buffer, *(cast(roci_value_t*, value.ptr)));
	buffer_printf(buffer, "\n");
  }));
  // clang-format on
}
