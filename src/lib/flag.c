#line 2 "flag.c"
/**
 * @file flag.c
 *
 * A command line parser for flags (and the uninterpreted "file"
 * arguments, aka "left-over" arguments). We use a pragmatic
 * declarative configuration and unless you need to localize the
 * result, we also automatically generate the "usage" / "help"
 * documentation (via flag_print_help).
 *
 * Here is maybe the smallest example you might possibly use:
 *
 * ```
 *   // For simple tools, just use "global" variaables but you can place
 *   // stuff wherever you choose, for example in a struct you can pass
 *   // around.
 *
 *   boolean_t FLAG_verbose = true; // default value is true instead of
 *                                  // false in this case
 *   array_t* FLAG_file_args = NULL;
 *
 *   flag_program_name("program-name");
 *   flag_boolean("--verbose", &FLAG_verbose);
 *   flag_file_args(&FLAG_files);
 *
 *   char* error = flag_parse_command_line(argc, argv);
 *   if (error) {
 *     flag_print_help(stderr, error);
 *     exit(1);
 *   }
 * ```
 *
 * To make the auto generated help be the most useful, a human
 * readable description can be added to the last mentioned "thing" via
 * flag_description().
 *
 * To make your program "easier" to use, you can also define aliases
 * for flags and ("sub-commands" explained below).
 *
 * Another capability is using "sub-commands" so that your tool can be
 * a little more like "git", "apt", "yum" or "docker") versus more
 * traditional command line tools. We are a little more specific about
 * where the sub-command should be located in the command (it *must*
 * currently be in the first position). You can easily work around my
 * opinionated stance in several ways for example by "bubbling up"
 * anything that looks like one of your commands to the beginning of
 * the argument array passed to flag_parse_command_line. The
 * sub-command functionality is automatically enabled once you define
 * *any* sub-command. It's possible to create flags that only apply
 * when the correct sub-command is used.
 *
 * Obviously flag_command() (just like flag_<type> can be called)
 * multiple times to define multiple "sub commands" using different
 * command names.
 *
 * Aliases for flags and commands are also supported.
 *
 * Besides the obvious basic types like integers and strings, it's
 * possible to add flags for enums (really just named integers with a
 * passed in sizeof) and eventually custom value parsers so that lists
 * and maps can be command line arguments. Want to parse a date? Even
 * though I'm not providing it, you can do it and catch errors before
 * something unexpected happens.
 *
 * The interface presented here is *not* thread safe but generally a
 * program will define and parse command line arguments in the initial
 * thread before spawning other threads so this isn't a terrible
 * restriction.
 *
 * Currently, flags and "left-over" arguments are not allowed to be
 * freely mixed. Once something that doesn't start with a "-" is seen,
 * all remaining arguments are treated as "left-over" arguments. The
 * special flag "--" can be used by the user to seperate things that
 * might otherwise be misinterpreted.
 *
 * Note that when an error is returned, some of the "write backs" may
 * have been partially applied changing the default value they may
 * already contain. Unless you are running a test, the best option
 * will be to show help and exit to prevent continuing execution from
 * an undefined state.
 *
 * I think adding a "switch" type may make sense since we currently
 * don't support flags like "-abCdefG" where each each letter is a
 * different flag alias nor do we support "--no-" long format which is
 * sometimes used to "negate" a switch.
 *
 * TODO(jawilson): strict mode and custom parsers.
 * TODO(jawilson): allow stdin, stdout, and stderr deescriptions.
 * TODO(jawilson): allow limits on numeric arguments.
 * TODO(jawilson): allow "switches" which are like boolean flags but
 * don't require an argument. --bubble-gum, --no-bubble-gum.
 */

#ifndef _FLAG_H_
#define _FLAG_H_

/**
 * @enum flag_type_t
 */
typedef enum {
  flag_type_none,
  flag_type_boolean,
  // TODO(jawilson): flag_type_switch,
  flag_type_string,
  flag_type_uint64,
  flag_type_int64,
  flag_type_double,
  flag_type_enum,
  flag_type_custom,
} flag_type_t;

struct program_descriptor_S {
  char* name;
  char* description;
  string_tree_t* flags;
  string_tree_t* commands;
  value_array_t** write_back_file_args_ptr;
};
typedef struct program_descriptor_S program_descriptor_t;

struct command_descriptor_S {
  program_descriptor_t* program;
  char* name;
  char* description;
  char** write_back_ptr;
  value_array_t** write_back_file_args_ptr;
  string_tree_t* flags;
};
typedef struct command_descriptor_S command_descriptor_t;

struct flag_descriptor_S {
  char* name;
  char* description;
  flag_type_t flag_type;
  char* help_string;
  void* write_back_ptr;
  int enum_size;
  string_tree_t* enum_values;
  // TODO(jawilson): add custom parser call back (and call back data).
};
typedef struct flag_descriptor_S flag_descriptor_t;

extern void flag_program_name(char* name);
extern void flag_description(char* description);
extern void flag_file_args(value_array_t** write_back_ptr);
extern void flag_command(char* name, char** write_back_ptr);

extern void flag_boolean(char* name, boolean_t* write_back_ptr);
extern void flag_string(char* name, char** write_back_ptr);
extern void flag_uint64(char* name, uint64_t* write_back_ptr);
extern void flag_int64(char* name, int64_t* write_back_ptr);
extern void flag_double(char* name, double* write_back_ptr);
extern void flag_enum(char* name, int* write_back_ptr);
extern void flag_enum_64(char* name, uint64_t* write_back_ptr);
extern void flag_enum_value(char* name, uint64_t value);
extern void flag_alias(char* alias);

// TODO(jawilson): flag_custom

extern char* flag_parse_command_line(int argc, char** argv);

extern void flag_print_help(FILE* out, char* error);

#endif /* _FLAG_H_ */

// Non exported data structures

struct flag_key_value_S {
  char* key;
  char* value;
};
typedef struct flag_key_value_S flag_key_value_t;

// Non exported function prototypes

command_descriptor_t* flag_find_command_descriptor(char* name);

flag_descriptor_t* flag_find_flag_descriptor(command_descriptor_t* command,
                                             char* name);

flag_key_value_t flag_split_argument(char* arg);

char* parse_and_write_value(flag_descriptor_t* flag,
                            flag_key_value_t key_value);

char* parse_and_write_boolean(flag_descriptor_t* flag,
                              flag_key_value_t key_value);

char* parse_and_write_uint64(flag_descriptor_t* flag,
                             flag_key_value_t key_value);

char* parse_and_write_enum(flag_descriptor_t* flag, flag_key_value_t key_value);

// Global Variables

program_descriptor_t* current_program;
command_descriptor_t* current_command;
flag_descriptor_t* current_flag;

/**
 * @function flag_program_name
 *
 * Starts the command line configuration builder process and sets the
 * program name to be used in the automatically generated help string.
 */
void flag_program_name(char* name) {
  current_program = malloc_struct(program_descriptor_t);
  current_program->name = name;
  current_command = NULL;
  current_flag = NULL;
}

/**
 * @function flag_program_name
 *
 * Starts the command line configuration builder process and sets the
 * program name to be used in the automatically generated help string.
 */
void flag_command(char* name, char** write_back_ptr) {
  current_command = malloc_struct(command_descriptor_t);
  current_command->name = name;
  current_command->write_back_ptr = write_back_ptr;
  current_flag = NULL;
  current_program->commands = string_tree_insert(
      current_program->commands, name, ptr_to_value(current_command));
}

/**
 * @function flag_description
 *
 * Sets a description for the last "thing" "started".
 */
void flag_description(char* description) {
  if (current_flag != NULL) {
    current_flag->description = description;
  } else if (current_command != NULL) {
    current_command->description = description;
  } else if (current_program != NULL) {
    current_program->description = description;
  } else {
    log_fatal("A current flag, program or command must be executed first");
    fatal_error(ERROR_ILLEGAL_STATE);
  }
}

/**
 * @function flag_file_args
 *
 * Set where to write "left-over" arguments (which are usually file
 * paths, wild-cards, etc.) should be written. Any pointer to the
 * write back array is completely replaced with a new array so you
 * will typically initialize it to just NULL and everything just
 * works.
 *
 * Unless you have a very good reason call this sometime after calling
 * flag_program_name but not before you start processing "children" of
 * the program node.
 */
void flag_file_args(value_array_t** write_back_file_args_ptr) {
  if (current_command != NULL) {
    current_command->write_back_file_args_ptr = write_back_file_args_ptr;
  } else if (current_program != NULL) {
    current_program->write_back_file_args_ptr = write_back_file_args_ptr;
  } else {
    log_fatal("A current program or command must be executed first");
    fatal_error(ERROR_ILLEGAL_STATE);
  }
}

// Place a flag in either the current_command or current_program. The
// name is passed in explicitly to allow aliases.
void add_flag(char* name, void* write_back_ptr, flag_type_t flag_type) {
  current_flag = malloc_struct(flag_descriptor_t);
  current_flag->flag_type = flag_type;
  current_flag->name = name;
  current_flag->write_back_ptr = write_back_ptr;

  // TODO(jawilson): check for a flag with the same name?
  if (current_command != NULL) {
    current_command->flags = string_tree_insert(current_command->flags, name,
                                                ptr_to_value(current_flag));
  } else if (current_program != NULL) {
    current_program->flags = string_tree_insert(current_program->flags, name,
                                                ptr_to_value(current_flag));
  } else {
    log_fatal("A current program or command must be executed first");
    fatal_error(ERROR_ILLEGAL_STATE);
  }
}

void flag_boolean(char* name, boolean_t* write_back_ptr) {
  add_flag(name, write_back_ptr, flag_type_boolean);
}

void flag_string(char* name, char** write_back_ptr) {
  add_flag(name, write_back_ptr, flag_type_string);
}

void flag_uint64(char* name, uint64_t* write_back_ptr) {
  add_flag(name, write_back_ptr, flag_type_uint64);
}

void flag_int64(char* name, int64_t* write_back_ptr) {
  add_flag(name, write_back_ptr, flag_type_int64);
}

void flag_double(char* name, double* write_back_ptr) {
  add_flag(name, write_back_ptr, flag_type_double);
}

/**
 * @function flag_enum
 *
 * This should handle normal enums (which are represented in C as an
 * "int" when you use the enum to declare the type of the variable)
 * though an LLM is telling me that sometimes C will try to stuff
 * enum's into smaller types when they fit and I don't know enough to
 * say if this will be a problem.
 *
 * You will need to cast the write_back_ptr to int* to call this
 * function without getting a warning with Clang and other
 * compilers. Since you will probably declare the flag only once but
 * use it multiple times, this extra cast may not matter too much. You
 * could consider using an explicitly size type like uint64_t or
 * int64_t instead to hold the enum value and essentially only use
 * "enum" as a convenient way to "define" constants.
 */
void flag_enum(char* name, int* write_back_ptr) {
  add_flag(name, write_back_ptr, flag_type_enum);
  current_flag->enum_size = sizeof(int) * 8;
}

void flag_enum_64(char* name, uint64_t* write_back_ptr) {
  add_flag(name, write_back_ptr, flag_type_enum);
  current_flag->enum_size = 64;
}

void flag_enum_value(char* name, uint64_t value) {
  if (!current_flag || current_flag->flag_type != flag_type_enum) {
    log_fatal("The current flag is not an enum type");
    fatal_error(ERROR_ILLEGAL_STATE);
  }

  current_flag->enum_values = string_tree_insert(current_flag->enum_values,
                                                 name, u64_to_value(value));
}

void flag_alias(char* alias) {
  if (current_flag != NULL) {
    // TODO(jawilson): check for a flag with the same name?
    if (current_command != NULL) {
      current_command->flags = string_tree_insert(current_command->flags, alias,
                                                  ptr_to_value(current_flag));
    } else if (current_program != NULL) {
      current_program->flags = string_tree_insert(current_program->flags, alias,
                                                  ptr_to_value(current_flag));
    } else {
      log_fatal("A current program or command must exist first");
      fatal_error(ERROR_ILLEGAL_STATE);
    }
  } else {
    log_fatal("A current flag must present to use flag_alias");
    fatal_error(ERROR_ILLEGAL_STATE);
  }
}

// TODO(jawilson): flag_type_switch,
// flag_type_custom,

/**
 * @function flag_parse_command_line
 *
 * Parses a command line writing back the actual parsed values so they
 * can be used after command line parsing.
 *
 * When an error occurs, return a string with an error message OR NULL
 * if parsing was successfull.
 */
char* flag_parse_command_line(int argc, char** argv) {
  if (current_program == NULL) {
    log_fatal(
        "flag_parse_command_line can't be called unless flag_program_name() is "
        "first called.");
    fatal_error(ERROR_ILLEGAL_STATE);
  }

  int start = 1;
  command_descriptor_t* command = NULL;
  if (current_program->commands) {
    if (argc <= 1) {
      return "This program requires a command but not enough arguments were "
             "given";
    }
    char* name = argv[1];
    command = flag_find_command_descriptor(name);
    if (command == NULL) {
      return string_printf(
          "The first command line argument is not a known command: %s", name);
    } else {
      *(command->write_back_ptr) = command->name;
    }
    start = 2;
  }

  value_array_t* files = make_value_array(argc);
  boolean_t parse_flags = true;

  for (int i = start; i < argc; i++) {
    char* arg = argv[i];
    if (parse_flags) {
      if (string_equal(arg, "--")) {
        parse_flags = false;
        continue;
      }

      if (string_starts_with(arg, "-")) {
        flag_key_value_t key_value = flag_split_argument(arg);
        if (key_value.key == NULL) {
          return string_printf("This argument is not a well formed flag: %s",
                               arg);
        }
        flag_descriptor_t* flag
            = flag_find_flag_descriptor(command, key_value.key);
        if (flag == NULL) {
          return string_printf(
              "The argument looks like a flag but was not found: '%s'\n\n"
              "(You may want to use ' -- ' to seperate flags from non flag "
              "arguments (aka file arguments).)",
              arg);
        }
        // If the value is NULL, that means there was no "=" sign
        // which means we should grab the next argument directly. When
        // the argument ends with a trailing "=", we get back an empty
        // string which is legal for purely string arguments but other
        // argument types will generaly error out during parsing.
        if (key_value.value == NULL) {
          // TODO(jawilson): bounds checking!
          i++;
          key_value.value = argv[i];
        }
        char* error = parse_and_write_value(flag, key_value);
        if (error) {
          return error;
        }
        continue;
      }
    }
    // Either it doesn't look like a flag or else we are no longer
    // parsing flags because we saw "--".
    value_array_add(files, str_to_value(arg));
  }

  // Write back the left-over arguments
  if (command != NULL && command->write_back_file_args_ptr != NULL) {
    *(command->write_back_file_args_ptr) = files;
  }
  if (current_program->write_back_file_args_ptr != NULL) {
    *(current_program->write_back_file_args_ptr) = files;
  }
  return NULL;
}

// Search the implicit "current_program" to see if anyone define
// "name" as a commmand.,
command_descriptor_t* flag_find_command_descriptor(char* name) {
  if (current_program->commands == NULL) {
    log_fatal(
        "flag_get_command() shouldn't not be called when we don't have any "
        "defined commands.");
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  value_result_t command_value
      = string_tree_find(current_program->commands, name);
  if (is_ok(command_value)) {
    return cast(command_descriptor_t*, command_value.ptr);
  } else {
    return NULL;
  }
}

// Search the command for a matching flag and return that first but if
// the command doesn't have a definition for flag, then the the
// "program" might have a definition for the flag so we also search
// for it there.
flag_descriptor_t* flag_find_flag_descriptor(/*nullable*/
                                             command_descriptor_t* command,
                                             char* name) {
  if (command != NULL) {
    value_result_t command_flag_value = string_tree_find(command->flags, name);
    if (is_ok(command_flag_value)) {
      return cast(flag_descriptor_t*, command_flag_value.ptr);
    }
  }

  value_result_t program_flag_value
      = string_tree_find(current_program->flags, name);
  if (is_ok(program_flag_value)) {
    return cast(flag_descriptor_t*, program_flag_value.ptr);
  }

  return NULL;
}

// The returned key will start with one or more "-" characters.
//
// BTW, we are way down in the call stack and are not prepared to
// properly deal with say "---", and the caller currently actually
// benefits from seeing either "-" or "--" prepended to the key so we
// don't even bother to detect if there are more than two dashes. If
// this is illegal, the key (aka flag) is not found and better error
// reporting will happen in the caller.
//
// This actually looks like it could be a useful addition to the
// library if it can be given a descriptive generic name. Split on
// first?
//
// TODO(jawilson): Nothing says that error handler couldn't do as part
// of fuzzy matching notice this and be more helpful as a special
// case. Long command lines get broken at weird places so that might
// be useful to look out for.
flag_key_value_t flag_split_argument(char* arg) {
  int equal_sign_index = string_index_of_char(arg, '=');
  if (equal_sign_index >= 0) {
    char* key = string_substring(arg, 0, equal_sign_index);
    // We know there is an "=". If nothing comes after it, we want to
    // set value to "" instead of NULL so that we don't try to process
    // the next argument. So --foo and --foo=, will *not* be treeated
    // the same way.
    char* value = string_substring(arg, equal_sign_index + 1, strlen(arg));
    return compound_literal(flag_key_value_t, {.key = key, .value = value});
  }
  return compound_literal(flag_key_value_t, {.key = arg, .value = NULL});
}

// Figure out what parser to use for the value, parse it, and then use
// the address in the flag descriptor to write the flag value to where
// the user requested.
char* parse_and_write_value(flag_descriptor_t* flag,
                            flag_key_value_t key_value) {
  switch (flag->flag_type) {
  case flag_type_boolean:
    return parse_and_write_boolean(flag, key_value);

  case flag_type_string:
    *cast(char**, flag->write_back_ptr) = key_value.value;
    return NULL;

  case flag_type_uint64:
    return parse_and_write_uint64(flag, key_value);

  case flag_type_enum:
    return parse_and_write_enum(flag, key_value);

  default:
    fatal_error(ERROR_ILLEGAL_STATE);
    break;
  }
  return "<ILLEGAL-STATE-NOT-REACHED>";
}

char* parse_and_write_boolean(flag_descriptor_t* flag,
                              flag_key_value_t key_value) {
  char* val = key_value.value;
  if (string_equal("true", val) || string_equal("t", val)
      || string_equal("1", val)) {
    *cast(boolean_t*, flag->write_back_ptr) = true;
  } else if (string_equal("false", val) || string_equal("f", val)
             || string_equal("0", val)) {
    *cast(boolean_t*, flag->write_back_ptr) = false;
  } else {
    return string_printf("boolean flag %s does not accept value %s",
                         key_value.key, key_value.value);
  }
  return NULL;
}

char* parse_and_write_uint64(flag_descriptor_t* flag,
                             flag_key_value_t key_value) {
  value_result_t val_result = string_parse_uint64(key_value.value);
  if (is_ok(val_result)) {
    *cast(uint64_t*, flag->write_back_ptr) = val_result.u64;
  } else {
    return string_printf("uint64_t flag %s does not accept value %s",
                         key_value.key, key_value.value);
  }
  return NULL;
}

char* parse_and_write_enum(flag_descriptor_t* flag,
                           flag_key_value_t key_value) {
  value_result_t val_result
      = string_tree_find(flag->enum_values, key_value.value);
  if (is_ok(val_result)) {
    switch (flag->enum_size) {
    case 64:
      // TODO(jawilson): switch on size, check upper bits, etc.
      *cast(uint64_t*, flag->write_back_ptr) = val_result.u64;
      return NULL;
    case 32:
      // TODO(jawilson): switch on size, check upper bits, etc.
      *cast(uint32_t*, flag->write_back_ptr) = val_result.u64;
      return NULL;

    default:
      fatal_error(ERROR_ILLEGAL_STATE);
      break;
    }
  }
  return string_printf("Flag %s does not accept the argument value %s",
                       key_value.key, key_value.value);

  // TODO(jawilson): allow specifying by value if allowed
  /*
  value_result_t val_result = string_parse_uint64(key_value.value);
  if (is_ok(val_result)) {
    *cast(uint64_t*, flag->write_back_ptr) = val_result.u64;
  } else {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  */
}

void flag_print_flags(FILE* out, char* header, string_tree_t* flags) {
  fprintf(out, "%s\n", header);
  // clang-format off
  string_tree_foreach(flags, key, value, block_expr({
      fprintf(out, "      %s\t%s\n", key, cast(flag_descriptor_t*, value.ptr)->description);
  }));
  // clang-format on
}

/**
 * @function flag_print_help
 *
 * Print help according to the flags and "sub-commands" that have been
 * defined.
 */
void flag_print_help(FILE* out, char* message) {
  fprintf(out, "\nMessage: %s\n", message);

  if (current_program == NULL) {
    fprintf(out,
            "Command line parsing was not configured so help can not be "
            "provided.");
    return;
  }

  if (current_program->commands != NULL) {
    fprintf(out, "\nUsage: %s <command> <flags> <files>\n",
            current_program->name);
    fprintf(out, "\nDescription: %s\n\n", current_program->description);

    flag_print_flags(out, "Global flags:", current_program->flags);

    fprintf(out, "\nCommands:\n");
    // clang-format off
    string_tree_foreach(current_program->commands, key, value, block_expr({
	fprintf(out, "\n    %s\t%s\n", key, cast(command_descriptor_t*, value.ptr)->description);
	flag_print_flags(out, "      Flags:", cast(command_descriptor_t*, value.ptr)->flags);
    }));
    // clang-format on
  } else {
    fprintf(out, "\nUsage: %s <flags> <files>\n", current_program->name);
    fprintf(out, "\nDescription: %s\n\n", current_program->description);
    flag_print_flags(out, "Flags:", current_program->flags);
  }
}
