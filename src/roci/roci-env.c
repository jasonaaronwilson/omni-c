/**
 * @file roci-env.c
 *
 * This is a not particular clever abstraction for environments used
 * by the roci VM.
 */

typedef roci_env_binding_t = struct {
  value_t value;
  roci_tag_t tag;
};

typedef roci_env_t = struct {
  roci_env_t* parent;
  string_hashtable_t* bindings;
};

roci_env_binding_t* roci_get_var(roci_env_t* env, char* name) {
  while (env != NULL) {
    value_result_t find_result = string_ht_find(env->bindings, name);
    if (is_ok(find_result)) {
      return cast(roci_env_binding_t*, find_result.ptr);
    }
    env = env->parent;
  }
  return NULL;
}

void roci_set_var(roci_env_t* env, char* name, value_t value, roci_tag_t tag) {
  roci_env_binding_t* binding = roci_get_var(env, name);
  binding->value = value;
  binding->tag = tag;
}

void roci_define_var(roci_env_t* env, char* name, value_t value,
                     roci_tag_t tag) {
  value_result_t find_result = string_ht_find(env->bindings, name);
  if (is_ok(find_result)) {
    log_fatal("redefinition of a variable %s", name);
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  roci_env_binding_t* binding = malloc_struct(roci_env_binding_t);
  binding->value = value;
  binding->tag = tag;
  string_ht_insert(env->bindings, name, ptr_to_value(binding));
}

roci_env_t* roci_new_env(roci_env_t* parent) {
  roci_env_t* result = malloc_struct(roci_env_t);
  result->parent = parent;
  result->bindings = make_string_hashtable(7);
  return result;
}
