/**
 * @file roci-primitives.c
 *
 * Roci has a minimal set of primitives. Primitives consume their
 * arguments from the stack and *always* push a single value back onto
 * the stack. A set of helper functions help with manipulating the
 * stack.
 */

void roci_add_primitives_to_env(roci_env_t* env) {
  roci_add_primitive(env, &roci_primitive_load, "load");
  roci_add_primitive(env, &roci_primitive_exit, "exit");
  // TODO(jawilson): remove once roci is looking pretty good since the
  // roci repl understands #env which does the same thing and I don't
  // think we need regularly using this though I'm a bit undecided...
  roci_add_primitive(env, &roci_primitive_print_env, "debug_print_env");
  roci_add_primitive(env, &roci_primitive_to_string, "to_string");
  roci_add_primitive(env, &roci_primitive_print_string, "print_string");
  roci_add_primitive(env, &roci_primitive_println, "println");
  roci_add_primitive(env, &roci_primitive_string_append, "string_append");
  roci_add_primitive(env, &roci_primitive_string_equal, "string_equal");
  roci_add_primitive(env, &roci_primitive_string_starts_with,
                     "string_starts_with");
  roci_add_primitive(env, &roci_primitive_string_ends_with, "string_ends_with");
  roci_add_primitive(env, &roci_primitive_string_substring, "string_substring");
  roci_add_primitive(env, &roci_primitive_make_list, "make_list");
  roci_add_primitive(env, &roci_primitive_list_get, "list_get");
  roci_add_primitive(env, &roci_primitive_list_set, "list_set");
  roci_add_primitive(env, &roci_primitive_list_push, "list_push");
  roci_add_primitive(env, &roci_primitive_list_for_each, "list_for_each");
  roci_add_primitive(env, &roci_primitive_timestamp, "timestamp");
  roci_add_primitive(env, &roci_primitive_shell, "shell");
  roci_add_primitive(env, &roci_primitive_platform, "platform");
  roci_add_primitive(env, &roci_primitive_glob, "glob");
  roci_add_primitive(env, &roci_primitive_iadd, "iadd");
  roci_add_primitive(env, &roci_primitive_igte, "igte");
  roci_add_primitive(env, &roci_primitive_iequal, "iequal");
  roci_add_primitive(env, &roci_primitive_neg, "neg");
  roci_add_primitive(env, &roci_primitive_not, "not");
  roci_add_primitive(env, &roci_primitive_getenv, "getenv");
  roci_add_primitive(env, &roci_primitive_is_boolean, "is_boolean");
  roci_add_primitive(env, &roci_primitive_is_string, "is_string");
  roci_add_primitive(env, &roci_primitive_is_integer, "is_integer");
  roci_add_primitive(env, &roci_primitive_is_list, "is_list");
  roci_add_primitive(env, &roci_primitive_is_double, "is_double");
  roci_add_primitive(env, &roci_primitive_pwd, "pwd");
  roci_add_primitive(env, &roci_primitive_cd, "cd");
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
  value_array_t* bblocks = build_bblocks(compiler_state->bblocks);
  roci_bb_t* entry_point = value_array_get_ptr(bblocks, 0, typeof(roci_bb_t*));
  roci_execute(state->env, entry_point);
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

void roci_primitive_string_substring(roci_vm_state_t* state) {
  if (state->n_args != 3) {
    roci_debug_error(state, "string_substring expectds 3 arguments");
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
  }
  roci_push_false(state);
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
 * Excepts a list where the first element is the command and the reset
 * of the elements are arguments to that command.
 *
 * (This function is under flux, for example, it only returns stdout,
 * no error codes, etc.)
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

  buffer_t* stdout = make_buffer(1);
  buffer_t* stderr = stdout;
  do {
    sub_process_read(sub_process, stdout, stderr);
    usleep(5);
  } while (is_sub_process_running(sub_process));
  sub_process_read(sub_process, stdout, stderr);
  sub_process_wait(sub_process);

  roci_push_string(state, buffer_to_c_string(stdout));
}

void roci_primitive_platform(roci_vm_state_t* state) {
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

void roci_primitive_glob(roci_vm_state_t* state) {
  if (state->n_args != 1) {
    roci_debug_error(state, "glob expects 1 argument");
  }
  char* pattern = roci_pop_string(state);

  glob_t glob_result = compound_literal(glob_t, {0});

  // Find all .txt files in the current directory
  int return_value = glob(pattern, 0, NULL, &glob_result);
  value_array_t* result = make_value_array(state->n_args);
  if (return_value == 0) {
    for (int i = 0; i < glob_result.gl_pathc; ++i) {
      value_array_push(
          result, ptr_to_value(string_to_roci_string(glob_result.gl_pathv[i])));
    }
  } else if (return_value != GLOB_NOMATCH) {
    roci_debug_error(state, "An error occurred during globbing.");
  }
  roci_push_list(state, result);
}

void roci_primitive_iadd(roci_vm_state_t* state) {
  if (state->n_args != 2) {
    roci_debug_error(state, "iadd expects two integer arguments");
  }
  uint64_t arg1 = roci_pop_integer(state);
  uint64_t arg0 = roci_pop_integer(state);
  roci_push_integer(state, arg1 + arg0);
}

void roci_primitive_iequal(roci_vm_state_t* state) {
  if (state->n_args != 2) {
    roci_debug_error(state, "iequal expects two integer arguments");
  }
  uint64_t arg1 = roci_pop_integer(state);
  uint64_t arg0 = roci_pop_integer(state);
  if (arg0 == arg1) {
    roci_push_true(state);
  } else {
    roci_push_false(state);
  }
}

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

void roci_primitive_pwd(roci_vm_state_t* state) {
  if (state->n_args != 0) {
    roci_debug_error(state, "pwd expects 0 argument");
  }
  char cwd[PATH_MAX];
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

void roci_primitive_igte(roci_vm_state_t* state) {
  if (state->n_args != 2) {
    roci_debug_error(state, "igte expects two integer arguments");
  }
  uint64_t arg1 = roci_pop_integer(state);
  uint64_t arg0 = roci_pop_integer(state);
  roci_push_boolean(state, arg0 >= arg1);
}

void roci_primitive_neg(roci_vm_state_t* state) {
  if (state->n_args != 1) {
    roci_debug_error(state, "neg expects 1 argument");
  }
  roci_push_integer(state, -roci_pop_integer(state));
}
