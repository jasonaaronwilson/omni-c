#line 2 "value.c"

#ifndef _VALUE_H_
#define _VALUE_H_

#include <stdint.h>

/**
 * @file value.c
 *
 * A major part of the armyknife library is giving basic "collection"
 * capabilities to C.
 *
 * In an ideal world, C would allow structures to be parameterized so
 * that a field holding a "key" or a "value" could change size (based
 * on its static parameters).
 *
 * Since that capability doesn't currently exist, instead we take an
 * approach where keys and values are always the same size (64
 * bits). While this doesn't allow storing *anything* as a key or
 * value, it does allow storing a *pointer to anything* as a key or
 * value. This is actually very similar to how Java collections work
 * except we can store primitive values like integers and doubles
 * without "boxing".
 *
 * When "searching" a collection for a key, we want to be able to
 * return "not found" (and potentially other "non-fatal" error
 * conditions). For this reason we also have the value_result_t
 * structure, which is a pair of a value_t and a
 * non_fatal_error_code_t. (It's actually slightly more complicated
 * because `tcc` treats anonymous unions a little differently than
 * `gcc` and `clang` so we work around that by repeating the fields of
 * a value to make value_result_t a bit more convenient to work with).
 *
 * Sometimes value_result_t is also used by non collection based
 * functions, such as parsing an integer, so that other non-fatal
 * errors can be communicated back to the caller.
 *
 * The contract with returning a non-fatal errors is that the state of
 * the system is in a good state to continue processing (or they get
 * to decide if the error should be fatal). Users will thus expect
 * that when a non-fatal error is returned that no global state
 * modification has ocurred.
 *
 * value_t's are typically only passed into functions while
 * optional_value_result_t's are typically returned from functions.
 *
 * When a value_result_t is returned you must always check for an
 * error code before using the value component of the result. `is_ok`
 * and `is_not_ok` make this slightly easier and easier to read.
 *
 * value_t's and value_result_t's carry no type information that can
 * be queried at runtime and by their nature C compilers are going to
 * do a very incomplete job of statically type checking these. For
 * example you can easily put a double into a collection and
 * successfully get back this value and use it as a very suspicious
 * pointer and the compiler will not warn you about this. So
 * collections aren't as safe as in other languages at either compile
 * or run-time. (Java's collections (when generic types are *not*
 * used), are not "safe" at compile time but are still dynamically
 * safe.)
 *
 * On the positive side, the lack of dynamic typing means you haven't
 * paid a price to maintain these and in theory your code can run
 * faster.
 *
 * If C had a richer type-system, namely generic types, we could catch
 * all all potential type errors at compile time and potentially even
 * allow storing "values" larger than 64bits "inline" with a little
 * more magic.
 *
 * The most common things to use as keys are strings, integers, and
 * pointers (while common association values are strings, integers,
 * pointers and booleans).
 *
 * Our primary goal is to make collections convenient. Using typedef
 * and inline functions you can also make these safer at compile time.
 */

/**
 * @typedef value_t
 *
 * An un-typed union of integers, doubles, and pointers.
 */
typedef union {
  uint64_t u64;
  uint64_t i64;
  char* str;
  void* ptr;
  void* dbl;
} value_t;

/**
 * @enum non_fatal_error_code_t
 *
 * These are user recoverable errors and when a non-recoverable error
 * is returned, the state of the system should be left in a
 * recoverable state.
 */
typedef enum {
  NF_OK,
  NF_ERROR_NOT_FOUND,
  NF_ERROR_NOT_PARSED_AS_NUMBER,
  NF_ERROR_NOT_PARSED_AS_EXPECTED_ENUM,
} non_fatal_error_code_t;

/**
 * @typedef value_result_t
 *
 * A pair of a value_t and a non-fatal error code. When the error code
 * is set, the value_t should never be looked at (most likely will be
 * "zero" or a "nullptr" but you shouldn't trust that).
 */
typedef struct {
  union {
    uint64_t u64;
    int64_t i64;
    double dbl;
    char* str;
    void* ptr;
    value_t val;
  };
  // TODO(jawilson): change the name of the field after checking if
  // the compiler helps at all here!
  non_fatal_error_code_t nf_error;
} value_result_t;

// TODO(jawilson): we can use _Generic for this and just use
// to_value()

#define boolean_to_value(x) compound_literal(value_t, {.u64 = x})
#define u64_to_value(x) compound_literal(value_t, {.u64 = x})
#define i64_to_value(x) compound_literal(value_t, {.i64 = x})
#define str_to_value(x) compound_literal(value_t, {.str = x})
#define ptr_to_value(x) compound_literal(value_t, {.ptr = x})
#define dbl_to_value(x) compound_literal(value_t, {.dbl = x})

/**
 * @function is_ok
 *
 * Return true if the given value_result_t contains a legal value
 * instead of an error condition.
 */
static inline boolean_t is_ok(value_result_t value) {
  return value.nf_error == NF_OK;
}

/**
 * @function is_not_ok
 *
 * Return true if the given value_result_t contains an error, such as
 * NF_ERROR_NOT_FOUND.
 */
static inline boolean_t is_not_ok(value_result_t value) {
  return value.nf_error != NF_OK;
}

/**
 * @typedef value_comparison_fn
 *
 * A type for a function pointer which will compare two values,
 * returning -1 when value1 < value2, 0 when value1 == value2, and 1
 * when value1 > value2.
 */
// typedef int (*value_comparison_fn)(value_t value1, value_t value2);
typedef fn_t(int, value_t value1, value_t value2) value_comparison_fn;

/**
 * @typedef value_hash_fn
 *
 * A type for a function pointer which will hash it's value_t to a
 * uint64_t.
 */
// typedef uint64_t (*value_hash_fn)(value_t value1);
typedef fn_t(uint64_t, value_t value1) value_hash_fn;


int cmp_string_values(value_t value1, value_t value2);
uint64_t hash_string_value(value_t value1);

#endif /* _VALUE_H_ */

/**
 * @function cmp_string_values
 *
 * Assumes value1 and value2 are char* (aka strings or C strings) and
 * does the equivalent of strcmp on them.
 */
int cmp_string_values(value_t value1, value_t value2) {
  return strcmp(value1.str, value2.str);
}

/**
 * @function string_hash
 *
 * Assumes value1 is char* and performs a string hash on it.
 */
uint64_t hash_string_value(value_t value1) { return string_hash(value1.str); }
