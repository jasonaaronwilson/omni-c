/**
 * @file generic-calls
 *
 * Implements opt-in overloading of function calls with caveats:
 *
 * 1) you can't take the address of such a function yet. (Maybe this
 * does work if we pass in types or something?)
 * 2) all functions with the same name and arity must all be marked
 * "generic". This isn't compatible with names used in legacy
 * libraries.
 */

/*

First pass collects name+arity of all overloaded functions. complain
if ... in args list.

Second pass assembles the macro dispatchers for each function (and
renames these functions to include type names).

There isn't any other passes except to print out these dispatch tables
in a subsection of the macro area.

Original from LLM...

#define PAIR_TYPE(A, B) struct { typeof(A) a; typeof(B) b; }

#define add_custom(A, B) _Generic((PAIR_TYPE(A, B)){ (A), (B) }, \
    struct { int a; double b; }:       add_int_double,            \
    struct { double a; int b; }:       add_double_int,            \
    struct { Vector2 a; Vector2 b; }:  add_vec2_vec2              \
)((A), (B))

Though something 

#define GENERIC_DISPATCH_1(A) struct { typeof(A) a; }
#define GENERIC_DISPATCH_2(A, B) struct { typeof(A) a; typeof(B) b; }
#define GENERIC_DISPATCH_3(A, B, C) ...

#define add_custom(A, B) _Generic((PAIR_TYPE(A, B)){ (A), (B) }, \
    struct { int a; double b; }:       add_int_double,            \
    struct { double a; int b; }:       add_double_int,            \
    struct { Vector2 a; Vector2 b; }:  add_vec2_vec2              \
)((A), (B))

*/
