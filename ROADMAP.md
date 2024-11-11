# Roadmap

This roadmap no longer matches the one in README.md.

# Phase One

The goal of phase one was to be able to toss a bunch of C files at
omni-c and have it generate a single output file that combines them
together eliminating the need for function prototypes, forward
declarations, etc. We also wanted to autogenerate code for things like
enum->string and string->enum.

Phase one is generally complete. We can now use omni-c to generate a
single file "self.c" that can be compiled with gcc/clang/tcc and that
version results in the same output file. omni-c makes heavy use of
c-armyknife-lib and that library is now generated omni-c (though it
still can use an older mode).

The big remaining issue appears to be that we don't properly reorder
static inline functions.

In terms of bootstrapping, we'd prefer to throw all of the files of
omni-c and c-armyknife-lib together at the same time to produce a
fully independent self.c file and use that for our "stable" binary. At
that point, we may be able to start removing certain legacy stuff like
prototypes, etc.

# Phase Two (non template features)

The goal of this phase is to fully parse everything inside of function
bodies, perform type checking/annotation, allow overloading even when
outputting C code, allow [] in expressions as syntatic sugar, allow
visibility annotations as well as automatically added prefixes for
"poorman's namespaces", and syntactic sugar for method call syntax,
etc.

# Phase Three (templates/macros/comptime)

The overall goal of this phase is to eliminate the need for the C
pre-processor.

# Phase Four (advanced features like exceptions, gc support, closures,
  etc.)
