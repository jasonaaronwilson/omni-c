/**
 * @file
 *
 * Roci is an interpreter for a "nano" Lisp/Scheme interpreter with an
 * intentionally minimal runtime (very limited I/O). It's intended to
 * be used as a configuration language for a cross platform build tool
 * which does what "ninja" doesn't.
 *
 * The tokenizer and parser adhere to C like syntax instead of a
 * traditional s-expression syntax.
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

  ROCI_OPCODE_NEW_ENVIRONMENT,
  ROCI_OPCODE_DEFINE_VAR,
  ROCI_OPCODE_GET_VAR,
  ROCI_OPCODE_SET_VAR,

  ROCI_OPCODE_BR_FALSE,
  ROCI_OPCODE_BR,

  ROCI_OPCODE_CALL_0,
  ROCI_OPCODE_CALL_1,
  ROCI_OPCODE_CALL_2,
  ROCI_OPCODE_CALL_3,
  ROCI_OPCODE_CALL_4,
  ROCI_OPCODE_CALL_5,
  ROCI_OPCODE_CALL_6,
  ROCI_OPCODE_CALL_7,
  ROCI_OPCODE_CALL_8,
  ROCI_OPCODE_CALL_9,
  ROCI_OPCODE_CALL_10,
  ROCI_OPCODE_CALL_11,
  ROCI_OPCODE_CALL_12,
  ROCI_OPCODE_CALL_13,
  ROCI_OPCODE_CALL_14,
  ROCI_OPCODE_CALL_15,
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
  roci_bb_t* return_stack;
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
 * Build and link all of the bblocks into the interpreter efficient
 * format and return the address of the first bblock.
 *
 * If there are no bblocks, then NULL is returned.
 *
 * This is slightly tricky to use and will evolve over time at which
 * time hopefully I will update the documentation.
 */
roci_bb_t* build_bblocks(roci_bb_builder_array_t* bblocks) { return NULL; }

/**
 * @function
 *
 * Once a file is compiled into bblocks, this should be used to run
 * that program fragment.
 */
roci_runtime_error_t roci_execute(roci_bb_t* entry_point) {
  // TODO(jawilson): take a roci_env_t* top_level_environment
  // TODO(jawilson): create a new roci_vm_state_t
  return ROCI_RUNTIME_ERROR_NONE;
}

/**
 * @function
 *
 */
void roci_runtime_error(roci_runtime_error_t runtime_error) {
  log_fatal("A runtime error has occurred evaluating a roci script");
  fatal_error(ERROR_ILLEGAL_STATE);
}

roci_runtime_error_t roci_execute_bblock(roci_bb_t* bb,
                                         roci_vm_state_t* state) {
 start_bblock:
  state->opcode_ptr = cast(uint8_t*, bb) + 8 + bb->num_data * 8;
  state->data_ptr = cast(uint64_t*, bb) + 1;
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
      break;

    default:
      return ROCI_RUNTIME_ERROR_ILLEGAL_OPCODE;
    }
  }
  return ROCI_RUNTIME_ERROR_NONE;
}
