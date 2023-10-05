# The C23 Transpiler

The C23 transpiler converts source files and libraries (which are simply the
concatention of one or more sources files) into C code conforming to the C23
standard which can then be compiled with suitable C compilers such as clang
and gcc.

## High Level Overview

1. the command line is parsed to determine a set of libraries and source files to compile
2. in parallel, all of the source files are parsed. this will require at least as much space
   as the source files and libraries themselves
3. we assemble namespaces by collecting each of the name space fragments
4. for each namespace we create the private and public top-level definitions
5. type checking and annotation - every expression is given a type
6. we move closures to the namespace level by having them take a struct pointer for
   captured variables, aka, an environment and replace with appropriate code at the
   lambda site
7. for each namespace in parallel, we output a C source file via an inorder treewalk
    of all functions in the namespace sorted by function name (for reproduceability)
8. for each namespace in parallel, collect all of the function prototype fragments, enum fragments,
    etc.
9. we typologically sort the header file. enumerations come first, structures (including environments)
   come next then prototypes  
10. invoke the C compiler (in parallel?) on all of the generated source files

## Function Code Generation
