/**
 * @file roci.c
 *
 * Roci is an interpreter for a C syntax "nano" Lisp/Scheme
 * interpreter with an intentionally minimal runtime (very limited
 * I/O). It's intended to be used as a configuration language for a
 * cross platform build tool which does what "ninja" doesn't.
 *
 * The VM is also a little unique because objects in the heap are
 * stored without tags but the stack and environments track tags to
 * conservatively provide some dynmaic type checking.
 */

typedef roci_tag_t = enum {
  ROCI_TAG_UNKNOWN,
  ROCI_TAG_BOOLEAN,
  ROCI_TAG_INTEGER,
  ROCI_TAG_DOUBLE,
  ROCI_TAG_STRING,
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

/**
 * @function roci_execute
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
 * @function roci_runtime_error
 *
 */
void roci_runtime_error(roci_runtime_error_t runtime_error) {
  log_fatal("A runtime error has occurred evaluating a roci script");
  fatal_error(ERROR_ILLEGAL_STATE);
}

/**
 * @function roci_execute_bblock
 *
 * This is the real execution engine of the roci interpreter. It's
 * small size (less than 500 lines of code) suggests that we can make
 * it bullet proof - must bugs are going to be at the periphary
 * instead of here.
 */
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
