/**
 * @file roci-primitives.c
 *
 * Roci has a minimal set of primitives. Primitives consume their
 * arguments from the stack and *always* push a single value back onto
 * the stack. A set of helper functions help with manipulating the
 * stack.
 */

void roci_add_primitives_to_env(roci_env_t* env) {

  // General
  roci_add_primitive(env, &roci_primitive_debug_error, "debug_error");
  roci_add_primitive(env, &roci_primitive_equal, "equal");
  roci_add_primitive(env, &roci_primitive_hash, "hash");
  roci_add_primitive(env, &roci_primitive_eval, "eval");
  roci_add_primitive(env, &roci_primitive_apply, "apply");
  roci_add_primitive(env, &roci_primitive_command_line_args,
                     "command_line_args");
  roci_add_primitive(env, &roci_primitive_load, "load");
  roci_add_primitive(env, &roci_primitive_random_int, "random_int");
  roci_add_primitive(env, &roci_primitive_invoke_debugger, "invoke_debugger");

  // TODO(jawilson): remove once roci is looking pretty good since the
  // roci repl understands #env which does the same thing and I don't
  // think we need regularly using this though I'm a bit undecided...
  roci_add_primitive(env, &roci_primitive_print_env, "debug_print_env");

  roci_add_primitive(env, &roci_primitive_platform, "platform");

  // System
  roci_add_primitive(env, &roci_primitive_exit, "exit");
  roci_add_primitive(env, &roci_primitive_getenv, "getenv");
  roci_add_primitive(env, &roci_primitive_pwd, "pwd");
  roci_add_primitive(env, &roci_primitive_current_time_millis,
                     "current_time_millis");
  roci_add_primitive(env, &roci_primitive_cd, "cd");

  // IO
  roci_add_primitive(env, &roci_primitive_print_string, "print_string");
  roci_add_primitive(env, &roci_primitive_println, "println");
  roci_add_primitive(env, &roci_primitive_read_file, "read_file");
  roci_add_primitive(env, &roci_primitive_write_file, "write_file");
  roci_add_primitive(env, &roci_primitive_glob, "glob");
  roci_add_primitive(env, &roci_primitive_shell, "shell");
  roci_add_primitive(env, &roci_primitive_shell_command_line, "shell_command_line");
  roci_add_primitive(env, &roci_primitive_shell_exit_code, "shell_exit_code");
  roci_add_primitive(env, &roci_primitive_shell_stdout, "shell_stdout");
  roci_add_primitive(env, &roci_primitive_timestamp, "timestamp");
  roci_add_primitive(env, &roci_primitive_path_is_directory,
                     "path_is_directory");
  roci_add_primitive(env, &roci_primitive_path_is_file, "path_is_file");
  roci_add_primitive(env, &roci_primitive_path_remove, "path_remove");

  // String Functions
  roci_add_primitive(env, &roci_primitive_is_string, "is_string");
  roci_add_primitive(env, &roci_primitive_string_length_bytes,
                     "string_length_bytes");
  roci_add_primitive(env, &roci_primitive_string_append, "string_append");
  roci_add_primitive(env, &roci_primitive_string_equal, "string_equal");
  roci_add_primitive(env, &roci_primitive_string_starts_with,
                     "string_starts_with");
  roci_add_primitive(env, &roci_primitive_string_substring, "string_substring");
  roci_add_primitive(env, &roci_primitive_string_ends_with, "string_ends_with");
  roci_add_primitive(env, &roci_primitive_string_index_of, "string_index_of");
  roci_add_primitive(env, &roci_primitive_string_contains, "string_contains");
  roci_add_primitive(env, &roci_primitive_string_replace_all,
                     "string_replace_all");
  roci_add_primitive(env, &roci_primitive_string_get_byte, "string_get_byte");
  roci_add_primitive(env, &roci_primitive_to_string, "to_string");
  roci_add_primitive(env, &roci_primitive_ascii_to_string, "ascii_to_string");

  // List Functions
  roci_add_primitive(env, &roci_primitive_is_list, "is_list");
  roci_add_primitive(env, &roci_primitive_make_list, "make_list");
  roci_add_primitive(env, &roci_primitive_list_length, "list_length");
  roci_add_primitive(env, &roci_primitive_list_get, "list_get");
  roci_add_primitive(env, &roci_primitive_list_set, "list_set");
  roci_add_primitive(env, &roci_primitive_list_push, "list_push");
  roci_add_primitive(env, &roci_primitive_list_delete_at, "list_delete_at");
  roci_add_primitive(env, &roci_primitive_list_for_each, "list_for_each");

  // Integer Operations
  roci_add_primitive(env, &roci_primitive_is_integer, "is_integer");
  roci_add_primitive(env, &roci_primitive_neg, "neg");
  roci_add_primitive(env, &roci_primitive_iadd, "iadd");
  roci_add_primitive(env, &roci_primitive_isub, "isub");
  roci_add_primitive(env, &roci_primitive_imul, "imul");
  roci_add_primitive(env, &roci_primitive_idiv, "idiv");
  roci_add_primitive(env, &roci_primitive_irem, "irem");
  roci_add_primitive(env, &roci_primitive_ilt, "ilt");
  roci_add_primitive(env, &roci_primitive_ilte, "ilte");
  roci_add_primitive(env, &roci_primitive_iequal, "iequal");
  roci_add_primitive(env, &roci_primitive_igte, "igte");
  roci_add_primitive(env, &roci_primitive_igt, "igt");
  // Not as powerful as Python's range
  roci_add_primitive(env, &roci_primitive_for_each_integer, "for_each_integer");

  // Bitwise operations
  roci_add_primitive(env, &roci_primitive_bit_not, "bit_not");
  roci_add_primitive(env, &roci_primitive_bit_and, "bit_and");
  roci_add_primitive(env, &roci_primitive_bit_or, "bit_or");
  roci_add_primitive(env, &roci_primitive_bit_shr, "bit_shr");
  roci_add_primitive(env, &roci_primitive_bit_shl, "bit_shl");

  // Boolean operations
  roci_add_primitive(env, &roci_primitive_is_boolean, "is_boolean");
  roci_add_primitive(env, &roci_primitive_not, "not");
  roci_add_primitive(env, &roci_primitive_boolean_and, "boolean_and");
  roci_add_primitive(env, &roci_primitive_boolean_or, "boolean_or");
  /// non_empty?

  // Doubles
  roci_add_primitive(env, &roci_primitive_is_double, "is_double");

  // Buffers
  roci_add_primitive(env, &roci_primitive_is_buffer, "is_buffer");
  roci_add_primitive(env, &roci_primitive_make_buffer, "make_buffer");
  roci_add_primitive(env, &roci_primitive_buffer_get, "buffer_get");
  roci_add_primitive(env, &roci_primitive_buffer_length, "buffer_length");
  roci_add_primitive(env, &roci_primitive_buffer_append_string,
                     "buffer_append_string");
  roci_add_primitive(env, &roci_primitive_buffer_to_string, "buffer_to_string");

  // alists
  // hashtables
  // -- we implemented both in roci-lib.roci
  // -- obviously this will be slower then a native C implementation

  // Records
  roci_add_primitive(env, &roci_primitive_is_record, "is_record");
  roci_add_primitive(env, &roci_primitive_make_record, "make_record");
  roci_add_primitive(env, &roci_primitive_record_tag, "record_tag");
  roci_add_primitive(env, &roci_primitive_record_get, "record_get");
  roci_add_primitive(env, &roci_primitive_record_set, "record_set");


  // Random Testing Code
  roci_add_primitive(env, &roci_primitive_draw_random_screen,
                     "draw_random_screen");
  roci_add_primitive(env, &roci_primitive_fatal_error, "fatal_error");
}

/**
 * @function roci_add_primitive
 *
 * Utility function to add a primitive to a roci environment (usually
 * the top-level environment).
 */
void roci_add_primitive(roci_env_t* env, roci_c_primitive_t primitive,
                        char* name) {
  roci_define_var(env, name, u64_to_value(cast(uint64_t, primitive)),
                  ROCI_TAG_C_PRIMITIVE);
}

/**
 * @function roci_primitive_debug_error
 *
 * Until we have try/catch (or continuations), this is about the best
 * option for signaling errors from the part of the roci library
 * written in roci.
 *
 * A single argument is required, roughly what the failure or reason
 * is for calling the debugger.
 *
 * After the debugger is entered, we fail by exiting the entire
 * process with an exit code of 1.
 */
void roci_primitive_debug_error(roci_vm_state_t* state) {
  roci_debug_error(state, roci_pop_string(state));
  fprintf(
      stderr,
      "Exiting after user signaled error (debug_error is not continuable.)");
  exit(1);
}

void roci_primitive_equal(roci_vm_state_t* state) {
  if (state->n_args != 2) {
    roci_debug_error(state, "equal expects 2 argument");
  }
  roci_value_t arg2 = roci_pop_value(state);
  roci_value_t arg1 = roci_pop_value(state);
  roci_push_boolean(state, roci_values_equal(arg1, arg2));
}

boolean_t roci_values_equal(roci_value_t a, roci_value_t b) {
  if (a.tag == b.tag && a.raw == b.raw) {
    return true;
  }
  if (a.tag == ROCI_TAG_STRING && b.tag == ROCI_TAG_STRING) {
    return string_equal(cast(char*, a.raw), cast(char*, b.raw));
  }
  if (a.tag == ROCI_TAG_RECORD && b.tag == ROCI_TAG_RECORD) {
    roci_record_t* rec_a = cast(roci_record_t*, a.raw);
    roci_record_t* rec_b = cast(roci_record_t*, b.raw);
    if (!string_equal(rec_a->record_tag, rec_b->record_tag)) {
      return false;
    }
    if (rec_a->length != rec_b->length) {
      return false;
    }
    for (int i = 0; i < rec_a->length; i++) {
      roci_value_t va = roci_record_get(rec_a, i);
      roci_value_t vb = roci_record_get(rec_b, i);
      if (!roci_values_equal(va, vb)) {
        return false;
      }
    }
    return true;
  }

  // TODO(jawilson): ROCI_TAG_LIST, ROCI_TAG_BUFFER
  return false;
}

void roci_primitive_hash(roci_vm_state_t* state) {
  if (state->n_args != 1) {
    roci_debug_error(state, "hash expects 1 argument");
  }
  roci_value_t arg1 = roci_pop_value(state);
  roci_push_integer(state, roci_hash_value(state, arg1));
}

static inline uint64_t roci_mix64(uint64_t x) {
  x ^= x >> 30;
  x *= 0xbf58476d1ce4e5b9ULL;
  x ^= x >> 27;
  x *= 0x94d049bb133111ebULL;
  x ^= x >> 31;
  return x;
}

// Gemni Flash thinks that by using roci_mix64, we can get away with
// power of two growth which simplifies the slow code in the roci
// hashtable implementation.
int64_t roci_hash_value(roci_vm_state_t* state, roci_value_t value) {
  uint64_t mask = 0x7fffffffffffffffULL;
  uint64_t tag_bits = cast(uint64_t, value.tag) << 56;

  switch (value.tag) {

  case ROCI_TAG_UNKNOWN:
    break;

  case ROCI_TAG_BOOLEAN:
    return cast(int64_t, (roci_mix64(tag_bits | (value.raw ? 1 : 0)) & mask));

  case ROCI_TAG_INTEGER:
    return cast(int64_t, (roci_mix64(tag_bits ^ value.raw) & mask));

  case ROCI_TAG_DOUBLE: {
    // TODO(jawilson): Gemini Flash suggests normalizing -0.0 to
    // +0.0. For this to matter we must also normailize in the
    // standard equal function. Honestly, it is kind of crazy to hash
    // a double as a key to a hashtable so I'm not losing sleep right
    // now.
    uint64_t bits = double_as_uint64(value.raw);
    return cast(int64_t, (roci_mix64(tag_bits ^ bits) & mask));
  }

  case ROCI_TAG_STRING: {
    uint64_t str_hash
        = fasthash64(cast(char*, value.raw), strlen(cast(char*, value.raw)), 0);
    return cast(int64_t, (roci_mix64(tag_bits ^ str_hash) & mask));
  }

  case ROCI_TAG_CLOSURE:
  case ROCI_TAG_C_PRIMITIVE:
  case ROCI_TAG_LIST:
  case ROCI_TAG_BUFFER:
    return cast(int64_t, (roci_mix64(tag_bits ^ value.raw) & mask));

  case ROCI_TAG_RECORD: {
    roci_record_t* rec = cast(roci_record_t*, value.raw);
    int64_t hash = string_hash(rec->record_tag);
    for (int i = 0; i < rec->length; i++) {
      roci_value_t v = roci_record_get(rec, i);
      hash ^= roci_hash_value(state, v);
      hash = roci_mix64(hash);
    }
    return hash;
  }

  case ROCI_TAG_STACK_MARKER:
    break;
  }

  roci_debug_error(state, "unexpected roci value given to hash");

  return 0;
}

int eval_count = 0;

void roci_primitive_eval(roci_vm_state_t* state) {
  if (state->n_args != 1) {
    roci_debug_error(state, "eval expects 1 argument");
  }
  char* arg1 = roci_pop_string(state);

  roci_eval_result_t result = roci_eval_buffer(
      get_root_env(state->env), string_printf("*eval*%d", eval_count++),
      buffer_from_string(arg1), false);

  if (result.compiler_state->compiler_error != ROCI_COMPILE_TIME_ERROR_NONE) {
    roci_debug_error(state, "Compilation of a dynamically called eval failed");
  }

  roci_push_false(state);
}

void roci_primitive_apply(roci_vm_state_t* state) {
  if (state->n_args != 2) {
    roci_debug_error(state, "apply expects 2 argument");
  }
  value_array_t* list = roci_pop_list(state);
  roci_value_t proc = roci_pop_value(state);
  for (int i = 0; i < list->length; i++) {
    roci_value_t* element = cast(roci_value_t*, value_array_get(list, i).ptr);
    roci_push_value(state, *element);
  }
  state->n_args = list->length;
  roci_call(state, proc, list->length);
}

/**
 * @function roci_primitive_exit
 *
 * Exit the current process.
 */
void roci_primitive_exit(roci_vm_state_t* state) {
  if (state->n_args != 1) {
    roci_debug_error(state, "roci_exit expects 1 argument");
  }
  int64_t code = roci_pop_integer(state);
  exit(code);
}

/**
 * @function roci_primitive_load
 *
 * Load a roci file and interpret it (in the same environment that
 * load was defined in (i.e., the top-level environment).
 */
void roci_primitive_load(roci_vm_state_t* state) {
  if (state->n_args != 1) {
    roci_debug_error(state, "roci_load expects 1 argument");
  }
  char* filename = roci_pop_string(state);
  file_t* file = read_file(filename);
  roci_compiler_state_t* compiler_state = malloc_struct(roci_compiler_state_t);
  compiler_state->bblocks = make_value_array(16);
  roci_compile_buffer(compiler_state, file->file_name, file->data);
  if (compiler_state->compiler_error != ROCI_COMPILE_TIME_ERROR_NONE) {
    roci_debug_error(state, "roci_load experienced an error.");
    roci_push_false(state);
    return;
  }
  value_array_t* bblocks = build_bblocks(compiler_state->bblocks);
  roci_bb_t* entry_point = value_array_get_ptr(bblocks, 0, typeof(roci_bb_t*));
  roci_execute(roci_make_vm_state(state->env), entry_point);
  roci_push_false(state);
}

void roci_primitive_print_env(roci_vm_state_t* state) {
  buffer_t* buffer = make_buffer(10);
  roci_dump_env(state->env, buffer);
  fprintf(stdout, "%s", buffer_to_c_string(buffer));
  roci_push_false(state);
}

void roci_primitive_print_string(roci_vm_state_t* state) {
  if (state->n_args != 1) {
    roci_debug_error(state, "print_string expects a single string argument");
  }
  char* arg = roci_pop_string(state);
  fprintf(stdout, "%s", arg);
  roci_push_false(state);
}

void roci_primitive_println(roci_vm_state_t* state) {
  if (state->n_args != 1) {
    roci_debug_error(state, "println_string expects 1 argument");
  }
  roci_value_t element = roci_pop_value(state);
  if (element.tag == ROCI_TAG_STRING) {
    fprintf(stdout, "%s\n", cast(char*, element.raw));
  } else {
    fprintf(stdout, "%s\n", roci_value_to_c_string(element));
  }
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

void roci_primitive_string_index_of(roci_vm_state_t* state) {
  if (state->n_args != 2) {
    roci_debug_error(state, "string_index_of expects two string arguments");
  }
  char* arg1 = roci_pop_string(state);
  char* arg0 = roci_pop_string(state);
  roci_push_integer(state, string_index_of(arg0, arg1));
}

void roci_primitive_string_contains(roci_vm_state_t* state) {
  if (state->n_args != 2) {
    roci_debug_error(state, "string_contains expects two string arguments");
  }
  char* arg1 = roci_pop_string(state);
  char* arg0 = roci_pop_string(state);
  if (string_contains(arg0, arg1)) {
    roci_push_true(state);
  } else {
    roci_push_false(state);
  }
}

void roci_primitive_string_replace_all(roci_vm_state_t* state) {
  if (state->n_args != 3) {
    roci_debug_error(state, "string_replace_all expects two three arguments");
  }
  char* arg2 = roci_pop_string(state);
  char* arg1 = roci_pop_string(state);
  char* arg0 = roci_pop_string(state);
  roci_push_string(state, string_replace_all(arg0, arg1, arg2));
}

void roci_primitive_string_substring(roci_vm_state_t* state) {
  if (state->n_args != 3) {
    roci_debug_error(state, "string_substring expects 3 arguments");
  }
  int64_t end = roci_pop_integer(state);
  int64_t start = roci_pop_integer(state);
  char* str = roci_pop_string(state);
  char* result = string_substring(str, start, end);
  roci_push_string(state, result);
}

/**
 * @function string_append
 *
 * Appends zero or more string arguments returning their concatenation.
 */
void roci_primitive_string_append(roci_vm_state_t* state) {
  buffer_t* buffer = make_buffer(10);
  for (int64_t arg_num = 0; arg_num < state->n_args; arg_num++) {
    roci_value_t value = roci_debug_peek_value(state, state->n_args - arg_num);
    if (value.tag != ROCI_TAG_STRING) {
      roci_debug_error(state, "string_append expects string arguments");
    }
    buffer_append_string(buffer, cast(char*, value.raw));
  }
  for (int64_t arg_num = 0; arg_num < state->n_args; arg_num++) {
    roci_pop_value(state);
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
  for (int64_t arg_num = 0; arg_num < state->n_args; arg_num++) {
    roci_pop_value(state);
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

void roci_primitive_list_delete_at(roci_vm_state_t* state) {
  if (state->n_args != 2) {
    roci_debug_error(state, "list_delete_at expects 2 arguments");
  }
  int64_t index = roci_pop_integer(state);
  value_array_t* list = roci_pop_list(state);
  value_array_delete_at(list, index);
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
    roci_pop_value(state);
  }
  roci_push_false(state);
}

void roci_primitive_list_length(roci_vm_state_t* state) {
  if (state->n_args != 1) {
    roci_debug_error(state, "list_length expects 1 arguments");
  }
  value_array_t* list = roci_pop_list(state);
  roci_push_integer(state, list->length);
}

void roci_primitive_to_string(roci_vm_state_t* state) {
  if (state->n_args != 1) {
    roci_debug_error(state, "to_string expects 1 argument");
  }
  roci_value_t element = roci_pop_value(state);
  roci_push_string(state, roci_value_to_c_string(element));
}

void roci_primitive_timestamp(roci_vm_state_t* state) {
  if (state->n_args != 1) {
    roci_debug_error(state, "timestamp expects 1 argument");
  }
  char* filename = roci_pop_string(state);
  uint64_t timestamp = get_file_modification_time(filename);
  roci_push_integer(state, timestamp);
}

/**
 * @function shell
 *
 * Excepts a list where the first element is the command and the rest
 * of the elements are arguments to that command.
 *
 * Currently returns a list of the exit code stdout mixed in with
 * stdout.
 */
void roci_primitive_shell(roci_vm_state_t* state) {
  if (state->n_args != 1) {
    roci_debug_error(state, "shell expects 1 argument");
  }
  value_array_t* lst = roci_pop_list(state);
  uint64_t len = lst->length;
  value_array_t* argv = make_value_array(lst->length);
  for (int i = 0; i < len; i++) {
    roci_value_t* element = cast(roci_value_t*, value_array_get(lst, i).ptr);
    if (element->tag != ROCI_TAG_STRING) {
      roci_debug_error(state, "shell expects all list elements to be strings");
    }
    value_array_push(argv, str_to_value(cast(char*, element->raw)));
  }

  sub_process_t* sub_process = make_sub_process(argv);
  sub_process_launch(sub_process);

  buffer_t* stdout_buf = make_buffer(1);
  buffer_t* stderr_buf = stdout_buf;
  do {
    sub_process_read(sub_process, stdout_buf, stderr_buf);
    usleep(5);
  } while (is_sub_process_running(sub_process));
  sub_process_read(sub_process, stdout_buf, stderr_buf);
  sub_process_wait(sub_process);

  roci_push_list(state, lst);
  roci_push_integer(state, sub_process->exit_code);
  roci_push_string(state, buffer_to_c_string(stdout_buf));

  state->n_args = 3;
  roci_primitive_make_list(state);
}

void roci_primitive_shell_command_line(roci_vm_state_t* state) {
  if (state->n_args != 1) {
    roci_debug_error(state, "shell_command_line expects 1 argument");
  }
  roci_push_integer(state, 0);
  state->n_args = 2;
  roci_primitive_list_get(state);
}

void roci_primitive_shell_exit_code(roci_vm_state_t* state) {
  if (state->n_args != 1) {
    roci_debug_error(state, "shell_exit_code expects 1 argument");
  }
  roci_push_integer(state, 1);
  state->n_args = 2;
  roci_primitive_list_get(state);
}

void roci_primitive_shell_stdout(roci_vm_state_t* state) {
  if (state->n_args != 1) {
    roci_debug_error(state, "shell_stdout expects 1 argument");
  }
  roci_push_integer(state, 2);
  state->n_args = 2;
  roci_primitive_list_get(state);
}

void XXX_roci_primitive_platform(roci_vm_state_t* state) {
  if (state->n_args != 0) {
    roci_debug_error(state, "platform expects 0 argument");
  }
  roci_push_string(state, "linux");
  // omni-c doesn't support this kind of proprocessor stuff.
  /*
#if defined(__linux__)
roci_push_string(state, "linux");
#elif defined(__APPLE__)
  roci_push_string(state, "darwin");
#elif defined(_WIN32) || defined(_WIN64)
  roci_push_string(state, "windows");
#else
  #error "asdf"
  roci_push_string(state, "unknown");
#endif
  */
}

// AI rewrite now that we have file_glob
void roci_primitive_glob(roci_vm_state_t* state) {
    if (state->n_args != 1) {
        roci_debug_error(state, "glob expects 1 argument");
    }
    char* pattern = roci_pop_string(state);

    value_array_t* file_paths = file_glob(pattern);
    if (file_paths == NULL) {
        roci_debug_error(state, "An error occurred during globbing.");
    }

    size_t count = file_paths->length;
    value_array_t* result = make_value_array(count);

    for (size_t i = 0; i < count; ++i) {
      char* path = value_array_get(file_paths, i).str;
      value_array_push(result, ptr_to_value(string_to_roci_string(path)));
    }

    roci_push_list(state, result);
}

void roci_primitive_path_is_directory(roci_vm_state_t* state) {
  if (state->n_args != 1) {
    roci_debug_error(state, "path_is_directory expects 1 argument");
  }
  char* path = roci_pop_string(state);
  roci_push_boolean(state, path_is_directory(path));
}

void roci_primitive_path_is_file(roci_vm_state_t* state) {
  if (state->n_args != 1) {
    roci_debug_error(state, "path_is_file expects 1 argument");
  }
  char* path = roci_pop_string(state);
  roci_push_boolean(state, path_is_file(path));
}

void roci_primitive_path_remove(roci_vm_state_t* state) {
  if (state->n_args != 1) {
    roci_debug_error(state, "path_remove expects 1 argument");
  }
  char* path = roci_pop_string(state);
  if (remove(path) != 0) {
    roci_debug_error(state, string_append("File was not removed: ", path));
  }
  roci_push_false(state);
}

// Integer Operations

void roci_primitive_is_integer(roci_vm_state_t* state) {
  if (state->n_args != 1) {
    roci_debug_error(state, "is_integer expects 1 argument");
  }
  roci_value_t value = roci_pop_value(state);
  if (value.tag == ROCI_TAG_INTEGER) {
    roci_push_true(state);
  } else {
    roci_push_false(state);
  }
}

void roci_primitive_neg(roci_vm_state_t* state) {
  if (state->n_args != 1) {
    roci_debug_error(state, "neg expects 1 argument");
  }
  roci_push_integer(state, -roci_pop_integer(state));
}

void roci_primitive_iadd(roci_vm_state_t* state) {
  if (state->n_args != 2) {
    roci_debug_error(state, "iadd expects two integer arguments");
  }
  int64_t arg1 = roci_pop_integer(state);
  int64_t arg0 = roci_pop_integer(state);
  roci_push_integer(state, arg0 + arg1);
}

void roci_primitive_isub(roci_vm_state_t* state) {
  if (state->n_args != 2) {
    roci_debug_error(state, "isub expects two integer arguments");
  }
  int64_t arg1 = roci_pop_integer(state);
  int64_t arg0 = roci_pop_integer(state);
  roci_push_integer(state, arg0 - arg1);
}

void roci_primitive_imul(roci_vm_state_t* state) {
  if (state->n_args != 2) {
    roci_debug_error(state, "imul expects two integer arguments");
  }
  int64_t arg1 = roci_pop_integer(state);
  int64_t arg0 = roci_pop_integer(state);
  roci_push_integer(state, arg0 * arg1);
}

void roci_primitive_idiv(roci_vm_state_t* state) {
  if (state->n_args != 2) {
    roci_debug_error(state, "idiv expects two integer arguments");
  }
  int64_t arg1 = roci_pop_integer(state);
  int64_t arg0 = roci_pop_integer(state);
  roci_push_integer(state, arg0 / arg1);
}

void roci_primitive_irem(roci_vm_state_t* state) {
  if (state->n_args != 2) {
    roci_debug_error(state, "irem expects two integer arguments");
  }
  int64_t arg1 = roci_pop_integer(state);
  int64_t arg0 = roci_pop_integer(state);
  roci_push_integer(state, arg0 % arg1);
}

void roci_primitive_ilt(roci_vm_state_t* state) {
  if (state->n_args != 2) {
    roci_debug_error(state, "ilt expects two integer arguments");
  }
  int64_t arg1 = roci_pop_integer(state);
  int64_t arg0 = roci_pop_integer(state);
  roci_push_boolean(state, arg0 < arg1);
}

void roci_primitive_ilte(roci_vm_state_t* state) {
  if (state->n_args != 2) {
    roci_debug_error(state, "ilte expects two integer arguments");
  }
  int64_t arg1 = roci_pop_integer(state);
  int64_t arg0 = roci_pop_integer(state);
  roci_push_boolean(state, arg0 <= arg1);
}

void roci_primitive_iequal(roci_vm_state_t* state) {
  if (state->n_args != 2) {
    roci_debug_error(state, "iequal expects two integer arguments");
  }
  int64_t arg1 = roci_pop_integer(state);
  int64_t arg0 = roci_pop_integer(state);
  if (arg0 == arg1) {
    roci_push_true(state);
  } else {
    roci_push_false(state);
  }
}

void roci_primitive_igte(roci_vm_state_t* state) {
  if (state->n_args != 2) {
    roci_debug_error(state, "igte expects two integer arguments");
  }
  int64_t arg1 = roci_pop_integer(state);
  int64_t arg0 = roci_pop_integer(state);
  roci_push_boolean(state, arg0 >= arg1);
}

void roci_primitive_igt(roci_vm_state_t* state) {
  if (state->n_args != 2) {
    roci_debug_error(state, "igt expects two integer arguments");
  }
  int64_t arg1 = roci_pop_integer(state);
  int64_t arg0 = roci_pop_integer(state);
  roci_push_boolean(state, arg0 > arg1);
}

// Bitwise operations

void roci_primitive_bit_not(roci_vm_state_t* state) {
  if (state->n_args != 1) {
    roci_debug_error(state, "bit_not expects 1 argument");
  }
  uint64_t arg1 = roci_pop_integer(state);
  roci_push_integer(state, ~arg1);
}

void roci_primitive_bit_and(roci_vm_state_t* state) {
  if (state->n_args != 2) {
    roci_debug_error(state, "bit_and expects two integer arguments");
  }
  uint64_t arg1 = roci_pop_integer(state);
  uint64_t arg0 = roci_pop_integer(state);
  roci_push_integer(state, arg0 & arg1);
}

void roci_primitive_bit_or(roci_vm_state_t* state) {
  if (state->n_args != 2) {
    roci_debug_error(state, "bit_or expects two integer arguments");
  }
  uint64_t arg1 = roci_pop_integer(state);
  uint64_t arg0 = roci_pop_integer(state);
  roci_push_integer(state, arg0 | arg1);
}

void roci_primitive_bit_shl(roci_vm_state_t* state) {
  if (state->n_args != 2) {
    roci_debug_error(state, "bit_shl expects two integer arguments");
  }
  uint64_t arg1 = roci_pop_integer(state);
  uint64_t arg0 = roci_pop_integer(state);
  roci_push_integer(state, arg0 << arg1);
}

void roci_primitive_bit_shr(roci_vm_state_t* state) {
  if (state->n_args != 2) {
    roci_debug_error(state, "bit_shr expects two integer arguments");
  }
  uint64_t arg1 = roci_pop_integer(state);
  uint64_t arg0 = roci_pop_integer(state);
  roci_push_integer(state, arg0 >> arg1);
}

// Boolean not
void roci_primitive_not(roci_vm_state_t* state) {
  if (state->n_args != 1) {
    roci_debug_error(state, "roci_exit expects 1 argument");
  }
  boolean_t value = roci_pop_boolean(state);
  if (value) {
    roci_push_false(state);
  } else {
    roci_push_true(state);
  }
}

void roci_primitive_boolean_and(roci_vm_state_t* state) {
  if (state->n_args != 2) {
    roci_debug_error(state, "boolean_and expects 2 argument");
  }
  boolean_t arg2 = roci_pop_boolean(state);
  boolean_t arg1 = roci_pop_boolean(state);
  if (arg1 && arg2) {
    roci_push_true(state);
  } else {
    roci_push_false(state);
  }
}

void roci_primitive_boolean_or(roci_vm_state_t* state) {
  if (state->n_args != 2) {
    roci_debug_error(state, "boolean_or expects 2 argument");
  }
  boolean_t arg2 = roci_pop_boolean(state);
  boolean_t arg1 = roci_pop_boolean(state);
  if (arg1 || arg2) {
    roci_push_true(state);
  } else {
    roci_push_false(state);
  }
}

void roci_primitive_getenv(roci_vm_state_t* state) {
  if (state->n_args != 1) {
    roci_debug_error(state, "getenv expects 1 argument");
  }
  char* varname = roci_pop_string(state);
  char* result = getenv(varname);
  if (result == nullptr) {
    roci_push_false(state);
  } else {
    roci_push_string(state, result);
  }
}

void roci_primitive_is_boolean(roci_vm_state_t* state) {
  if (state->n_args != 1) {
    roci_debug_error(state, "is_boolean expects 1 argument");
  }
  roci_value_t value = roci_pop_value(state);
  if (value.tag == ROCI_TAG_BOOLEAN) {
    roci_push_true(state);
  } else {
    roci_push_false(state);
  }
}

void roci_primitive_is_string(roci_vm_state_t* state) {
  if (state->n_args != 1) {
    roci_debug_error(state, "is_string expects 1 argument");
  }
  roci_value_t value = roci_pop_value(state);
  if (value.tag == ROCI_TAG_STRING) {
    roci_push_true(state);
  } else {
    roci_push_false(state);
  }
}

void roci_primitive_string_length_bytes(roci_vm_state_t* state) {
  if (state->n_args != 1) {
    roci_debug_error(state, "is_string expects 1 argument");
  }
  char* str = roci_pop_string(state);
  roci_push_integer(state, strlen(str));
}

void roci_primitive_string_get_byte(roci_vm_state_t* state) {
  if (state->n_args != 2) {
    roci_debug_error(state, "string_get_byte expects 2 argument");
  }
  int64_t index = roci_pop_integer(state);
  char* str = roci_pop_string(state);
  int64_t limit = strlen(str);
  if (index < 0 || index >= limit) {
    roci_debug_error(state, "string_get_byte out of bounds");
  }
  roci_push_integer(state, (str[index]) & 0xff);
}


void roci_primitive_is_list(roci_vm_state_t* state) {
  if (state->n_args != 1) {
    roci_debug_error(state, "is_list expects 1 argument");
  }
  roci_value_t value = roci_pop_value(state);
  if (value.tag == ROCI_TAG_LIST) {
    roci_push_true(state);
  } else {
    roci_push_false(state);
  }
}

void roci_primitive_is_double(roci_vm_state_t* state) {
  if (state->n_args != 1) {
    roci_debug_error(state, "is_double expects 1 argument");
  }
  roci_value_t value = roci_pop_value(state);
  if (value.tag == ROCI_TAG_DOUBLE) {
    roci_push_true(state);
  } else {
    roci_push_false(state);
  }
}

void roci_primitive_current_time_millis(roci_vm_state_t* state) {
  if (state->n_args != 0) {
    roci_debug_error(state, "current_time_millis expects 0 argument");
  }
  roci_push_integer(state, current_time_millis());
}

void roci_primitive_random_int(roci_vm_state_t* state) {
  static random_state_t* rand_state = nullptr;

  if (state->n_args != 0) {
    roci_debug_error(state, "random expects 0 argument");
  }

  if (rand_state == nullptr) {
    rand_state = random_state();
  }

  roci_push_integer(state, random_next(rand_state));
}


void roci_primitive_pwd(roci_vm_state_t* state) {
  if (state->n_args != 0) {
    roci_debug_error(state, "pwd expects 0 argument");
  }
  char cwd[4096]; // PATH_MAX isn't define in windows?
  if (getcwd(cwd, sizeof(cwd)) != NULL) {
    roci_push_string(state, string_duplicate(cwd));
  } else {
    roci_debug_error(state, "getcwd return NULL");
  }
}

void roci_primitive_cd(roci_vm_state_t* state) {
  if (state->n_args != 1) {
    roci_debug_error(state, "cd expects 1 argument");
  }
  if (chdir(roci_pop_string(state))) {
    roci_push_false(state);
  } else {
    roci_debug_error(state, "chdir returned false");
  }
}

void roci_primitive_command_line_args(roci_vm_state_t* state) {
  value_array_t* list = make_value_array(8);
  roci_value_t value = compound_literal(roci_value_t, {0});
  value.tag = ROCI_TAG_STRING;
  for (int i = 0; i < FLAG_files->length; i++) {
    char* arg = value_array_get(FLAG_files, i).str;
    value.raw = cast(uint64_t, arg);
    value_array_add(list, ptr_to_value(roci_value_to_heap(value)));
  }
  roci_push_list(state, list);
}

// Buffers

void roci_primitive_is_buffer(roci_vm_state_t* state) {
  if (state->n_args != 1) {
    roci_debug_error(state, "is_buffer expects 1 argument");
  }
  roci_value_t value = roci_pop_value(state);
  if (value.tag == ROCI_TAG_BUFFER) {
    roci_push_true(state);
  } else {
    roci_push_false(state);
  }
}

void roci_primitive_make_buffer(roci_vm_state_t* state) {
  if (state->n_args != 0) {
    roci_debug_error(state, "make_buffer expects 0 arguments");
  }
  roci_push_buffer(state, make_buffer(8));
}

void roci_primitive_buffer_to_string(roci_vm_state_t* state) {
  if (state->n_args != 1) {
    roci_debug_error(state, "make_buffer expects 1 arguments");
  }
  buffer_t* buffer = roci_pop_buffer(state);
  roci_push_string(state, buffer_to_c_string(buffer));
}

void roci_primitive_read_file(roci_vm_state_t* state) {
  if (state->n_args != 1) {
    roci_debug_error(state, "read_file expects 1 argument");
  }
  char* filename = roci_pop_string(state);
  buffer_t* buffer = buffer_read_file(filename);
  roci_push_buffer(state, buffer);
}

void roci_primitive_write_file(roci_vm_state_t* state) {
  if (state->n_args != 2) {
    roci_debug_error(state, "write_file expects 2 argument");
  }
  char* filename = roci_pop_string(state);
  buffer_t* buffer = roci_pop_buffer(state);
  buffer_write_file(buffer, filename);
  roci_push_false(state);
}


void roci_primitive_buffer_get(roci_vm_state_t* state) {
  if (state->n_args != 2) {
    roci_debug_error(state, "buffer_get expects 2 argument");
  }
  int64_t position = roci_pop_integer(state);
  buffer_t* buffer = roci_pop_buffer(state);
  roci_push_integer(state, buffer_get(buffer, position) & 0xff);
}

void roci_primitive_buffer_append_string(roci_vm_state_t* state) {
  if (state->n_args != 2) {
    roci_debug_error(state, "buffer_append_string expects 2 argument");
  }
  char* str = roci_pop_string(state);
  buffer_t* buffer = roci_pop_buffer(state);
  buffer_append_string(buffer, str);
  roci_push_false(state);
}

void roci_primitive_buffer_length(roci_vm_state_t* state) {
  if (state->n_args != 1) {
    roci_debug_error(state, "buffer_length expects 1 argument");
  }
  buffer_t* buffer = roci_pop_buffer(state);
  roci_push_integer(state, buffer->length);
}

void roci_primitive_for_each_integer(roci_vm_state_t* state) {
  if (state->n_args != 3) {
    roci_debug_error(state, "for_each_integer expects 3 arguments");
  }
  roci_value_t proc = roci_pop_value(state);
  int64_t limit = roci_pop_integer(state);
  int64_t start = roci_pop_integer(state);
  for (int i = start; i < limit; i++) {
    roci_push_integer(state, i);
    roci_call(state, proc, 1);
    roci_pop_value(state);
  }
  roci_push_false(state);
}

void roci_primitive_ascii_to_string(roci_vm_state_t* state) {
  if (state->n_args != 1) {
    roci_debug_error(state, "ascii_to_string expects a single string argument");
  }
  uint64_t arg = roci_pop_integer(state);
  char* result = cast(char*, malloc_bytes(2));
  result[0] = arg & 0xff;
  result[1] = 0;

  roci_push_string(state, result);
}

void roci_primitive_invoke_debugger(roci_vm_state_t* state) {
  if (state->n_args != 1) {
    roci_debug_error(state, "invoke_debugger expects a single string argument");
  }
  roci_debug_error(state, roci_pop_string(state));
  roci_push_false(state);
}

void roci_primitive_is_record(roci_vm_state_t* state) {
  if (state->n_args != 1) {
    roci_debug_error(state, "is_record expects 1 argument");
  }
  roci_value_t value = roci_pop_value(state);
  roci_push_boolean(state, value.tag == ROCI_TAG_RECORD);
}

void roci_primitive_record_tag(roci_vm_state_t* state) {
  if (state->n_args != 1) {
    roci_debug_error(state, "record_tag expects 1 argument");
  }
  roci_record_t* record = roci_pop_record(state);
  roci_push_string(state, record->record_tag);
}

void roci_primitive_make_record(roci_vm_state_t* state) {
  if (state->n_args != 2) {
    roci_debug_error(state, "make_record expects 2 argument");
  }
  uint64_t length = cast(uint64_t, roci_pop_integer(state));
  char* record_tag = roci_pop_string(state);
  roci_push_value(state, roci_make_record(record_tag, length));
}

void roci_primitive_record_get(roci_vm_state_t* state) {
  if (state->n_args != 2) {
    roci_debug_error(state, "record_get expects 2 argument");
  }
  int64_t index = roci_pop_integer(state);
  roci_record_t* record = roci_pop_record(state);
  roci_push_value(state, roci_record_get(record, index));
}

void roci_primitive_record_set(roci_vm_state_t* state) {
  if (state->n_args != 3) {
    roci_debug_error(state, "record_set expects 3 argument");
  }
  roci_value_t value = roci_pop_value(state);
  int64_t index = roci_pop_integer(state);
  roci_record_t* record = roci_pop_record(state);
  roci_record_set(record, index, value);
  roci_push_false(state);
}

void roci_primitive_draw_random_screen(roci_vm_state_t* state) {
  boolean_t draw = roci_pop_boolean(state);
  draw_random_screen(draw);
  roci_push_false(state);
}

void roci_primitive_fatal_error(roci_vm_state_t* state) {
  if (state->n_args != 0) {
    roci_debug_error(state, "fatal_error expects zero arguments");
  }
  fatal_error(ERROR_FATAL);
  roci_push_false(state);
}
