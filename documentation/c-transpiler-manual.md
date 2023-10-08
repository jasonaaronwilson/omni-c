# Omni C Transpiler Manual

This document describes how to use a precompiled Omni C Transpiler.

## Command Line Arguments

The Omni C transpiler contains the functionality of multiple tools and
thus the command line always begins with a word describing what action
should be taken.

1. build -- build a binary or compiled library
1. install -- install a binary or shared library to standard locations
1. translate -- like build but doesn't actually invoke the C compiler
1. archive -- creates an OMNI archive from files or extracts files
   from an archive to the filesystem
1. debug -- invokes gdb or clang for a Omni C executable setting
   useful break-points such as on fatal-error and other conveniences
   allowed by known debuggers.

Next come GNU long style flags that should always have one of these
formats:

```
--flag-name=flag-value
--flag-name
```

Finally come "file" arguments which are any command line argument that
don't start with a "--".

Like other tools before the Omni C transpiler, "--" turns off flag
parsing for the remainder of the command line arguments. This seems
unnecessary right now but could become useful later and it is pretty
standard.

## No Short Flag Names

There are currently no short options defined for the Omni C
transpiler. While it is true this is mostly out of laziness, we also
expect most users to automate compilation via either `make` or another
build tool. Long flag names provide the same functionality but are
more readable and easier to Google search on.

## build flags

### --executable=<file-path>

Produce a standalone executable file by default.

### --omni-c-library=<file-path>

Produce an Omni C library. This is similar to what can be achieved by
using the archive command except that some basic analysis may be
performed to ensure that the library passes basic checks.

### --static-library=<file-path>

Produce a platform dependent ".a" file and a header file that will
allow this library to be used by C (or Omni C which could require some
extra stuff).

### --dynamic-library=<file-path>

Produce a platform dependent ".so" or ".dynlib: file and a header file
that will allow this library to be used by C (or Omni C which could
require some extra stuff).

### --debug=<LEVEL>

This controls what debug information is provided and may even change
the code generation strategy.

Initially supported "levels" are:

1. false -- turns off debug info in binaries to minimize size
1. true -- the default -- emit full debug information. This ensures
   that at least the `-g` flag is passed to the actual C compiler
   though other flags may be added as well. While debug information
   make binariers larger, this information doesn't usually change the
   run-time performance since debugging information in a binary is not
   typically loaded into memory when a resulting binary is executed.
1. enhanced -- not only do we build the binary with full debug
   information, the enhanced mode will insert run-time checks in Omni
   C code (but not C code) that detect other errors as earlier as
   possible. This may be similar to running tools such as
   valgrind. Enhanced debug mode will allow easily intercepting
   statements that throw exceptions, etc. since our compilation
   strategy makes these opaque to gdb and other debugger right
   now. Unlike "true", this will definitely *slow* your program down
   but that is usually worth it when a bug appears (which hopefully
   only happens in unsafe C code).

### --backend=<>

This flag will be used in the future to use a custom code generation
strategy for example to compile directly to "wasm" (so probably
clang), RISC-V, ARM64, or my comet-vm virtual machine.

### --additional-cc-flags=-O3 -lfoo

This flag will typically need to be "quoted" when you use it if there
are spaces in the flags list which may be common:

```bash
  omni-c build '--additional-cc-flags=-O3 -lfoo' file1.oc file2.c
```

## install

Install will *not* build a target as install typically requires "sudo"
prelidges and we don't want the rest of the compliation process to run
with enhanced priledges.

If you are distributing a library on github and want it to be
installed for common use, then you should try to make this work:

```
    omni-c build
    sudo omni-c install <binary-or-library-name>*
```

## translate

This performs all of the build steps except invoking the C compiler to
produce C binaries. This is normally used for debugging but can also
be used so that the final build process is under more control.

## archive

While there are safer ways to create an archive than this command,
this command provides a way to create, inspect, or extract library
archives.

## debug

debug invokes a debugger but also sets break-points at important
locations to stop the debugger when an error occurs before the program
terminates abnoramally.
