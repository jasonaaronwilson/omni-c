# Roadmap

This roadmap no longer matches the one in README.md.

# Phase One (basic source file conveniences)

1. implement header file creator (parse top-level stuff and just
   output .h)
1. add some auto-generated code like enum to string and back
   again.
1. eventually output a single .c and .h for all files in a
   library.

# Phase Two (non template features)

1. fully parse function bodies
1. compute types of everything
   * "auto" in C23 can be provided but more importantly we can query
     it's actual type from an editor, tool, etc.
1. overloaded functions
1. [] in expressions are syntactic sugure for
   index_get_operation/index_set_operation
1. method syntax
7. namespaces

# Phase Three (templates)

# Phase Four (advanced features like exceptions, gc support, closures,
  etc.)
