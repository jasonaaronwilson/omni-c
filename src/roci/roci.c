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
  ROCI_TAG_LIST,
  // Temporary (for a while) hack so we can find the top of stack
  // while debugging.
  ROCI_TAG_STACK_MARKER,
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
  ROCI_OPCODE_DROP_ENVIRONMENT,
  ROCI_OPCODE_DEFINE_VAR,
  ROCI_OPCODE_GET_VAR,
  ROCI_OPCODE_SET_VAR,

  ROCI_OPCODE_BR_TRUE,
  ROCI_OPCODE_BR,

  ROCI_OPCODE_MAKE_CLOSURE,
  ROCI_OPCODE_RETURN,
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
  ROCI_OPCODE_CALL_16,
  ROCI_OPCODE_CHECK_ARGS,

  ROCI_OPCODE_DEBUG_INFO,
  ROCI_OPCODE_COMMENT,
};

typedef roci_runtime_error_t = enum {
  ROCI_RUNTIME_ERROR_NONE,
  ROCI_RUNTIME_ERROR_TRAP,
  ROCI_RUNTIME_ERROR_ILLEGAL_OPCODE,
  ROCI_RUNTIME_ERROR_BOOLEAN_REQUIRED,
};

typedef roci_closure_t = struct {
  roci_bb_t* entry_point;
  roci_env_t* env;
};

typedef roci_cont_t = struct {
  roci_bb_t* bb;
  roci_env_t* env;
  uint64_t* stack;
  uint8_t* stack_tags;
};

typedef roci_debug_state_t = struct {
  int64_t n_instructions; // Handles single stepping, etc.
  boolean_t break_on_call_target;
  boolean_t break_on_return;
  boolean_t break_on_next_statement;
  boolean_t trace;
};

typedef roci_vm_state_t = struct {
  roci_runtime_error_t runtime_error;
  roci_bb_t* current_bb;
  uint8_t* opcode_ptr;
  uint64_t* data_ptr;
  uint64_t* stack;
  uint8_t* stack_tags;
  roci_env_t* env;
  roci_cont_t** continuations;
  roci_debug_state_t* debug;
  uint64_t n_args;
  roci_src_info_t debug_info;
};

/**
 * @typedef roci_c_primitive_t
 *
 * Type of a function that takes roci_vm_state_t (so arguments on the
 * stack).
 */
typedef fn_t(void, roci_vm_state_t*) roci_c_primitive_t;

/**
 * @function roci_execute
 *
 * Once a file is compiled into bblocks, this should be used to run
 * that program fragment.
 */
roci_runtime_error_t roci_execute(roci_env_t* env, roci_bb_t* entry_point) {
  roci_vm_state_t* state = roci_make_vm_state(env);
  return roci_execute_bblock(entry_point, state);
}

roci_vm_state_t* roci_make_vm_state(roci_env_t* env) {
  roci_vm_state_t* state = malloc_struct(roci_vm_state_t);
  state->stack = cast(uint64_t*, malloc(256 * 8));
  state->stack_tags = cast(uint8_t*, malloc(256));
  state->continuations = cast(roci_cont_t**, malloc(256 * 8));
  roci_set_env(state, env);
  if (false) {
    state->debug = malloc_struct(roci_debug_state_t);
    state->debug->trace = true;
  }
  // This is a hack so that we can print all of the value of the stack
  // while debugging.
  roci_push_value_parts(state, 0xCAFEBABE, ROCI_TAG_STACK_MARKER);
  return state;
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
  buffer_t* buffer = make_buffer(80);

start_bblock:

  state->current_bb = bb;
  state->opcode_ptr = bblock_opcode_pointer(bb);
  state->data_ptr = bblock_data_pointer(bb);

  while (true) {
    if (state->debug != nullptr && state->debug->trace) {
      roci_debug_trace(state, buffer);
    }
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

    case ROCI_OPCODE_BR_TRUE:
      roci_bb_t* taken_bb = cast(roci_bb_t*, *(state->data_ptr++));
      roci_tag_t tag = *(--state->stack_tags);
      uint64_t tos = *(--state->stack);
      if (tag != ROCI_TAG_BOOLEAN) {
        return ROCI_RUNTIME_ERROR_BOOLEAN_REQUIRED;
      }
      if (tos != 0) {
        bb = taken_bb;
        goto start_bblock;
      }
      break;

    case ROCI_OPCODE_BR:
      bb = cast(roci_bb_t*, *(state->data_ptr++));
      goto start_bblock;

    case ROCI_OPCODE_MAKE_CLOSURE:
      roci_debug_breakpoint();
      // DATUM: function entry point bblock
      roci_closure_t* closure = malloc_struct(roci_closure_t);
      closure->entry_point = cast(roci_bb_t*, *(state->data_ptr++));
      closure->env = roci_current_env(state);
      roci_push_value_parts(state, cast(uint64_t, closure), ROCI_TAG_CLOSURE);
      roci_debug_breakpoint();
      break;

    case ROCI_OPCODE_CALL_0:
    case ROCI_OPCODE_CALL_1:
    case ROCI_OPCODE_CALL_2:
    case ROCI_OPCODE_CALL_3:
    case ROCI_OPCODE_CALL_4:
    case ROCI_OPCODE_CALL_5:
    case ROCI_OPCODE_CALL_6:
    case ROCI_OPCODE_CALL_7:
    case ROCI_OPCODE_CALL_8:
    case ROCI_OPCODE_CALL_9:
    case ROCI_OPCODE_CALL_10:
    case ROCI_OPCODE_CALL_11:
    case ROCI_OPCODE_CALL_12:
    case ROCI_OPCODE_CALL_13:
    case ROCI_OPCODE_CALL_14:
    case ROCI_OPCODE_CALL_15:
    case ROCI_OPCODE_CALL_16:
      // STACK: arg0, ..., argn, closure/primitive
      // DATUM: return-bblock
      uint64_t n_args = opcode - ROCI_OPCODE_CALL_0;
      state->n_args = n_args;
      roci_value_t proc = roci_pop_value(state);
      if (proc.tag == ROCI_TAG_C_PRIMITIVE) {
        roci_bb_t* cont_bb = cast(roci_bb_t*, *(state->data_ptr++));
        roci_env_t* cont_env = state->env;
        cast(roci_c_primitive_t, proc.raw)(state);
        bb = cont_bb;
        state->env = cont_env;
        goto start_bblock;
      } else if (proc.tag == ROCI_TAG_CLOSURE) {
        roci_closure_t* function = cast(roci_closure_t*, proc.raw);
        roci_set_env(state, function->env);
        roci_push_continuation(state, cast(roci_bb_t*, *(state->data_ptr++)),
                               n_args);
        bb = function->entry_point;
        goto start_bblock;
      } else {
        fatal_error(ERROR_ILLEGAL_STATE);
      }
      break;

    case ROCI_OPCODE_RETURN: {
      roci_value_t tos = roci_pop_value(state);
      roci_cont_t* continuation = roci_pop_continuation(state);
      bb = continuation->bb;
      state->stack = continuation->stack;
      state->stack_tags = continuation->stack_tags;
      roci_push_value_parts(state, tos.raw, tos.tag);
      if (bb == NULL) {
        return ROCI_RUNTIME_ERROR_NONE;
      }
      goto start_bblock;
    }

    case ROCI_OPCODE_NEW_ENVIRONMENT:
      roci_set_env(state, roci_new_env(roci_current_env(state)));
      break;

    case ROCI_OPCODE_DEFINE_VAR: {
      char* str = cast(char*, *(state->data_ptr++));
      roci_tag_t tag = *(--state->stack_tags);
      uint64_t tos = *(--state->stack);
      roci_define_var(roci_current_env(state), str, u64_to_value(tos), tag);
      break;
    }

    case ROCI_OPCODE_GET_VAR: {
      char* str = cast(char*, *(state->data_ptr++));
      roci_value_t* binding = roci_get_var(roci_current_env(state), str);
      if (binding == nullptr) {
        roci_debug_error(state, string_printf("variable not found: %s", str));
      }
      *(state->stack++) = binding->raw;
      *(state->stack_tags++) = binding->tag;
      break;
    }

    case ROCI_OPCODE_SET_VAR: {
      char* str = cast(char*, *(state->data_ptr++));
      roci_tag_t tag = *(--state->stack_tags);
      uint64_t tos = *(--state->stack);
      roci_set_var(state, str, u64_to_value(tos), tag);
      break;
    }

    case ROCI_OPCODE_COMMENT:
      state->data_ptr++;
      break;

    case ROCI_OPCODE_DEBUG_INFO:
      state->debug_info = *(state->data_ptr++);
      break;

    case ROCI_OPCODE_DROP_ENVIRONMENT:
      roci_drop_env(state);
      break;

    case ROCI_OPCODE_CHECK_ARGS: {
      uint64_t n_args = *(state->data_ptr++);
      if (n_args != state->n_args) {
        roci_debug_error(state, "argument call mismatch");
      }
      break;
    }

    case ROCI_OPCODE_DROP:
      roci_pop_value(state);
      break;

    default:
      return ROCI_RUNTIME_ERROR_ILLEGAL_OPCODE;
    }
  }
  return ROCI_RUNTIME_ERROR_NONE;
}

/**
 * This function allows a primitive to invoke another primitive or a
 * closure.
 */
void roci_call(roci_vm_state_t* state, roci_value_t proc, int64_t n_args) {
  state->n_args = n_args;
  if (proc.tag == ROCI_TAG_C_PRIMITIVE) {
    cast(roci_c_primitive_t, proc.raw)(state);
  } else if (proc.tag == ROCI_TAG_CLOSURE) {
    roci_push_continuation(state, nullptr, n_args);
    roci_closure_t* function = cast(roci_closure_t*, proc.raw);
    roci_set_env(state, function->env);
    roci_execute_bblock(function->entry_point, state);
  } else {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
}
