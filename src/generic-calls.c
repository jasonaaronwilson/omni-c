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
*/

typedef generic_function_key_t = struct {
  char* function_name;
  int arity;
};

void lower_generic_functions(symbol_table_t* symbol_table) {
  value_hashtable_t* by_name_and_arity = find_generic_functions(symbol_table);
  symbol_table->generic_functions_map = by_name_and_arity;
  handle_generic_name_clashes(symbol_table);
}

void lower_generic_functions_array(value_array_t* array) {
  for (int i = 0; i < array->length; i++) {
    lower_generic_function(value_array_get_ptr(function_node_t*, array, i));
  }
}

void lower_generic_function(function_node_t* fn_node) {
  // Qencode the argument types into the functions name.
  // Remove generic from the declaration
}

// The associate value is an array of function nodes with the same
// function_name and arity.
value_hashtable_t* find_generic_functions(symbol_table_t* symbol_table) {
  return nullptr;
}

// Must be called after lower_generic_functions
void emit_generic_dispatchers(buffer_t buffer, value_hashtable_t* generic_calls) {
}

/*

#define GENERIC_DISPATCH_1(A) struct { typeof(A) a; }
#define GENERIC_DISPATCH_2(A, B) struct { typeof(A) a; typeof(B) b; }
#define GENERIC_DISPATCH_3(A, B, C) ...

#define add_custom(A, B) _Generic((GENERIC_DISPATCH_2(ARG_0, ARG_1)){ (ARG_0), (ARG_1) }, \
    struct { int a; double b; }:       add_int_double,            \
    struct { double a; int b; }:       add_double_int,            \
    struct { Vector2 a; Vector2 b; }:  add_vec2_vec2              \
)((ARG_0), (ARG_1))

*/

void emit_dispatcher(buffer_t* buffer, generic_function_key_t* key, value_array_t* function_nodes) {
  // Header
  // foreach
  // emit_dispatch_case(buffer_t* buffer, function_node_t* node);
  // Footer
}

//"    struct { double arg_0; int arg_1; }:       add_double_int,            \"
void emit_dispatch_case(buffer_t* buffer, function_node_t* node) {
}
