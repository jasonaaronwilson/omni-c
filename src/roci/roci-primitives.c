/**
 * @file roci-primitives.c
 *
 * Roci has a minimal set of primitives. Primitives consume their
 * arguments from the stack and *always* push a single value back onto
 * the stack. A set of helper functions help with manipulating the
 * stack.
 */

static inline void roci_push_false(roci_vm_state_t* state) {
  *(state->stack++) = 0;
  *(state->stack_tags++) = ROCI_TAG_BOOLEAN;
}

static inline void roci_push_true(roci_vm_state_t* state) {
  *(state->stack++) = 1;
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
  uint64_t tos = *(--state->stack);
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

void roci_primitive_print_env(roci_vm_state_t* state) {
  buffer_t* buffer = make_buffer(10);
  roci_dump_env(state->env, buffer);
  fprintf(stdout, "%s", buffer_to_c_string(buffer));
}
