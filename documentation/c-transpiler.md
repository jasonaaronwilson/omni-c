# The C Transpiler

The C transpiler converts source files and libraries into standard C
which can then be compiled with suitable C compilers such as clang and
gcc (the transpiler will automatically produce a binary target unless
directed otherwise).

This document does not describe how to use the C transpiler, that is

## High Level Overview

1. the command line is parsed to determine compiler options and all
   source files are loaded into memory and placed into the files
   array.
1. parse all of the C ".h" and ".c" files to obtain the definitions
   they import and export to Omni C (it can place stuff into different
   namespaces!)
1. parse all of the Omni C files and further populate the namespaces
1. type checking, inference and annotation - every Omni C expression
   is given a type
1. closure conversion removes nested functions which C doesn't support
1. since we want to allow mutual dependencies between things in
   different namespaces, we need to generate a single header file
   (oc_definitions.h) that includes all of the union, struct, enums,
   (I guess including the stuff we got from C files - it seems C will
   ignore duplicate definitions).
1. generate C files for everything defined in Omni C. for all of the
   actual C files we have, they are required to already include
   oc_definitions.h). Simply write them to the build area. Eventually
   we may generate very specific headers for each file so that if any
   C file and the headers it depends on have not changed, then
   invoking the C compiler again can be skipped to speed up
   incremental builds.
1. invoke the C compiler on all of the C files (at least one per
   namespace) in parallel like make. then combine all of the .o files
   into a shared library, archive, or executable as requested).

Without any Omni C source files, the C transpiler is also a build tool
for pure C (and perhaps not a terrible one if a few additional steps
are taken).

Most of these steps are inherently parallel so eventually the compiler
will utilize as many compute threads as possible while still producing
determenistic results.

## C Code Generation

The c-transpiler must generate C code for all of the definitions in
the Omni C source files plus enough scaffolding to make them compile
(along with injecting things into C code that calls Omni C).

Many of the file level forms are pretty easy to generate. Structures,
especially generic ones, are more difficult and described
elsewhere. Lastly, functions require the most transformation in order
to support Omni C semantics. For example we need to perform closure
conversion since standard C does not understand closures.

## Omni C Implementation

The Omni C transpiler is currently written in C. We also considered
dynamic language like scheme but implementing in C may allows us to
eventually write the transpiler in Omni C itself with the least amount
of porting though this can't happen until the c-transpiler is
extremely robust.

The C and Omni C parsers are both tree-sitter based parsers. These are
known to be very fast and are very easy to use from C. Kudos to the
creator as well as the contributors to the tree-sitter project.

The reference grammar itself is the very same tree-sitter grammar used
by the c-transpiler though we may attempt to convert this to a more
standard BNF like syntax for documentation purposes.

Excluding the tree-sitter grammar and generated parser, we expect
expect the transpiler to be on the order of about 10K lines of code
excluding c-armyknife-lib which is kind of like a prototype for the
Omni C standard library.

Any additional backends will be written in Omni C and we can bootstrap
with the the c-transpiler to be able to compile them.
