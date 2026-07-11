/**
 * @file roci-stack.c
 *
 * Typesafe handling of the roci stack.
 *
 * TODO(jawilson): also track stack depth and error out if out of
 * bounds.
 */

static inline void roci_push_false(roci_vm_state_t* state) {
  *(state->stack++) = 0;
  *(state->stack_tags++) = ROCI_TAG_BOOLEAN;
}

static inline void roci_push_true(roci_vm_state_t* state) {
  *(state->stack++) = 1;
  *(state->stack_tags++) = ROCI_TAG_BOOLEAN;
}

static inline void roci_push_boolean(roci_vm_state_t* state, boolean_t value) {
  *(state->stack++) = value ? 1 : 0;
  *(state->stack_tags++) = ROCI_TAG_BOOLEAN;
}

static inline void roci_push_integer(roci_vm_state_t* state, int64_t number) {
  *(state->stack++) = number;
  *(state->stack_tags++) = ROCI_TAG_INTEGER;
}

static inline void roci_push_double(roci_vm_state_t* state, double number) {
  *(state->stack++) = double_as_uint64(number);
  *(state->stack_tags++) = ROCI_TAG_DOUBLE;
}

static inline void roci_push_string(roci_vm_state_t* state, char* str) {
  *(state->stack++) = cast(uint64_t, str);
  *(state->stack_tags++) = ROCI_TAG_STRING;
}

static inline void roci_push_list(roci_vm_state_t* state, value_array_t* str) {
  *(state->stack++) = cast(uint64_t, str);
  *(state->stack_tags++) = ROCI_TAG_LIST;
}

static inline void roci_push_value_parts(roci_vm_state_t* state, uint64_t data,
                                         roci_tag_t tag) {
  *(state->stack++) = data;
  *(state->stack_tags++) = tag;
}

static inline void roci_push_value(roci_vm_state_t* state, roci_value_t value) {
  roci_push_value_parts(state, value.raw, value.tag);
}

static inline boolean_t roci_pop_boolean(roci_vm_state_t* state) {
  roci_tag_t tag = *(--state->stack_tags);
  uint64_t tos = *(--state->stack);
  if (tag != ROCI_TAG_BOOLEAN) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return tos;
}

static inline int64_t roci_pop_integer(roci_vm_state_t* state) {
  roci_tag_t tag = *(--state->stack_tags);
  int64_t tos = *(--state->stack);
  if (tag != ROCI_TAG_INTEGER) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return tos;
}

static inline double roci_pop_double(roci_vm_state_t* state) {
  roci_tag_t tag = *(--state->stack_tags);
  uint64_t tos = *(--state->stack);
  if (tag != ROCI_TAG_DOUBLE) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return uint64_as_double(tos);
}

static inline char* roci_pop_string(roci_vm_state_t* state) {
  roci_tag_t tag = *(--state->stack_tags);
  uint64_t tos = *(--state->stack);
  if (tag != ROCI_TAG_STRING) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return cast(char*, tos);
}

static inline value_array_t* roci_pop_list(roci_vm_state_t* state) {
  roci_tag_t tag = *(--state->stack_tags);
  uint64_t tos = *(--state->stack);
  if (tag != ROCI_TAG_LIST) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return cast(value_array_t*, tos);
}

static inline roci_value_t roci_pop_value(roci_vm_state_t* state) {
  roci_value_t value;
  roci_tag_t tag = *(--state->stack_tags);
  uint64_t tos = *(--state->stack);
  if (tag == ROCI_TAG_STACK_MARKER) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  value.raw = tos;
  value.tag = tag;
  return value;
}

static inline roci_value_t roci_debug_peek_value(roci_vm_state_t* state,
                                                 int offset) {
  roci_value_t value;
  roci_tag_t tag = *(state->stack_tags - offset);
  uint64_t datum = *(state->stack - offset);
  value.raw = datum;
  value.tag = tag;
  return value;
}

static inline roci_env_t* roci_current_env(roci_vm_state_t* state) {
  return state->env;
}

static inline void roci_set_env(roci_vm_state_t* state, roci_env_t* env) {
  state->env = env;
}

static inline void roci_drop_env(roci_vm_state_t* state) {
  state->env = state->env->parent;
}

static inline void roci_push_continuation(roci_vm_state_t* state, roci_bb_t* bb,
                                          uint32_t stack_arg_numbers) {
  roci_cont_t* continuation = malloc_struct(roci_cont_t);
  continuation->bb = bb;
  continuation->env = state->env;
  continuation->stack = state->stack - stack_arg_numbers;
  continuation->stack_tags = state->stack_tags - stack_arg_numbers;
  *(state->continuations++) = continuation;
}

static inline roci_cont_t* roci_pop_continuation(roci_vm_state_t* state) {
  return *(--state->continuations);
}
