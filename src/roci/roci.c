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
  uint64_t bblock_number;
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

  // First allocate all of the bblocks before building the final
  // contents so we can "link" everything properly.
  for (int i = 0; i < bblocks->length; i++) {
    roci_bb_builder_t* builder
        = value_array_get_ptr(bblocks, i, typeof(roci_bb_builder_t*));
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

  return NULL;
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
      *(data_ptr++) = value_array_get(builder->data, dindex++).u64;
      break;

    case ROCI_OPCODE_BR_FALSE:
    case ROCI_OPCODE_BR:
      uint64_t bb_number = value_array_get(builder->data, dindex++).u64;
      *(data_ptr++) = bb_number_to_address(bblocks, bb_number);
      break;

    default:
      break;
    }
  }
}

uint64_t bb_number_to_address(roci_bb_builder_array_t* bblocks, int bb_number) {
  return cast(uint64_t, value_array_get_ptr(bblocks, bb_number,
                                            typeof(roci_bb_builder_t*))
                            ->bblock);
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

    default:
      return ROCI_RUNTIME_ERROR_ILLEGAL_OPCODE;
    }
  }
  return ROCI_RUNTIME_ERROR_NONE;
}
