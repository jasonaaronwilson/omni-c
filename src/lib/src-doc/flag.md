# @file flag.c

A command line parser for flags (and the uninterpreted "file"
arguments, aka "left-over" arguments). We use a pragmatic
declarative configuration and unless you need to localize the
result, we also automatically generate the "usage" / "help"
documentation (via flag_print_help).

Here is maybe the smallest example you might possibly use:

```
  // For simple tools, just use "global" variaables but you can place
  // stuff wherever you choose, for example in a struct you can pass
  // around.

  boolean_t FLAG_verbose = true; // default value is true instead of
                                 // false in this case
  array_t* FLAG_file_args = NULL;

  flag_program_name("program-name");
  flag_boolean("--verbose", &FLAG_verbose);
  flag_file_args(&FLAG_files);

  char* error = flag_parse_command_line(argc, argv);
  if (error) {
    flag_print_help(stderr, error);
    exit(1);
  }
```

To make the auto generated help be the most useful, a human
readable description can be added to the last mentioned "thing" via
flag_description().

To make your program "easier" to use, you can also define aliases
for flags and ("sub-commands" explained below).

Another capability is using "sub-commands" so that your tool can be
a little more like "git", "apt", "yum" or "docker") versus more
traditional command line tools. We are a little more specific about
where the sub-command should be located in the command (it *must*
currently be in the first position). You can easily work around my
opinionated stance in several ways for example by "bubbling up"
anything that looks like one of your commands to the beginning of
the argument array passed to flag_parse_command_line. The
sub-command functionality is automatically enabled once you define
*any* sub-command. It's possible to create flags that only apply
when the correct sub-command is used.

Obviously flag_command() (just like flag_<type> can be called)
multiple times to define multiple "sub commands" using different
command names.

Aliases for flags and commands are also supported.

Besides the obvious basic types like integers and strings, it's
possible to add flags for enums (really just named integers with a
passed in sizeof) and eventually custom value parsers so that lists
and maps can be command line arguments. Want to parse a date? Even
though I'm not providing it, you can do it and catch errors before
something unexpected happens.

The interface presented here is *not* thread safe but generally a
program will define and parse command line arguments in the initial
thread before spawning other threads so this isn't a terrible
restriction.

Currently, flags and "left-over" arguments are not allowed to be
freely mixed. Once something that doesn't start with a "-" is seen,
all remaining arguments are treated as "left-over" arguments. The
special flag "--" can be used by the user to seperate things that
might otherwise be misinterpreted.

Note that when an error is returned, some of the "write backs" may
have been partially applied changing the default value they may
already contain. Unless you are running a test, the best option
will be to show help and exit to prevent continuing execution from
an undefined state.

I think adding a "switch" type may make sense since we currently
don't support flags like "-abCdefG" where each each letter is a
different flag alias nor do we support "--no-" long format which is
sometimes used to "negate" a switch.

TODO(jawilson): strict mode and custom parsers.
TODO(jawilson): allow stdin, stdout, and stderr deescriptions.
TODO(jawilson): allow limits on numeric arguments.
TODO(jawilson): allow "switches" which are like boolean flags but
don't require an argument. --bubble-gum, --no-bubble-gum.
 
## @function flag_description

Sets a description for the last "thing" "started".
 
## @function flag_enum

This should handle normal enums (which are represented in C as an
"int" when you use the enum to declare the type of the variable)
though an LLM is telling me that sometimes C will try to stuff
enum's into smaller types when they fit and I don't know enough to
say if this will be a problem.

You will need to cast the write_back_ptr to int* to call this
function without getting a warning with Clang and other
compilers. Since you will probably declare the flag only once but
use it multiple times, this extra cast may not matter too much. You
could consider using an explicitly size type like uint64_t or
int64_t instead to hold the enum value and essentially only use
"enum" as a convenient way to "define" constants.
 
## @function flag_file_args

Set where to write "left-over" arguments (which are usually file
paths, wild-cards, etc.) should be written. Any pointer to the
write back array is completely replaced with a new array so you
will typically initialize it to just NULL and everything just
works.

Unless you have a very good reason call this sometime after calling
flag_program_name but not before you start processing "children" of
the program node.
 
## @function flag_parse_command_line

Parses a command line writing back the actual parsed values so they
can be used after command line parsing.

When an error occurs, return a string with an error message OR NULL
if parsing was successfull.
 
## @function flag_print_help

Print help according to the flags and "sub-commands" that have been
defined.
 
## @function flag_program_name

Starts the command line configuration builder process and sets the
program name to be used in the automatically generated help string.
 
## @enum flag_type_t
 
