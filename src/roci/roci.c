/**
 * @file
 *
 * Roci is an interpreter for a C syntax "nano" Lisp/Scheme
 * interpreter with an intentionally minimal runtime (very limited
 * I/O). It's intended to be used as a configuration language for a
 * cross platform build tool which does what "ninja" doesn't.
 *
 * The VM is also a little unique because objects in the heap are
 * stored without tags but the stack and variable environments still
 * track tags to conservatively provide some dynmaic type checking.
 */

typedef roci_tag_t = enum {
  ROCI_TAG_UNKNOWN,
  ROCI_TAG_BOOLEAN,
  ROCI_TAG_INTEGER,
  ROCI_TAG_DOUBLE,
  ROCI_TAG_STRING,
  ROCI_TAG_SYMBOL,
  ROCI_TAG_CLOSURE,
  ROCI_TAG_C_PRIMITIVE,
  ROCI_TAG_ARRAY,
};

typedef roci_opcode_t = enum {
  ROCI_OPCODE_TRAP,
  ROCI_OPCODE_PUSH_FALSE,
  ROCI_OPCODE_PUSH_TRUE,
  ROCI_OPCODE_PUSH_INTEGER,
  ROCI_OPCODE_PUSH_DOUBLE,
  ROCI_OPCODE_PUSH_STRING,
  ROCI_OPCODE_PUSH_SYMBOL,

  ROCI_OPCODE_DROP,

  ROCI_OPCODE_NEW_ENVIRONMENT,
  ROCI_OPCODE_DEFINE_VAR,
  ROCI_OPCODE_GET_VAR,
  ROCI_OPCODE_SET_VAR,

  ROCI_OPCODE_BR_FALSE,
  ROCI_OPCODE_BR,

  ROCI_OPCODE_CALL,
  ROCI_OPCODE_RETURN,
};

typedef roci_bb_t = struct {
  uint32_t num_data;
  uint32_t num_opcodes;
  // data
  // opcodes + alignment nops
};

typedef roci_bb_builder_t = struct {
  char* bblock_label;
  roci_bb_t* bblock;
  value_array_t* data;
  buffer_t* opcodes;
};

typedef roci_bb_builder_array_t = value_array_t;

typedef roci_runtime_error_t = enum {
  ROCI_RUNTIME_ERROR_NONE,
  ROCI_RUNTIME_ERROR_TRAP,
  ROCI_RUNTIME_ERROR_ILLEGAL_OPCODE,
  ROCI_RUNTIME_ERROR_BOOLEAN_REQUIRED,
};

typedef roci_vm_state_t = struct {
  roci_runtime_error_t runtime_error;
  uint8_t* opcode_ptr;
  uint64_t* data_ptr;
  uint64_t* stack;
  uint8_t* stack_tags;
  roci_bb_t** return_stack;
  roci_env_t* env;
};

typedef roci_env_binding_t = struct {
  value_t value;
  roci_tag_t tag;
};

typedef roci_env_t = struct {
  roci_env_t* parent;
  string_hashtable_t* bindings;
};

/**
 * @function
 *
 */
roci_bb_builder_t* add_bblock(roci_bb_builder_array_t* bblocks) {
  roci_bb_builder_t* result = malloc_struct(roci_bb_builder_t);
  result->opcodes = make_buffer(10);
  result->data = make_value_array(1);
  value_array_add(bblocks, ptr_to_value(result));
  return result;
}

/**
 * @function
 *
 */
roci_bb_builder_t* get_bblock(roci_bb_builder_array_t* bblocks, int number) {
  return value_array_get_ptr(bblocks, number, typeof(roci_bb_builder_t*));
}

/**
 * @function
 *
 * Build and link all of the bblocks into the interpreter efficient
 * format and return the address of the first bblock.
 *
 * If there are no bblocks, then NULL is returned.
 *
 * This is slightly tricky to use and will evolve over time at which
 * time hopefully I will update the documentation.
 */
roci_bb_t* build_bblocks(roci_bb_builder_array_t* bblocks) {

  log_info("roci -- build_bblocks\n");

  // First allocate all of the bblocks before building the final
  // contents so we can "link" everything properly.
  for (int i = 0; i < bblocks->length; i++) {
    roci_bb_builder_t* builder
        = value_array_get_ptr(bblocks, i, typeof(roci_bb_builder_t*));

    log_info("roci -- builder %d, %d\n", builder->data->length,
             builder->opcodes->length);

    roci_bb_t* bblock
        = malloc((1 + builder->data->length) * 8 + builder->opcodes->length);
    bblock->num_data = builder->data->length;
    bblock->num_opcodes = builder->opcodes->length;
    builder->bblock = bblock;
  }

  // Now copy over opcodes and data converting bblock numbers to
  // bblock addresses
  for (int i = 0; i < bblocks->length; i++) {
    copy_opcodes_and_link(
        bblocks, value_array_get_ptr(bblocks, i, typeof(roci_bb_builder_t*)));
  }

  return value_array_get_ptr(bblocks, 0, typeof(roci_bb_builder_t*))->bblock;
}

void copy_opcodes_and_link(roci_bb_builder_array_t* bblocks,
                           roci_bb_builder_t* builder) {
  roci_bb_t* bb = builder->bblock;
  uint8_t* opcode_ptr = bblock_opcode_pointer(bb);
  uint64_t* data_ptr = bblock_data_pointer(bb);
  int dindex = 0;

  for (int i = 0; i < builder->opcodes->length; i++) {
    roci_opcode_t opcode = buffer_get(builder->opcodes, i);
    *(opcode_ptr++) = opcode;
    switch (opcode) {
    case ROCI_OPCODE_PUSH_INTEGER:
    case ROCI_OPCODE_PUSH_DOUBLE:
    case ROCI_OPCODE_PUSH_STRING:
    case ROCI_OPCODE_PUSH_SYMBOL:
    case ROCI_OPCODE_GET_VAR:
    case ROCI_OPCODE_SET_VAR:
    case ROCI_OPCODE_DEFINE_VAR:
      *(data_ptr++) = value_array_get(builder->data, dindex++).u64;
      break;

    case ROCI_OPCODE_BR_FALSE:
    case ROCI_OPCODE_BR:
      char* label = value_array_get_ptr(builder->data, dindex++, typeof(char*));
      *(data_ptr++) = bb_label_to_address(bblocks, label);
      break;

    default:
      break;
    }
  }
}

uint64_t bb_label_to_address(roci_bb_builder_array_t* bblocks, char* label) {
  for (int i = 0; i < bblocks->length; i++) {
    roci_bb_builder_t* builder
        = value_array_get_ptr(bblocks, i, typeof(roci_bb_builder_t*));
    if (string_equal(builder->bblock_label, label)) {
      return cast(uint64_t, builder->bblock);
    }
  }
  log_fatal("bblock with label %s not found", label);
  fatal_error(ERROR_ILLEGAL_STATE);
  return 0;
}

/**
 * @function
 *
 * Once a file is compiled into bblocks, this should be used to run
 * that program fragment.
 */
roci_runtime_error_t roci_execute(roci_bb_t* entry_point) {
  roci_vm_state_t* state = malloc_struct(roci_vm_state_t);
  state->stack = cast(uint64_t*, malloc(256 * 8));
  state->stack_tags = cast(uint8_t*, malloc(256));
  state->return_stack = cast(roci_bb_t**, malloc(16 * 8));
  // TODO(jawilson): take a roci_env_t* top_level_environment
  return roci_execute_bblock(entry_point, state);
}

/**
 * @function
 *
 */
void roci_runtime_error(roci_runtime_error_t runtime_error) {
  log_fatal("A runtime error has occurred evaluating a roci script");
  fatal_error(ERROR_ILLEGAL_STATE);
}

inline uint8_t* bblock_opcode_pointer(roci_bb_t* bb) {
  return cast(uint8_t*, bb) + 8 + bb->num_data * 8;
}

inline uint64_t* bblock_data_pointer(roci_bb_t* bb) {
  return cast(uint64_t*, bb) + 1;
}

roci_runtime_error_t roci_execute_bblock(roci_bb_t* bb,
                                         roci_vm_state_t* state) {
start_bblock:
  state->opcode_ptr = bblock_opcode_pointer(bb);
  state->data_ptr = bblock_data_pointer(bb);
  while (true) {
    roci_opcode_t opcode = *(state->opcode_ptr++);
    switch (opcode) {
    case ROCI_OPCODE_TRAP:
      return ROCI_RUNTIME_ERROR_TRAP;

    case ROCI_OPCODE_PUSH_FALSE:
      *(state->stack++) = 0;
      *(state->stack_tags++) = ROCI_TAG_BOOLEAN;
      break;

    case ROCI_OPCODE_PUSH_TRUE:
      *(state->stack++) = 1;
      *(state->stack_tags++) = ROCI_TAG_BOOLEAN;
      break;

    case ROCI_OPCODE_PUSH_INTEGER:
      *(state->stack++) = *(state->data_ptr++);
      *(state->stack_tags++) = ROCI_TAG_INTEGER;
      break;

    case ROCI_OPCODE_PUSH_DOUBLE:
      *(state->stack++) = *(state->data_ptr++);
      *(state->stack_tags++) = ROCI_TAG_DOUBLE;
      break;

    case ROCI_OPCODE_PUSH_STRING:
      *(state->stack++) = *(state->data_ptr++);
      *(state->stack_tags++) = ROCI_TAG_STRING;
      break;

    case ROCI_OPCODE_PUSH_SYMBOL:
      *(state->stack++) = *(state->data_ptr++);
      *(state->stack_tags++) = ROCI_TAG_SYMBOL;
      break;

    case ROCI_OPCODE_BR_FALSE:
      roci_bb_t* taken_bb = cast(roci_bb_t*, state->data_ptr++);
      roci_tag_t tag = *(--state->stack_tags);
      uint64_t tos = *(--state->stack);
      if (tag != ROCI_TAG_BOOLEAN) {
        return ROCI_RUNTIME_ERROR_BOOLEAN_REQUIRED;
      }
      if (tos == 0) {
        bb = taken_bb;
        goto start_bblock;
      }
      break;

    case ROCI_OPCODE_BR:
      bb = cast(roci_bb_t*, *(state->data_ptr++));
      goto start_bblock;

    case ROCI_OPCODE_CALL:
      // HERE
      break;

    case ROCI_OPCODE_RETURN:
      bb = *(state->return_stack--);
      if (bb == NULL) {
        return ROCI_RUNTIME_ERROR_NONE;
      }
      goto start_bblock;

    case ROCI_OPCODE_NEW_ENVIRONMENT:
      state->env = roci_new_env(state->env);
      break;

    case ROCI_OPCODE_DEFINE_VAR: {
      char* str = cast(char*, state->data_ptr++);
      roci_tag_t tag = *(--state->stack_tags);
      uint64_t tos = *(--state->stack);
      roci_define_var(state->env, str, u64_to_value(tos), tag);
      break;
    }

    case ROCI_OPCODE_GET_VAR: {
      char* str = cast(char*, state->data_ptr++);
      roci_env_binding_t* binding = roci_get_var(state->env, str);
      *(state->stack++) = binding->value.u64;
      *(state->stack_tags++) = binding->tag;
      break;
    }

    case ROCI_OPCODE_SET_VAR: {
      char* str = cast(char*, state->data_ptr++);
      roci_tag_t tag = *(--state->stack_tags);
      uint64_t tos = *(--state->stack);
      roci_set_var(state->env, str, u64_to_value(tos), tag);
      break;
    }

    default:
      return ROCI_RUNTIME_ERROR_ILLEGAL_OPCODE;
    }
  }
  return ROCI_RUNTIME_ERROR_NONE;
}

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
