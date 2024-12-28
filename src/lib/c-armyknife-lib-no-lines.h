/*
 * Copyright (c) 2024, Jason A. Wilson
 *
 * The project documentation, source code, tests, and the full open
 * source license is available at:
 *
 * https://github.com/jasonaaronwilson/c-armyknife-lib/
 * 
 * This file itself is automatically generated from about 18 or more
 * source files and is "packaged" into this particular format
 * sometimes known as a "header only" libray (see
 * https://en.wikipedia.org/wiki/Header-only) so we don't recommend
 * editing this file by hand. Other ways of using or obtaining this
 * library will be documented in the c-armyknife-lib repository
 * including instructions for turning off some included functionality
 * to save space.
 *
 * To actually use this library, one ".c" file in your project must
 * include this file, (typically called
 * c-armyknife-lib-<<version-string>>.h) after *first* defining
 * C_ARMYKNIFE_LIB_IMPL otherwise it only provides the prototypes,
 * inlined functions, macros, and type definitions, like you would
 * normally see in a C header file and your program will not link
 * correctly.
 */

// SSCF generated file from: omni-c.c

#ifndef _OMNI_C_H_
#define _OMNI_C_H_

/**
 * @file omni-c.c
 *
 * These macros help c-armyknife-lib conform to what omni-c can
 * (currently) handle. If you are writing code within c-armyknife-lib,
 * then you must use these macros but if you are just using
 * c-amryknife-lib within your own C project, then you can ignore
 * these macros unless you strictly want to use them.
 */

/**
 * @macro cast
 *
 * Perform an unsafe cast of expr to the given type. This is much
 * easier for the omni-c compiler to handle because we know to parse a
 * type as the first argument even if we don't know what all the types
 * are yet. While omni-c uses this macro, if you are using plain C,
 * you don't have to use it.
 */
#define cast(type, expr) ((type) (expr))

/**
 * @macro block_expr
 *
 * The omni-c compiler isn't smart enough to handle a statement block
 * passed to a macro yet but marking such blocks with this macro makes
 * it much easier to handle for now.
 */
#define block_expr(block) block

#endif /* _OMNI_C_H_ */
// SSCF generated file from: min-max.c

#ifndef _MIN_MAX_H_
#define _MIN_MAX_H_

/**
 * @macro min
 *
 * Produce the minimum of a and b. Prior to C23, we evalutate one of
 * the arguments twice but we eventually hope to evaluate each
 * argument only once and use the "auto" argument so that the macro
 * works for any type that can be compared with <.
 */
#define min(a, b) ((a) < (b) ? (a) : (b))

/**
 * @macro max
 *
 * Produce the maximum of a and b. Prior to C23, we evalutate one of
 * the arguments twice but we eventually hope to evaluate each
 * argument only once and use the "auto" argument so that the macro
 * works for any type that can be compared with >.
 */
#define max(a, b) ((a) > (b) ? (a) : (b))

#endif /* _MIN_MAX_H_ */
// SSCF generated file from: boolean.c

#ifndef _BOOLEAN_H_
#define _BOOLEAN_H_

#include <stdbool.h>

/**
 * @type boolean_t
 *
 * This is a simple typedef for "bool" (or _Bool from C99) which is
 * available from "stdbool.h" as bool and true and false constants are
 * also defined. We use it for more consistency in primitive types
 * (where only char* is commonly used in this library despite not
 * following the typically naming convention).
 */
typedef bool boolean_t;

// #define true ((boolean_t) 1)
// #define false ((boolean_t) 0)

#endif /* _BOOLEAN_H_ */
// SSCF generated file from: compound-literal.c

#ifndef _COMPOUND_LITERAL_H_
#define _COMPOUND_LITERAL_H_

#define compound_literal(type, ...) ((type) __VA_ARGS__)

#endif /* _COMPOUND_LITERAL_H_ */
// SSCF generated file from: fn.c

#ifndef _FN_H_
#define _FN_H_

/**
 * @file fn.c
 *
 * C's declarator syntax and the spiral rule are sometimes very hard
 * to read and write so fn_t enters the room.
 *
 * Before:
 *
 * ```
 * typedef uint64_t (*value_hash_fn)(value_t value);
 * ```
 *
 * After:
 * ```
 * typedef fn_t(uint64_t, value_t value) value_hash_fn;
 * ```
 *
 * Notice how the name of the type now appears all the way to the
 * right.
 *
 * (Omni C will eventually allow "=" for typedefs so:
 *
 * ```
 * typedef value_hash_fn = fn_t(uint64_t, value_t value);
 * ```
 *
 * I asked an LLM to help make my point and it suggested this
 * function prototype as a confusing case:
 *
 * ```
 * void (*signal(int sig, void (*func)(int)))(int);
 * ```
 *
 * This really is exteremely difficult to read. It's a function that
 * both accepts and returns a signal handler where a signal handler is
 * a function that takes an int and return's void. Even with that
 * clue, the above is pretty hard to read, while this is hopefully not
 * that difficult to read.
 *
 * ```
 * fn_t(void, int) signal(int sig, fn_t(void, int) handler);
`* ```
 */

#define fn_t(return_type, ...) typeof(return_type(*)(__VA_ARGS__))

#endif /* _FN_H_ */
// SSCF generated file from: leb128.c

#ifndef _LEB128_H_
#define _LEB128_H_

/**
 * @file leb128.c
 *
 * ULEB-128 (unsigned) and SLEB-128 (signed, aka, possibly negative
 * integers) are variable length encodings for possibly "very large"
 * numbers. I first came across LEB while working with "DWARF2" in in
 * the 1990s. Google uses the same encoding for unsigned integers but
 * uses "zig-zag" for signed numbers (the sign bit is stored in the
 * lowest bit instead of the more complicated technique SLEB-128
 * uses).
 *
 * Essentially the 8th bit of each byte says whether to continue
 * adding bits from the next byte or if the number is now "complete"
 * (so 7-bits per byte are the real data and 1-bit is
 * "overhead"). Since many numbers we deal with are actually small,
 * LEB encoding tend to be an efficient encoding of integers and grow
 * "gracefully" to accomodate larger numbers. In fact the "128" in
 * ULEB-128 is a misnomer -- you could in theory encode more than 128
 * bits. This implementation only reads 64bit numbers, not the full
 * 128 bits.
 *
 * This implementation originally comes from LLVM although I have
 * changed it to a C file, renamed the functions, got rid of camel
 * case, removed the C++ namespace, removed the inline directive,
 * removed the pad to argument and changed how results and errors are
 * returned.
 */

#include <stdint.h>

#define ERROR_INSUFFICIENT_INPUT -1
#define ERROR_TOO_BIG -2

typedef struct {
  uint64_t number;
  // Negative values mean an error occurred.
  int size;
} unsigned_decode_result;

typedef struct {
  uint64_t number;
  // Negative values mean an error occurred.
  int size;
} signed_decode_result;

extern unsigned encode_sleb_128(int64_t Value, uint8_t* p);
extern unsigned encode_uleb_128(uint64_t Value, uint8_t* p);
extern unsigned_decode_result decode_uleb_128(const uint8_t* p,
                                              const uint8_t* end);
extern signed_decode_result decode_sleb_128(const uint8_t* p,
                                            const uint8_t* end);

#endif /* _LEB128_H_ */
// SSCF generated file from: fatal-error.c

#ifndef _FATAL_ERROR_H_
#define _FATAL_ERROR_H_

struct fatal_error_config_S {
  boolean_t catch_sigsegv;
};

typedef struct fatal_error_config_S fatal_error_config_t;

/**
 * @constants error_code_t
 */
typedef enum {
  ERROR_UKNOWN,
  ERROR_SIGSEGV,
  ERROR_ACCESS_OUT_OF_BOUNDS,
  ERROR_BAD_COMMAND_LINE,
  ERROR_DYNAMICALLY_SIZED_TYPE_ILLEGAL_IN_CONTAINER,
  ERROR_ILLEGAL_ENUM_VALUE,
  ERROR_ILLEGAL_INITIAL_CAPACITY,
  ERROR_ILLEGAL_NULL_ARGUMENT,
  ERROR_ILLEGAL_ZERO_HASHCODE_VALUE,
  ERROR_ILLEGAL_RANGE,
  ERROR_MEMORY_ALLOCATION,
  ERROR_MEMORY_FREE_NULL,
  ERROR_NOT_REACHED,
  ERROR_REFERENCE_NOT_EXPECTED_TYPE,
  ERROR_UNIMPLEMENTED,
  ERROR_OPEN_LOG_FILE,
  ERROR_TEST,
  ERROR_INTERNAL_ASSERTION_FAILURE,
  ERROR_BAD_ALLOCATION_SIZE,
  ERROR_ILLEGAL_ARGUMENT,
  ERROR_MEMORY_START_PADDING_ERROR,
  ERROR_MEMORY_END_PADDING_ERROR,
  ERROR_FATAL,
  ERROR_ILLEGAL_STATE,
  ERROR_ILLEGAL_INPUT,
  ERROR_ILLEGAL_UTF_8_CODE_POINT,
  ERROR_ILLEGAL_TERMINAL_COORDINATES,
} error_code_t;

extern _Noreturn void fatal_error_impl(char* file, int line, int error_code);
extern const char* fatal_error_code_to_string(int error_code);
extern void configure_fatal_errors(fatal_error_config_t config);

/**
 * @macro fatal_error
 *
 * Terminates the program with a fatal error.
 */
#define fatal_error(code) fatal_error_impl(__FILE__, __LINE__, code)

#endif /* _FATAL_ERROR_H_ */
// SSCF generated file from: value.c

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
// SSCF generated file from: allocate.c

#ifndef _ALLOCATE_H_
#define _ALLOCATE_H_

#include <stdint.h>

extern uint8_t* checked_malloc(char* file, int line, uint64_t amount);
extern uint8_t* checked_malloc_copy_of(char* file, int line, uint8_t* source,
                                       uint64_t amount);
extern void checked_free(char* file, int line, void* pointer);

extern void check_memory_hashtable_padding();

/**
 * @macro malloc_bytes
 *
 * This is essentially the same as malloc but the memory is always
 * zeroed before return it to the user. We use a macro here to call
 * checked_malloc so that the file and line number can be passed.
 */
#define malloc_bytes(amount) (checked_malloc(__FILE__, __LINE__, amount))

/**
 * @macro free_bytes
 *
 * This is essentially the same as free.
 */
#define free_bytes(ptr) (checked_free(__FILE__, __LINE__, ptr))

/**
 * @macro malloc_struct
 *
 * This provides a convenient way to allocate a zero-filled space big
 * enough to hold the given structure with sizeof automatically used
 * and the result automatically casted to a pointer to the given type.
 */
#define malloc_struct(struct_name)                                             \
  ((struct_name*) (checked_malloc(__FILE__, __LINE__, sizeof(struct_name))))

/**
 * @macro malloc_copy_of
 *
 * This provides a convenient way to allocate a copy of a given
 * "source". Generally you would only use it with a pointer to a
 * structure though in theory it could be used on other things.
 *
 * See also: string_duplicate which automatically calls strlen, etc.
 */
#define malloc_copy_of(source, number_of_bytes)                                \
  (checked_malloc_copy_of(__FILE__, __LINE__, source, number_of_bytes))

// TODO(jawilson): malloc_copy_of_struct

#endif /* _ALLOCATE_H_ */
// SSCF generated file from: uint64.c

#ifndef _UINT64_H_
#define _UINT64_H_

#include <stdint.h>

extern int uint64_highest_bit_set(uint64_t n);

#endif /* _UINT64_H_ */
// SSCF generated file from: string-util.c

#ifndef _STRING_UTIL_H_
#define _STRING_UTIL_H_

#include <stdint.h>

extern int string_is_null_or_empty(const char* str1);
extern int string_equal(const char* str1, const char* str2);
extern int string_starts_with(const char* str1, const char* str2);
extern int string_ends_with(const char* str1, const char* str2);
extern boolean_t string_contains_char(const char* str, char ch);
extern int string_index_of_char(const char* a, char ch);
extern char* uint64_to_string(uint64_t number);
extern uint64_t string_hash(const char* str);
extern char* string_substring(const char* str, int start, int end);
extern value_result_t string_parse_uint64(const char* string);
extern value_result_t string_parse_uint64_dec(const char* string);
extern value_result_t string_parse_uint64_hex(const char* string);
extern value_result_t string_parse_uint64_bin(const char* string);
extern char* string_duplicate(const char* src);
extern char* string_append(const char* a, const char* b);
extern char* string_left_pad(const char* a, int count, char ch);
extern char* string_right_pad(const char* a, int count, char ch);
__attribute__((format(printf, 1, 2))) extern char* string_printf(char* format,
                                                                 ...);
char* string_truncate(char* str, int limit, char* at_limit_suffix);

#endif /* _STRING_UTIL_H_ */
// SSCF generated file from: logger.c

#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <stdarg.h>
#include <stdio.h>

#define LOGGER_OFF 0
#define LOGGER_TRACE 1
#define LOGGER_DEBUG 2
#define LOGGER_INFO 3
#define LOGGER_WARN 4
#define LOGGER_FATAL 5
#define LOGGER_TEST 6

struct logger_state_S {
  boolean_t initialized;
  int level;
  char* logger_output_filename;
  FILE* output;
};

typedef struct logger_state_S logger_state_t;

#ifndef LOGGER_DEFAULT_LEVEL
#define LOGGER_DEFAULT_LEVEL LOGGER_WARN
#endif /* LOGGER_DEFAULT_LEVEL */

extern logger_state_t global_logger_state;

extern void logger_init(void);

__attribute__((format(printf, 5, 6))) extern void
    logger_impl(char* file, int line_number, const char* function, int level,
                char* format, ...);

/**
 * @macro log_none
 *
 * This will never ever log and should have essentially zero impact on
 * compilation (including detecting errors). In other words it should
 * behave like an empty statment ";".
 *
 * On the other hand, errors in these statements will not be caught
 * and therefore it can't be depended on to keep working as you
 * refactor code and decide later that you want to turn it on.
 */
#define log_none(format, ...)                                                  \
  do {                                                                         \
  } while (0);

/**
 * @macro log_off
 *
 * This will never log however the compiler *should* still check to
 * make sure the code is legal and compiles. Any sufficiently smart
 * compiler with some level of optimization turned on should not
 * change it's code generation strategy at all if you leave one of these
 * statements in your source code and you should easily be able to
 * upgrade them to a real level later.
 */
#define log_off(format, ...)                                                   \
  do {                                                                         \
    if (0) {                                                                   \
      logger_impl(__FILE__, __LINE__, __FUNCTION__, LOGGER_TRACE, format,      \
                  ##__VA_ARGS__);                                              \
    }                                                                          \
  } while (0)

/**
 * @macro log_trace
 *
 * Log at the TRACE level using printf style formatting.
 */
#define log_trace(format, ...)                                                 \
  do {                                                                         \
    if (global_logger_state.level <= LOGGER_TRACE) {                           \
      logger_impl(__FILE__, __LINE__, __FUNCTION__, LOGGER_TRACE, format,      \
                  ##__VA_ARGS__);                                              \
    }                                                                          \
  } while (0)

/**
 * @macro log_debug
 *
 * Log at the DEBUG level using printf style formatting.
 */
#define log_debug(format, ...)                                                 \
  do {                                                                         \
    if (global_logger_state.level <= LOGGER_DEBUG) {                           \
      logger_impl(__FILE__, __LINE__, __FUNCTION__, LOGGER_DEBUG, format,      \
                  ##__VA_ARGS__);                                              \
    }                                                                          \
  } while (0)

/**
 * @macro log_info
 *
 * Log at the INFO level using printf style formatting.
 */
#define log_info(format, ...)                                                  \
  do {                                                                         \
    if (global_logger_state.level <= LOGGER_INFO) {                            \
      logger_impl(__FILE__, __LINE__, __FUNCTION__, LOGGER_INFO, format,       \
                  ##__VA_ARGS__);                                              \
    }                                                                          \
  } while (0)

/**
 * Determine if logging at the INFO level is enabled.
 */
static inline boolean_t should_log_info() {
  return global_logger_state.level <= LOGGER_INFO;
}

/**
 * @macro log_warn
 *
 * Log at the WARN level using printf style formatting.
 */
#define log_warn(format, ...)                                                  \
  do {                                                                         \
    if (global_logger_state.level <= LOGGER_WARN) {                            \
      logger_impl(__FILE__, __LINE__, __FUNCTION__, LOGGER_WARN, format,       \
                  ##__VA_ARGS__);                                              \
    }                                                                          \
  } while (0)

/**
 * @macro log_fatal
 *
 * Log at the FATAL level using printf style formatting.
 *
 * Typically this is only done before invoking fatal_error though I
 * don't have a convenient way to enforce this.
 */
#define log_fatal(format, ...)                                                 \
  do {                                                                         \
    if (global_logger_state.level <= LOGGER_FATAL) {                           \
      logger_impl(__FILE__, __LINE__, __FUNCTION__, LOGGER_FATAL, format,      \
                  ##__VA_ARGS__);                                              \
    }                                                                          \
  } while (0)

/**
 * @macro log_test
 *
 * Log at the TEST level using printf style formatting. This should
 * only be used inside of test code to communicate very basic
 * information back to the user when running a test and is therefore
 * independent of the actual log level.
 */
#define log_test(format, ...)                                                  \
  do {                                                                         \
    logger_impl(__FILE__, __LINE__, __FUNCTION__, LOGGER_TEST, format,         \
                ##__VA_ARGS__);                                                \
  } while (0)

#endif /* _LOGGER_H_ */
// SSCF generated file from: utf8-decoder.c

#ifndef _UTF8_DECODER_H_
#define _UTF8_DECODER_H_

#include <stdint.h>

/**
 * @struct utf8_decode_result_t
 *
 * Holds the result of utf8_decode.
 */
struct utf8_decode_result_S {
  uint32_t code_point;
  uint8_t num_bytes;
  boolean_t error;
};

typedef struct utf8_decode_result_S utf8_decode_result_t;

extern utf8_decode_result_t utf8_decode(const uint8_t* utf8_bytes);

#endif /* _UTF8_DECODER_H_ */
// SSCF generated file from: buffer.c

#ifndef _BUFFER_H_
#define _BUFFER_H_

/**
 * @file buffer.c
 *
 * A growable array of bytes.
 *
 * Buffers are useful for constructing UTF-8 strings (for example by
 * using buffer_printf) though they can hold any binary data including
 * interior NUL bytes. Buffers automatically grow as data is appended
 * (or inserted into) them reducing large classes of errors.
 */

#include <ctype.h>
#include <stdint.h>
#include <string.h>

struct buffer_S {
  uint32_t length;
  uint32_t capacity;
  uint8_t* elements;
};

/**
 * @struct buffer_t
 *
 * Buffers are non-thread-safe memory regions that generally "grow"
 * over time. Whenever a buffer grows beyound it's capacity, it is
 * moved to accomodates its new capacity (even if there may be other
 * pointers to it's insides, so don't do that unless you have decided
 * the buffer will never grow again).
 *
 * While buffers may seem scary, used properly from a single thread,
 * they are actually quite predictable.
 */
typedef struct buffer_S buffer_t;

extern buffer_t* make_buffer(uint64_t initial_capacity);

extern uint64_t buffer_length(buffer_t* buffer);

extern uint8_t buffer_get(buffer_t* buffer, uint64_t position);

extern char* buffer_c_substring(buffer_t* buffer, uint64_t start, uint64_t end);

extern char* buffer_to_c_string(buffer_t* buffer);

extern void buffer_clear(buffer_t* buffer);

extern buffer_t* buffer_increase_capacity(buffer_t* buffer, uint64_t capacity);

extern buffer_t* buffer_append_byte(buffer_t* buffer, uint8_t byte);

extern buffer_t* buffer_append_bytes(buffer_t* buffer, uint8_t* bytes,
                                     uint64_t n_bytes);

extern buffer_t* buffer_append_buffer(buffer_t* buffer, buffer_t* src_buffer);

extern buffer_t* buffer_append_sub_buffer(buffer_t* buffer,
                                          uint64_t start_position,
                                          uint64_t end_position,
                                          buffer_t* src_buffer);

extern buffer_t* buffer_append_string(buffer_t* buffer, const char* str);

__attribute__((format(printf, 2, 3))) extern buffer_t*
    buffer_printf(buffer_t* buffer, char* format, ...);

extern buffer_t* buffer_append_repeated_byte(buffer_t* buffer, uint8_t byte,
                                             int count);

utf8_decode_result_t buffer_utf8_decode(buffer_t* buffer, uint64_t position);

extern buffer_t* buffer_append_code_point(buffer_t* buffer,
                                          uint32_t code_point);

boolean_t buffer_match_string_at(buffer_t* buffer, uint64_t start_position,
                                 char* str);

buffer_t* buffer_from_string(char* string);

buffer_t* buffer_adjust_region(buffer_t* buffer, uint64_t original_start,
                               uint64_t original_end, uint64_t new_width);

buffer_t* buffer_replace_all(buffer_t* buffer, char* original_text,
                             char* replacement_text);

buffer_t* buffer_replace_matching_byte(buffer_t* buffer, uint8_t original,
                                       uint8_t replacement);

boolean_t buffer_region_contains(buffer_t* buffer, uint64_t start, uint64_t end,
                                 char* text);

uint64_t buffer_beginning_of_line(buffer_t* buffer, uint64_t start);

uint64_t buffer_end_of_line(buffer_t* buffer, uint64_t start);

buffer_t* buffer_to_uppercase(buffer_t* buffer);

buffer_t* buffer_to_lowercase(buffer_t* buffer);

typedef struct line_and_column_S {
  uint64_t line;
  uint64_t column;
} line_and_column_t;

line_and_column_t buffer_position_to_line_and_column(buffer_t* buffer,
                                                     uint64_t position);

#endif /* _BUFFER_H_ */
// SSCF generated file from: value-array.c

#ifndef _VALUE_ARRAY_H_
#define _VALUE_ARRAY_H_

struct value_array_S {
  uint32_t length;
  uint32_t capacity;
  value_t* elements;
};

/**
 * @typedef value_array_t
 *
 * A growable array of 64bit "values" (so integers, doubles, and
 * pointers).
 */
typedef struct value_array_S value_array_t;

extern value_array_t* make_value_array(uint64_t initial_capacity);
extern value_t value_array_get(value_array_t* array, uint32_t index);
extern void value_array_replace(value_array_t* array, uint32_t index,
                                value_t element);
extern void value_array_add(value_array_t* array, value_t element);
extern void value_array_push(value_array_t* array, value_t element);
extern value_t value_array_pop(value_array_t* array);
extern void value_array_insert_at(value_array_t* array, uint32_t position,
                                  value_t element);
extern value_t value_array_delete_at(value_array_t* array, uint32_t position);

#define value_array_get_ptr(array, index_expression, cast_type)                \
  (cast(cast_type, value_array_get(array, index_expression).ptr))

#endif /* _VALUE_ARRAY_H_ */
// SSCF generated file from: value-alist.c

#ifndef _VALUE_ALIST_H_
#define _VALUE_ALIST_H_

struct value_alist_S {
  struct value_alist_S* next;
  value_t key;
  value_t value;
};

typedef struct value_alist_S value_alist_t;

extern value_result_t value_alist_find(value_alist_t* list,
                                       value_comparison_fn cmp_fn, value_t key);

__attribute__((warn_unused_result)) extern value_alist_t*
    value_alist_insert(value_alist_t* list, value_comparison_fn cmp_fn,
                       value_t key, value_t value);

__attribute__((warn_unused_result)) extern value_alist_t*
    value_alist_delete(value_alist_t* list, value_comparison_fn cmp_fn,
                       value_t key);

__attribute__((warn_unused_result)) extern uint64_t
    value_alist_length(value_alist_t* list);

/**
 * @macro value_alist_foreach
 *
 * Allows iteration over the keys and values in a string association
 * list.
 */
#define value_alist_foreach(alist, key_var, value_var, statements)             \
  do {                                                                         \
    value_alist_t* head = alist;                                               \
    while (head) {                                                             \
      value_t key_var = head->key;                                             \
      value_t value_var = head->value;                                         \
      statements;                                                              \
      head = head->next;                                                       \
    }                                                                          \
  } while (0)

#endif /* _VALUE_ALIST_H_ */
// SSCF generated file from: string-alist.c

#ifndef _STRING_ALIST_H_
#define _STRING_ALIST_H_

struct string_alist_S {};

typedef struct string_alist_S string_alist_t;

/**
 * @function alist_find
 *
 * Find the value associate with the given key. Use is_ok() or
 * is_not_ok() to see if the value is valid (i.e., if the key was
 * actually found).
 */
static inline value_result_t alist_find(string_alist_t* list, char* key) {
  return value_alist_find(cast(value_alist_t*, list), cmp_string_values,
                          str_to_value(key));
}

/**
 * @function alist_insert
 *
 * Insert a new key and value into an assocation list.
 */
__attribute__((warn_unused_result)) static inline string_alist_t*
    alist_insert(string_alist_t* list, char* key, value_t value) {
  return cast(string_alist_t*,
              value_alist_insert(cast(value_alist_t*, list), cmp_string_values,
                                 str_to_value(key), value));
}

/**
 * @function alist_delete
 *
 * Delete the key and associated value from the given association
 * list. Neither the key nor the value associated are themselves
 * freed.
 */
__attribute__((warn_unused_result)) static inline string_alist_t*
    alist_delete(string_alist_t* list, char* key) {
  return cast(string_alist_t*,
              value_alist_delete(cast(value_alist_t*, list), cmp_string_values,
                                 str_to_value(key)));
}

/**
 * @function alist_length
 *
 * Determine the length of an alist.
 *
 * The alist argument MAY be null.
 */
__attribute__((warn_unused_result)) static inline uint64_t
    alist_length(string_alist_t* list) {
  return value_alist_length(cast(value_alist_t*, list));
}

/**
 * @macro string_alist_foreach
 *
 * Allows iteration over the keys and values in a string association
 * list.
 */
#define string_alist_foreach(alist, key_var, value_var, statements)            \
  do {                                                                         \
    value_alist_foreach(cast(value_alist_t*, alist), key_var##_value,          \
                        value_var, {                                           \
                          char* key_var = (key_var##_value).str;               \
                          statements;                                          \
                        });                                                    \
  } while (0)

#endif /* _STRING_ALIST_H_ */
// SSCF generated file from: value-hashtable.c

#ifndef _VALUE_HASHTABLE_H_
#define _VALUE_HASHTABLE_H_

/**
 * @compiliation_option ARMYKNIFE_HT_LOAD_FACTOR
 *
 * The "load factor" is the ratio of the number of keys in the hash
 * table to the most optimistic capacity for the table if every key
 * happened to be hashed to a different bucket. When the load factor
 * reaches this value, the hash table will be resized to a larger
 * capacity to improve performance. A higher value allows for a denser
 * hash table but can lead to more collisions and slower lookups and
 * insertions. A lower value wastes memory but reduces collisions.
 */
#ifndef ARMYKNIFE_HT_LOAD_FACTOR
#define ARMYKNIFE_HT_LOAD_FACTOR 0.75
#endif /* ARMYKNIFE_HT_LOAD_FACTOR */

/**
 * @compiliation_option AK_HT_UPSCALE_MULTIPLIER
 *
 * In all cases this should be a number > 1.0.
 */
#ifndef AK_HT_UPSCALE_MULTIPLIER
#define AK_HT_UPSCALE_MULTIPLIER 1.75
#endif /* AK_HT_UPSCALE_MULTIPLIER */

struct value_hashtable_S {
  uint64_t n_buckets;
  uint64_t n_entries;
  value_alist_t** buckets;
};

typedef struct value_hashtable_S value_hashtable_t;

extern value_hashtable_t* make_value_hashtable(uint64_t n_buckets);

extern value_hashtable_t* value_ht_insert(value_hashtable_t* ht,
                                          value_hash_fn hash_fn,
                                          value_comparison_fn cmp_fn,
                                          value_t key, value_t value);

extern value_hashtable_t* value_ht_delete(value_hashtable_t* ht,
                                          value_hash_fn hash_fn,
                                          value_comparison_fn cmp_fn,
                                          value_t key);

extern value_result_t value_ht_find(value_hashtable_t* ht,
                                    value_hash_fn hash_fn,
                                    value_comparison_fn cmp_fn, value_t key);

extern void value_hashtable_upsize_internal(value_hashtable_t* ht,
                                            value_hash_fn hash_fn,
                                            value_comparison_fn cmp_fn);

/**
 * @function value_ht_num_entries
 *
 * Returns the number of entries in the hashtable.
 */
static inline uint64_t value_ht_num_entries(value_hashtable_t* ht) {
  return ht->n_entries;
}

/**
 * @macro value_ht_foreach
 *
 * Allows traversing all elements of a hashtable in an unspecified
 * order.
 */
#define value_ht_foreach(ht, key_var, value_var, statements)                   \
  do {                                                                         \
    for (int ht_index = 0; ht_index < ht->n_buckets; ht_index++) {             \
      value_alist_t* alist = ht->buckets[ht_index];                            \
      if (alist != NULL) {                                                     \
        value_alist_foreach(alist, key_var, value_var, statements);            \
      }                                                                        \
    }                                                                          \
  } while (0)

#endif /* _VALUE_HASHTABLE_H_ */
// SSCF generated file from: string-hashtable.c

#ifndef _STRING_HASHTABLE_H_
#define _STRING_HASHTABLE_H_

struct string_hashtable_S {};

typedef struct string_hashtable_S string_hashtable_t;

static inline value_hashtable_t* to_value_hashtable(string_hashtable_t* ht) {
  return cast(value_hashtable_t*, ht);
}

/**
 * @function make_string_hashtable
 *
 * Create a hashtable with the given number of buckets.
 *
 * The minimum number of buckets is currently 2 to make it less likely
 * we run into some resize loop depending on the values of
 * ARMYKNIFE_HT_LOAD_FACTOR and AK_HT_UPSCALE_MULTIPLIER).
 */
static inline string_hashtable_t* make_string_hashtable(uint64_t n_buckets) {
  return cast(string_hashtable_t*, make_value_hashtable(n_buckets));
}

/**
 * @function string_ht_insert
 *
 * Insert an association into the hashtable.
 */
__attribute__((warn_unused_result)) static inline string_hashtable_t*
    string_ht_insert(string_hashtable_t* ht, char* key, value_t value) {
  return cast(string_hashtable_t*,
              value_ht_insert(to_value_hashtable(ht), hash_string_value,
                              cmp_string_values, str_to_value(key), value));
}

/**
 * @function string_ht_delete
 *
 * Delete an association from the hashtable. It is not an error to
 * delete a key that doesn't exist in the hashtable.
 */
__attribute__((warn_unused_result)) static inline string_hashtable_t*
    string_ht_delete(string_hashtable_t* ht, char* key) {
  return cast(string_hashtable_t*,
              value_ht_delete(to_value_hashtable(ht), hash_string_value,
                              cmp_string_values, str_to_value(key)));
}

/**
 * @function string_ht_find
 *
 * Find an association in the hashtable.
 */
static inline value_result_t string_ht_find(string_hashtable_t* ht, char* key) {
  return value_ht_find(to_value_hashtable(ht), hash_string_value,
                       cmp_string_values, str_to_value(key));
}

/**
 * @function string_ht_num_entries
 *
 * Returns the number of entries in the hashtable.
 */
static inline uint64_t string_ht_num_entries(string_hashtable_t* ht) {
  return value_ht_num_entries(to_value_hashtable(ht));
}

/**
 * @macro string_ht_foreach
 *
 * Allows traversing all elements of a hashtable in an unspecified
 * order.
 */
#define string_ht_foreach(ht, key_var, value_var, statements)                  \
  do {                                                                         \
    value_ht_foreach(to_value_hashtable(ht), key_var##_value, value_var, {     \
      char* key_var = (key_var##_value).str;                                   \
      statements;                                                              \
    });                                                                        \
  } while (0)

#endif /* _STRING_HASHTABLE_H_ */
// SSCF generated file from: value-tree.c

#ifndef _VALUE_TREE_H_
#define _VALUE_TREE_H_

struct value_tree_S {
  value_t key;
  value_t value;
  uint32_t level;
  struct value_tree_S* left;
  struct value_tree_S* right;
};

typedef struct value_tree_S value_tree_t;

extern value_result_t value_tree_find(value_tree_t* t,
                                      value_comparison_fn cmp_fn, value_t key);

__attribute__((warn_unused_result)) extern value_tree_t*
    value_tree_insert(value_tree_t* t, value_comparison_fn cmp_fn, value_t key,
                      value_t value);

__attribute__((warn_unused_result)) extern value_tree_t*
    value_tree_delete(value_tree_t* t, value_comparison_fn cmp_fn, value_t key);

/**
 * @macro value_tree_foreach
 *
 * Perform an inorder traversal of a value-tree.
 *
 * key_var is created in a new block scope with type value_t.
 *
 * value_var is created in a new block scope with type value_t and you
 * will probably want to use something like ".ptr" or ".u64" on the
 * value to obtain the actual value.
 *
 * statements should be a normal C block, aka, something like:
 * ```
 * {
 *   statement1();
 *   statement2();
 * }
 * ```
 *
 * Unforunately it is not possible to use "break" or "continue" with
 * this style of loop (and worse, there will be no compilation error
 * or warning if you accidentally do that...)
 */
#define value_tree_foreach(tree, key_var, value_var, statements)               \
  do {                                                                         \
    int stack_n_elements = 0;                                                  \
    value_tree_t* stack[64];                                                   \
    value_tree_t* current = tree;                                              \
    while (current != NULL || stack_n_elements > 0) {                          \
      while (current != NULL) {                                                \
        stack[stack_n_elements++] = current;                                   \
        current = current->left;                                               \
      }                                                                        \
      current = stack[--stack_n_elements];                                     \
      value_t key_var = current->key;                                          \
      value_t value_var = current->value;                                      \
      statements;                                                              \
      current = current->right;                                                \
    }                                                                          \
  } while (0)

#endif /* _VALUE_TREE_H_ */
// SSCF generated file from: string-tree.c

#ifndef _STRING_TREE_H_
#define _STRING_TREE_H_

struct string_tree_S {};

typedef struct string_tree_S string_tree_t;

/**
 * @function string_tree_find
 *
 * Find the value associate with the key in the tree.
 */
static inline value_result_t string_tree_find(string_tree_t* t, char* key) {
  return value_tree_find(cast(value_tree_t*, t), cmp_string_values,
                         str_to_value(key));
}

/**
 * @function string_tree_insert
 *
 * Insert an association of key and a value (or update the current
 * value stored in the tree).
 */
__attribute__((warn_unused_result)) static inline string_tree_t*
    string_tree_insert(string_tree_t* t, char* key, value_t value) {
  return cast(string_tree_t*,
              value_tree_insert(cast(value_tree_t*, t), cmp_string_values,
                                str_to_value(key), value));
}

/**
 * @function string_tree_delete
 *
 * Delete the association of key (if it exists in the tree). It is not
 * an error to delete a key that isn't present in the table.
 */
__attribute__((warn_unused_result)) static inline string_tree_t*
    string_tree_delete(string_tree_t* t, char* key) {
  return cast(string_tree_t*,
              value_tree_delete(cast(value_tree_t*, t), cmp_string_values,
                                str_to_value(key)));
}

/**
 * @macro string_tree_foreach
 *
 * Perform an inorder traversal of a string-tree.
 *
 * key_var is created in a new block scope with type char*.
 *
 * value_var is created in a new block scope with type value_t and you
 * will probably want to use something like ".ptr" or ".u64" on the
 * value to obtain the actual value.
 *
 * statements should be a normal C block, aka, something like:
 * ```
 * {
 *   statement1();
 *   statement2();
 * }
 * ```
 *
 * Unforunately it is not possible to use "break" or "continue" with
 * this style of loop (and worse, there will be no compilation error
 * or warning if you accidentally do that...)
 */
#define string_tree_foreach(tree, key_var, value_var, statements)              \
  do {                                                                         \
    value_tree_foreach(cast(value_tree_t*, tree), key_var##_value, value_var,  \
                       {                                                       \
                         char* key_var = (key_var##_value).str;                \
                         statements;                                           \
                       });                                                     \
  } while (0)

#endif /* _STRING_TREE_H_ */
// SSCF generated file from: flag.c

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
// SSCF generated file from: io.c

#ifndef _IO_H_
#define _IO_H_

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>

__attribute__((warn_unused_result)) extern buffer_t*
    buffer_append_file_contents(buffer_t* bytes, char* file_name);

__attribute__((warn_unused_result)) extern buffer_t*
    buffer_append_all(buffer_t* buffer, FILE* input);

extern void buffer_write_file(buffer_t* bytes, char* file_name);

__attribute__((warn_unused_result)) extern buffer_t*
    buffer_read_until(buffer_t* buffer, FILE* input, char end_of_line);

extern buffer_t* buffer_read_ready_bytes(buffer_t* buffer, FILE* input,
                                         uint64_t max_bytes);

extern buffer_t* buffer_read_ready_bytes_file_number(buffer_t* buffer,
                                                     int file_number,
                                                     uint64_t max_bytes);

int file_peek_byte(FILE* input);

boolean_t file_eof(FILE* input);

void file_copy_stream(FILE* input, FILE* output, boolean_t until_eof,
                      uint64_t size);

void file_skip_bytes(FILE* input, uint64_t n_bytes);

#endif /* _IO_H_ */
// SSCF generated file from: terminal.c

#ifndef _TERMINAL_H_
#define _TERMINAL_H_

#include <termios.h>

/**
 * @file terminal.c
 *
 * A simplistic terminal library that doesn't depend on ncurses,
 * etc. Realistically most terminals are now decendants of xterm
 * (itelf like vt100 or ANSI deep down) and tend to converge on how
 * new features work so we are going to just initialy code for Konsole
 * and go from there.
 *
 * Based on this document, I feel we can assume we have 24bit color
 * support and handle other terminals differently in the future
 * (perhaps by just not emitting colors):
 *
 * https://github.com/termstandard/colors
 */

/**
 * @struct box_drawing_t
 *
 * A structure to hold unicode code-points for each part of a box to
 * draw in the terminal.
 */
struct box_drawing_S {
  uint32_t upper_left_corner;
  uint32_t upper_right_corner;
  uint32_t lower_left_corner;
  uint32_t lower_right_corner;
  uint32_t top_edge;
  uint32_t left_edge;
  uint32_t right_edge;
  uint32_t bottom_edge;
};

typedef struct box_drawing_S box_drawing_t;

/**
 * @struct term_keypress_t
 *
 * Represents a keypress from the terminal. Either one of code_point
 * or key_code will be non-zero if n_bytes_consumed is greater than
 * zero.
 */
struct term_keypress_S {
  uint32_t code_point;
  uint8_t key_code;
  uint8_t n_bytes_consumed;
  uint8_t shift : 1;
  uint8_t ctrl : 1;
  uint8_t meta : 1;
  uint8_t super : 1;
  uint8_t hyper : 1;
};

// Names and descriptions are from ncurses but the values are
// different.
#define KEY_DOWN 1      /* down-arrow key */
#define KEY_UP 2        /* up-arrow key */
#define KEY_LEFT 3      /* left-arrow key */
#define KEY_RIGHT 4     /* right-arrow key */
#define KEY_HOME 5      /* home key */
#define KEY_BACKSPACE 6 /* backspace key */
#define KEY_F0 7        /* Function keys */
#define KEY_F1 8
#define KEY_F2 9
#define KEY_F3 10
#define KEY_F4 11
#define KEY_F5 12
#define KEY_F6 13
#define KEY_F7 14
#define KEY_F8 15
#define KEY_F9 16
#define KEY_F10 17
#define KEY_F11 18
#define KEY_F12 19
#define KEY_F13 20
#define KEY_F14 21
#define KEY_F15 22
#define KEY_DL 23        /* delete-line key */
#define KEY_IL 24        /* insert-line key */
#define KEY_DC 25        /* delete-character key */
#define KEY_IC 26        /* insert-character key */
#define KEY_EIC 27       /* sent by rmir or smir in insert mode */
#define KEY_CLEAR 28     /* clear-screen or erase key */
#define KEY_EOS 29       /* clear-to-end-of-screen key */
#define KEY_EOL 30       /* clear-to-end-of-line key */
#define KEY_SF 31        /* scroll-forward key */
#define KEY_SR 32        /* scroll-backward key */
#define KEY_NPAGE 33     /* next-page key */
#define KEY_PPAGE 34     /* previous-page key */
#define KEY_STAB 35      /* set-tab key */
#define KEY_CTAB 36      /* clear-tab key */
#define KEY_CATAB 37     /* clear-all-tabs key */
#define KEY_ENTER 38     /* enter/send key */
#define KEY_PRINT 39     /* print key */
#define KEY_LL 40        /* lower-left key (home down) */
#define KEY_A1 41        /* upper left of keypad */
#define KEY_A3 42        /* upper right of keypad */
#define KEY_B2 43        /* center of keypad */
#define KEY_C1 44        /* lower left of keypad */
#define KEY_C3 45        /* lower right of keypad */
#define KEY_BTAB 46      /* back-tab key */
#define KEY_BEG 47       /* begin key */
#define KEY_CANCEL 48    /* cancel key */
#define KEY_CLOSE 49     /* close key */
#define KEY_COMMAND 50   /* command key */
#define KEY_COPY 51      /* copy key */
#define KEY_CREATE 52    /* create key */
#define KEY_END 53       /* end key */
#define KEY_EXIT 54      /* exit key */
#define KEY_FIND 55      /* find key */
#define KEY_HELP 56      /* help key */
#define KEY_MARK 57      /* mark key */
#define KEY_MESSAGE 58   /* message key */
#define KEY_MOVE 59      /* move key */
#define KEY_NEXT 60      /* next key */
#define KEY_OPEN 61      /* open key */
#define KEY_OPTIONS 62   /* options key */
#define KEY_PREVIOUS 63  /* previous key */
#define KEY_REDO 64      /* redo key */
#define KEY_REFERENCE 65 /* reference key */
#define KEY_REFRESH 66   /* refresh key */
#define KEY_REPLACE 67   /* replace key */
#define KEY_RESTART 68   /* restart key */
#define KEY_RESUME 69    /* resume key */
#define KEY_SAVE 70      /* save key */
#define KEY_SBEG 71      /* shifted begin key */
#define KEY_SCANCEL 72   /* shifted cancel key */
#define KEY_SCOMMAND 73  /* shifted command key */
#define KEY_SCOPY 74     /* shifted copy key */
#define KEY_SCREATE 75   /* shifted create key */
#define KEY_SDC 76       /* shifted delete-character key */
#define KEY_SDL 77       /* shifted delete-line key */
#define KEY_SELECT 78    /* select key */
#define KEY_SEND 79      /* shifted end key */
#define KEY_SEOL 80      /* shifted clear-to-end-of-line key */
#define KEY_SEXIT 81     /* shifted exit key */
#define KEY_SFIND 82     /* shifted find key */
#define KEY_SHELP 83     /* shifted help key */
#define KEY_SHOME 84     /* shifted home key */
#define KEY_SIC 85       /* shifted insert-character key */
#define KEY_SLEFT 86     /* shifted left-arrow key */
#define KEY_SMESSAGE 87  /* shifted message key */
#define KEY_SMOVE 88     /* shifted move key */
#define KEY_SNEXT 89     /* shifted next key */
#define KEY_SOPTIONS 90  /* shifted options key */
#define KEY_SPREVIOUS 91 /* shifted previous key */
#define KEY_SPRINT 92    /* shifted print key */
#define KEY_SREDO 93     /* shifted redo key */
#define KEY_SREPLACE 94  /* shifted replace key */
#define KEY_SRIGHT 95    /* shifted right-arrow key */
#define KEY_SRSUME 96    /* shifted resume key */
#define KEY_SSAVE 97     /* shifted save key */
#define KEY_SSUSPEND 98  /* shifted suspend key */
#define KEY_SUNDO 99     /* shifted undo key */
#define KEY_SUSPEND 100  /* suspend key */
#define KEY_UNDO 101     /* undo key */
#define KEY_MOUSE 102    /* Mouse event has occurred */
#define KEY_RESIZE 103   /* Terminal resize event */

typedef struct term_keypress_S term_keypress_t;

__attribute__((warn_unused_result)) extern buffer_t*
    term_clear_screen(buffer_t* buffer);

__attribute__((warn_unused_result)) extern buffer_t*
    term_set_foreground_color(buffer_t* buffer, uint32_t color);

__attribute__((warn_unused_result)) extern buffer_t*
    term_set_background_color(buffer_t* buffer, uint32_t color);

__attribute__((warn_unused_result)) extern buffer_t*
    term_move_cursor_absolute(buffer_t* buffer, int x, int y);

__attribute__((warn_unused_result)) extern buffer_t*
    term_move_cursor_relative(buffer_t* buffer, int x, int y);

__attribute__((warn_unused_result)) extern buffer_t*
    term_bold(buffer_t* buffer);

__attribute__((warn_unused_result)) extern buffer_t* term_dim(buffer_t* buffer);

__attribute__((warn_unused_result)) extern buffer_t*
    term_italic(buffer_t* buffer);

__attribute__((warn_unused_result)) extern buffer_t*
    term_underline(buffer_t* buffer);

__attribute__((warn_unused_result)) extern buffer_t*
    term_reset_formatting(buffer_t* buffer);

__attribute__((warn_unused_result)) extern buffer_t*
    term_draw_box(buffer_t* buffer, uint16_t x0, uint16_t y0, uint16_t x1,
                  uint16_t y1, box_drawing_t* box);

extern struct termios term_echo_off();
extern void term_echo_restore(struct termios oldt);

// TODO(jawilson): terminal queries like request cursor position

#endif /* _TERMINAL_H_ */
// SSCF generated file from: tokenizer.c

#ifndef _TOKENIZER_H_
#define _TOKENIZER_H_

extern value_array_t* string_tokenize(const char* str, const char* delimiters);
extern value_array_t* buffer_tokenize(buffer_t* buffer, const char* delimiters);
extern value_array_t* tokenize_memory_range(uint8_t* start, uint64_t length,
                                            const char* delimiters);

// TODO(jawilson):

#endif /* _TOKENIZER_H_ */
// SSCF generated file from: random.c

#ifndef _RANDOM_H_
#define _RANDOM_H_

struct random_state_S {
  uint64_t a;
  uint64_t b;
};

typedef struct random_state_S random_state_t;

extern random_state_t random_state_for_test(void);
extern uint64_t random_next_uint64(random_state_t* state);
extern uint64_t random_next_uint64_below(random_state_t* state,
                                         uint64_t maximum);

#endif /* _RANDOM_H_ */
// SSCF generated file from: cdl-printer.c

#ifndef _CDL_PRINTER_H_
#define _CDL_PRINTER_H_

typedef struct {
  buffer_t* buffer;
  char* key_token;
  int indention_level;
} cdl_printer_t;

cdl_printer_t* make_cdl_printer(buffer_t* buffer);

void cdl_boolean(cdl_printer_t* printer, boolean_t bolean);
void cdl_string(cdl_printer_t* printer, char* string);
void cdl_int64(cdl_printer_t* printer, int64_t number);
void cdl_uint64(cdl_printer_t* printer, uint64_t number);
void cdl_double(cdl_printer_t* printer, double number);

void cdl_start_array(cdl_printer_t* printer);
void cdl_end_array(cdl_printer_t* printer);

void cdl_start_table(cdl_printer_t* printer);
void cdl_key(cdl_printer_t* printer, char* key);
void cdl_end_table(cdl_printer_t* printer);

#endif /* _CDL_PRINTER_H_ */
// SSCF generated file from: sub-process.c

#ifndef _SUB_PROCESS_H_
#define _SUB_PROCESS_H_

/**
 * @file sub-process.c
 *
 * Utility to launch a sub-processes and interfact with them.
 *
 * (This version will probably only work on Unix like operating
 * systems but we should be able to make an MS-Windows version at some
 * point.)
 */

/**
 * @enum sub_process_exit_status_t
 */
typedef enum {
  EXIT_STATUS_UNKNOWN,
  EXIT_STATUS_NORMAL_EXIT,
  EXIT_STATUS_SIGNAL,
  EXIT_STATUS_ABNORMAL
} sub_process_exit_status_t;

typedef struct {
  // launch parameters
  value_array_t* argv; // argv[0] is the program executable path

  // child process information
  pid_t pid;
  int stdin;
  int stdout;
  int stderr;

  // exit info
  sub_process_exit_status_t exit_status;
  int exit_code;
  int exit_signal;
} sub_process_t;

sub_process_t* make_sub_process(value_array_t* argv);

boolean_t sub_process_launch(sub_process_t* sub_process);

uint64_t sub_process_write(sub_process_t* sub_process, buffer_t* data,
                           uint64_t start_position);

void sub_process_close_stdin(sub_process_t* sub_process);

void sub_process_read(sub_process_t* sub_process, buffer_t* stdout,
                      buffer_t* stderr);

void sub_process_wait(sub_process_t* sub_process);

void sub_process_launch_and_wait(sub_process_t* sub_process,
                                 buffer_t* child_stdin, buffer_t* child_stdout,
                                 buffer_t* child_stderr);

#endif /* _SUB_PROCESS_H_ */
// SSCF generated file from: splitjoin.c

#ifndef _SPLITJOIN_H_
#define _SPLITJOIN_H_

buffer_t* join_array_of_strings(value_array_t* array_of_strings,
                                char* separator);

#endif /* _SPLITJOIN_H_ */
// SSCF generated file from: test.c

#ifndef _TEST_H_
#define _TEST_H_

/**
 * @macro test_fail
 *
 * Immediately fail a test after printing a message (supplied the same
 * way as printf, log_fatal, etc.)
 *
 * This is a macro instead of a function because it automatically
 * passes the current file and line number to test_fail_and_exit which
 * enables uniform and convenient error messagages according to GNU
 * guidelines (so they will look like gcc compiler errors if your IDE
 * understands those).
 *
 * It is *advised* (at least for now) to not pass complicated
 * expressions to `test_fail` if those are likely to fail. Instead run
 * the test in a debugger and set a break-point on
 * `test_fail_and_exit`.
 */
#define test_fail(format, ...)                                                 \
  do {                                                                         \
    test_fail_and_exit(__FILE__, __LINE__, format, ##__VA_ARGS__);             \
  } while (0)


/**
 * @macro test_assert
 *
 * Evaluate the condition and fail the test if the result is false.
 */
#define test_assert(condition)                                                 \
  do {                                                                         \
    if (!(condition))                                                          \
      test_fail("A test assertion failed. Condition expression was: %s",       \
                #condition);                                                   \
  } while (0)

/**
 * @macro test_assert_uint64_equal
 *
 * Assert that two values are the same by casting both of them to
 * uint64_t and seeing if they are "==".
 *
 * Except that we lose the "location" information, this would be
 * better as an inline function rather than a macro.
 */
#define test_assert_integer_equal(a, b)                                        \
  do {                                                                         \
    unsigned long long casted_a = (unsigned long long) (a);                    \
    unsigned long long casted_b = (unsigned long long) (b);                    \
    if (a != b) {                                                              \
      test_fail(                                                               \
          "An integer comparision failed\n  Expected:\n    ⟦%llu⟧\n  "     \
          "But was:\n    ⟦%llu⟧\n",                                            \
          casted_a, casted_b);                                                 \
    }                                                                          \
  } while (0)

/**
 * @macro test_assert_string_equal
 *
 * Assert that two c strings are the same.
 */
#define test_assert_string_equal(a, b)                                          \
  do {                                                                          \
    if (!b) {                                                                   \
      test_fail(                                                                \
          "A test string equal assertion failed\n  Expected:\n    ⟦%s⟧\n  " \
          "But was:\n    nullptr\n",                                            \
          a);                                                                   \
    }                                                                           \
    if (!string_equal(a, b)) {                                                  \
      test_fail(                                                                \
          "A test string equal assertion failed\n  Expected:\n    ⟦%s⟧\n  " \
          "But was:\n    ⟦%s⟧\n",                                               \
          a, b);                                                                \
    }                                                                           \
  } while (0)

static inline void open_arena_for_test(void) {
  /*
#ifdef C_ARMYKNIFE_LIB_USE_ARENAS
  arena_open(4096 * 256);
#endif
  */
}

static inline void close_arena_for_test(void) {
  /*
#ifdef C_ARMYKNIFE_LIB_USE_ARENAS
  arena_close();
#endif
  */
}

#endif /* _TEST_H_ */
#ifdef C_ARMYKNIFE_LIB_IMPL

#ifndef _OMNI_C_H_
#define _OMNI_C_H_

/**
 * @file omni-c.c
 *
 * These macros help c-armyknife-lib conform to what omni-c can
 * (currently) handle. If you are writing code within c-armyknife-lib,
 * then you must use these macros but if you are just using
 * c-amryknife-lib within your own C project, then you can ignore
 * these macros unless you strictly want to use them.
 */

/**
 * @macro cast
 *
 * Perform an unsafe cast of expr to the given type. This is much
 * easier for the omni-c compiler to handle because we know to parse a
 * type as the first argument even if we don't know what all the types
 * are yet. While omni-c uses this macro, if you are using plain C,
 * you don't have to use it.
 */
#define cast(type, expr) ((type) (expr))

/**
 * @macro block_expr
 *
 * The omni-c compiler isn't smart enough to handle a statement block
 * passed to a macro yet but marking such blocks with this macro makes
 * it much easier to handle for now.
 */
#define block_expr(block) block

#endif /* _OMNI_C_H_ */
/**
 * @file min-max.c
 *
 * Macros version of min and max functions.
 */

#ifndef _MIN_MAX_H_
#define _MIN_MAX_H_

/**
 * @macro min
 *
 * Produce the minimum of a and b. Prior to C23, we evalutate one of
 * the arguments twice but we eventually hope to evaluate each
 * argument only once and use the "auto" argument so that the macro
 * works for any type that can be compared with <.
 */
#define min(a, b) ((a) < (b) ? (a) : (b))

/**
 * @macro max
 *
 * Produce the maximum of a and b. Prior to C23, we evalutate one of
 * the arguments twice but we eventually hope to evaluate each
 * argument only once and use the "auto" argument so that the macro
 * works for any type that can be compared with >.
 */
#define max(a, b) ((a) > (b) ? (a) : (b))

#endif /* _MIN_MAX_H_ */
/**
 * @file boolean.c
 *
 * Make sure that at least true/false from <stdbool.h> are available
 * and and a new typedef named boolean_t because bool seems ugly. (Use
 * what you prefer!)
 */

// ======================================================================
// This section is extraced to boolean.h
// ======================================================================

#ifndef _BOOLEAN_H_
#define _BOOLEAN_H_

#include <stdbool.h>

/**
 * @type boolean_t
 *
 * This is a simple typedef for "bool" (or _Bool from C99) which is
 * available from "stdbool.h" as bool and true and false constants are
 * also defined. We use it for more consistency in primitive types
 * (where only char* is commonly used in this library despite not
 * following the typically naming convention).
 */
typedef bool boolean_t;

// #define true ((boolean_t) 1)
// #define false ((boolean_t) 0)

#endif /* _BOOLEAN_H_ */
/**
 * @file compound-literal.c
 *
 * This file exists solely for interfacing with early versions of the
 * omni-c compiler where it is temporarily more convenient to have a
 * keyword to denote compound literals (the ones that look like
 * casts).
 *
 * We don't need it when the literal looks like this:
 *
 * ```
 * point_t a = { 10, 20 };
 * int foo[5] = { 0 };
 * ```
 *
 * But we do need it when the the literal looks like this:
 *
 * ```
 * (point_t) { .x = 10, .y = 20 }
`* ```
 *
 * This is somewhat similar to usage of cast() macro though in the
 * case of cast, sometimes it actually becomes easier to read.
 *
 * ```
 * (int) (a - b) -----> cast(int, a - b)
 * ```
 */

// ======================================================================
// This section is extraced to compound-literal.h
// ======================================================================

#ifndef _COMPOUND_LITERAL_H_
#define _COMPOUND_LITERAL_H_

#define compound_literal(type, ...) ((type) __VA_ARGS__)

#endif /* _COMPOUND_LITERAL_H_ */

#ifndef _FN_H_
#define _FN_H_

/**
 * @file fn.c
 *
 * C's declarator syntax and the spiral rule are sometimes very hard
 * to read and write so fn_t enters the room.
 *
 * Before:
 *
 * ```
 * typedef uint64_t (*value_hash_fn)(value_t value);
 * ```
 *
 * After:
 * ```
 * typedef fn_t(uint64_t, value_t value) value_hash_fn;
 * ```
 *
 * Notice how the name of the type now appears all the way to the
 * right.
 *
 * (Omni C will eventually allow "=" for typedefs so:
 *
 * ```
 * typedef value_hash_fn = fn_t(uint64_t, value_t value);
 * ```
 *
 * I asked an LLM to help make my point and it suggested this
 * function prototype as a confusing case:
 *
 * ```
 * void (*signal(int sig, void (*func)(int)))(int);
 * ```
 *
 * This really is exteremely difficult to read. It's a function that
 * both accepts and returns a signal handler where a signal handler is
 * a function that takes an int and return's void. Even with that
 * clue, the above is pretty hard to read, while this is hopefully not
 * that difficult to read.
 *
 * ```
 * fn_t(void, int) signal(int sig, fn_t(void, int) handler);
`* ```
 */

#define fn_t(return_type, ...) typeof(return_type(*)(__VA_ARGS__))

#endif /* _FN_H_ */

#ifndef _LEB128_H_
#define _LEB128_H_

/**
 * @file leb128.c
 *
 * ULEB-128 (unsigned) and SLEB-128 (signed, aka, possibly negative
 * integers) are variable length encodings for possibly "very large"
 * numbers. I first came across LEB while working with "DWARF2" in in
 * the 1990s. Google uses the same encoding for unsigned integers but
 * uses "zig-zag" for signed numbers (the sign bit is stored in the
 * lowest bit instead of the more complicated technique SLEB-128
 * uses).
 *
 * Essentially the 8th bit of each byte says whether to continue
 * adding bits from the next byte or if the number is now "complete"
 * (so 7-bits per byte are the real data and 1-bit is
 * "overhead"). Since many numbers we deal with are actually small,
 * LEB encoding tend to be an efficient encoding of integers and grow
 * "gracefully" to accomodate larger numbers. In fact the "128" in
 * ULEB-128 is a misnomer -- you could in theory encode more than 128
 * bits. This implementation only reads 64bit numbers, not the full
 * 128 bits.
 *
 * This implementation originally comes from LLVM although I have
 * changed it to a C file, renamed the functions, got rid of camel
 * case, removed the C++ namespace, removed the inline directive,
 * removed the pad to argument and changed how results and errors are
 * returned.
 */

#include <stdint.h>

#define ERROR_INSUFFICIENT_INPUT -1
#define ERROR_TOO_BIG -2

typedef struct {
  uint64_t number;
  // Negative values mean an error occurred.
  int size;
} unsigned_decode_result;

typedef struct {
  uint64_t number;
  // Negative values mean an error occurred.
  int size;
} signed_decode_result;

extern unsigned encode_sleb_128(int64_t Value, uint8_t* p);
extern unsigned encode_uleb_128(uint64_t Value, uint8_t* p);
extern unsigned_decode_result decode_uleb_128(const uint8_t* p,
                                              const uint8_t* end);
extern signed_decode_result decode_sleb_128(const uint8_t* p,
                                            const uint8_t* end);

#endif /* _LEB128_H_ */

//===- llvm/Support/LEB128.h - [SU]LEB128 utility functions -----*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file declares some utility functions for encoding SLEB128 and
// ULEB128 values.
//
//===----------------------------------------------------------------------===//

/**
 * @function encode_sleb_128
 *
 * Utility function to encode a SLEB128 value to a raw byte
 * buffer. Returns the length in bytes of the encoded value. 10 bytes
 * should be enough to hold the 64bit number after encoding.
 */
unsigned encode_sleb_128(int64_t Value, uint8_t* p) {
  uint8_t* orig_p = p;
  int More;
  do {
    uint8_t Byte = Value & 0x7f;
    // NOTE: this assumes that this signed shift is an arithmetic right shift.
    Value >>= 7;
    More = !((((Value == 0) && ((Byte & 0x40) == 0))
              || ((Value == -1) && ((Byte & 0x40) != 0))));
    if (More)
      Byte |= 0x80; // Mark this byte to show that more bytes will follow.
    *p++ = Byte;
  } while (More);

  return cast(unsigned, p - orig_p);
}

/**
 * @function
 *
 * Utility function to encode a ULEB128 value to a raw byte
 * buffer. Returns the length in bytes of the encoded value. 10 bytes
 * should be enough to hold the 64bit number after encoding.
 */
unsigned encode_uleb_128(uint64_t Value, uint8_t* p) {
  uint8_t* orig_p = p;
  do {
    uint8_t Byte = Value & 0x7f;
    Value >>= 7;
    if (Value != 0)
      Byte |= 0x80; // Mark this byte to show that more bytes will follow.
    *p++ = Byte;
  } while (Value != 0);

  return cast(unsigned, (p - orig_p));
}

/**
 * @function decode_uleb_128
 *
 * Decode a ULEB-128 value (up to 64bits).
 */
unsigned_decode_result decode_uleb_128(const uint8_t* p, const uint8_t* end) {
  const uint8_t* orig_p = p;
  uint64_t Value = 0;
  unsigned Shift = 0;
  do {
    if (p == end) {
      unsigned_decode_result result = compound_literal(
          unsigned_decode_result, {0, ERROR_INSUFFICIENT_INPUT});
      return result;
    }
    uint64_t Slice = *p & 0x7f;
    if ((Shift >= 64 && Slice != 0) || Slice << Shift >> Shift != Slice) {
      unsigned_decode_result result
          = compound_literal(unsigned_decode_result, {0, ERROR_TOO_BIG});
      return result;
    }
    Value += Slice << Shift;
    Shift += 7;
  } while (*p++ >= 128);
  unsigned_decode_result result = compound_literal(
      unsigned_decode_result, {Value, cast(unsigned, p - orig_p)});
  return result;
}

/**
 * @function decode_sleb_128
 *
 * Decode a SLEB128 value (up to 64bits)
 */
signed_decode_result decode_sleb_128(const uint8_t* p, const uint8_t* end) {
  const uint8_t* orig_p = p;
  int64_t Value = 0;
  unsigned Shift = 0;
  uint8_t Byte;
  do {
    if (p == end) {
      signed_decode_result result = compound_literal(
          signed_decode_result, {0, ERROR_INSUFFICIENT_INPUT});
      return result;
    }
    Byte = *p;
    uint64_t Slice = Byte & 0x7f;
    // This handles decoding padded numbers, otherwise we might be
    // able to test very easily at the end of the loop.
    if ((Shift >= 64 && Slice != (Value < 0 ? 0x7f : 0x00))
        || (Shift == 63 && Slice != 0 && Slice != 0x7f)) {
      signed_decode_result result
          = compound_literal(signed_decode_result, {0, ERROR_TOO_BIG});
      return result;
    }
    Value |= Slice << Shift;
    Shift += 7;
    ++p;
  } while (Byte >= 128);
  // Sign extend negative numbers if needed.
  if (Shift < 64 && (Byte & 0x40))
    Value |= (-1ULL) << Shift;
  signed_decode_result result
      = compound_literal(signed_decode_result, {Value, (p - orig_p)});
  return result;
}
/**
 * @file fatal-error.c
 *
 * The intent is that everything but a normal program exit will end up
 * here. (To catch SIGSIGV errors you may call
 * configure_fatal_errors() first with catch_sigsegv set.)
 *
 * Note that you can use fatal_error's to your advantage by setting
 * the environment variable ARMYKNIFE_FATAL_ERROR_SLEEP_SECONDS to
 * some value to give yourself enough time to attach a debugger.
 *
 * In this case C's macros are paying off as the file and line number
 * are easy to obtain.
 *
 * TODO(jawilson): environment variable to be quieter...
 */

// ======================================================================
// This is block is extraced to fatal-error.h
// ======================================================================

#ifndef _FATAL_ERROR_H_
#define _FATAL_ERROR_H_

struct fatal_error_config_S {
  boolean_t catch_sigsegv;
};

typedef struct fatal_error_config_S fatal_error_config_t;

/**
 * @constants error_code_t
 */
typedef enum {
  ERROR_UKNOWN,
  ERROR_SIGSEGV,
  ERROR_ACCESS_OUT_OF_BOUNDS,
  ERROR_BAD_COMMAND_LINE,
  ERROR_DYNAMICALLY_SIZED_TYPE_ILLEGAL_IN_CONTAINER,
  ERROR_ILLEGAL_ENUM_VALUE,
  ERROR_ILLEGAL_INITIAL_CAPACITY,
  ERROR_ILLEGAL_NULL_ARGUMENT,
  ERROR_ILLEGAL_ZERO_HASHCODE_VALUE,
  ERROR_ILLEGAL_RANGE,
  ERROR_MEMORY_ALLOCATION,
  ERROR_MEMORY_FREE_NULL,
  ERROR_NOT_REACHED,
  ERROR_REFERENCE_NOT_EXPECTED_TYPE,
  ERROR_UNIMPLEMENTED,
  ERROR_OPEN_LOG_FILE,
  ERROR_TEST,
  ERROR_INTERNAL_ASSERTION_FAILURE,
  ERROR_BAD_ALLOCATION_SIZE,
  ERROR_ILLEGAL_ARGUMENT,
  ERROR_MEMORY_START_PADDING_ERROR,
  ERROR_MEMORY_END_PADDING_ERROR,
  ERROR_FATAL,
  ERROR_ILLEGAL_STATE,
  ERROR_ILLEGAL_INPUT,
  ERROR_ILLEGAL_UTF_8_CODE_POINT,
  ERROR_ILLEGAL_TERMINAL_COORDINATES,
} error_code_t;

extern _Noreturn void fatal_error_impl(char* file, int line, int error_code);
extern const char* fatal_error_code_to_string(int error_code);
extern void configure_fatal_errors(fatal_error_config_t config);

/**
 * @macro fatal_error
 *
 * Terminates the program with a fatal error.
 */
#define fatal_error(code) fatal_error_impl(__FILE__, __LINE__, code)

#endif /* _FATAL_ERROR_H_ */

// ======================================================================

#include <execinfo.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

fatal_error_config_t fatal_error_config = {0};

void segmentation_fault_handler(int signal_number) {
  fatal_error(ERROR_SIGSEGV);
}

void configure_fatal_errors(fatal_error_config_t config) {
  fatal_error_config = config;
  if (config.catch_sigsegv) {
    signal(SIGSEGV, segmentation_fault_handler);
  }
}

void print_fatal_error_banner();
void print_backtrace();
void print_error_code_name(int error_code);

char* get_command_line() {
  buffer_t* buffer
      = buffer_append_file_contents(make_buffer(1), "/proc/self/cmdline");
  buffer_replace_matching_byte(buffer, 0, ' ');
  return buffer_to_c_string(buffer);
}

char* get_program_path() {
  char buf[4096];
  int n = readlink("/proc/self/exe", buf, sizeof(buf));
  if (n > 0) {
    return string_duplicate(buf);
  } else {
    return "<program-path-unknown>";
  }
}

_Noreturn void fatal_error_impl(char* file, int line, int error_code) {
  print_fatal_error_banner();
  print_backtrace();
  fprintf(stderr, "%s:%d: FATAL ERROR %d", file, line, error_code);
  print_error_code_name(error_code);
  fprintf(stderr, "\nCommand line: %s\n\n", get_command_line());
  char* sleep_str = getenv("ARMYKNIFE_FATAL_ERROR_SLEEP_SECONDS");
  if (sleep_str != NULL) {
    value_result_t sleep_time = string_parse_uint64(sleep_str);
    if (is_ok(sleep_time)) {
      fprintf(stderr,
              "Sleeping for %lu seconds so you can attach a debugger.\n",
              sleep_time.u64);
      fprintf(stderr, "  gdb -tui %s %d\n", get_program_path(), getpid());
      sleep(sleep_time.u64);
    }
  } else {
    fprintf(stderr, "(ARMYKNIFE_FATAL_ERROR_SLEEP_SECONDS is not set)\n");
  }
  fprintf(stderr, "Necessaria Morte Mori...\n");
  exit(-(error_code + 100));
}

void print_fatal_error_banner() {
  // As the first thing we print, also responsible for at least one
  // newline to start a new line if we may not be at one.
  fprintf(stderr, "\n========== FATAL_ERROR ==========\n");
}

void print_backtrace() {
#ifndef NO_VM_BACKTRACE_ON_FATAL_ERROR
  do {
    void* array[10];
    int size = backtrace(array, 10);
    char** strings = backtrace_symbols(array, size);

    // Print the backtrace
    for (int i = 0; i < size; i++) {
      printf("#%d %s\n", i, strings[i]);
    }
  } while (0);
#endif /* NO_VM_BACKTRACE_ON_FATAL_ERROR */
}

const char* fatal_error_code_to_string(int error_code) {
  switch (error_code) {
  case ERROR_UKNOWN:
    return "ERROR_UKNOWN";
  case ERROR_MEMORY_ALLOCATION:
    return "ERROR_MEMORY_ALLOCATION";
  case ERROR_MEMORY_FREE_NULL:
    return "ERROR_MEMORY_FREE_NULL";
  case ERROR_REFERENCE_NOT_EXPECTED_TYPE:
    return "ERROR_REFERENCE_NOT_EXPECTED_TYPE";
  case ERROR_ILLEGAL_INITIAL_CAPACITY:
    return "ERROR_ILLEGAL_INITIAL_CAPACITY";
  case ERROR_DYNAMICALLY_SIZED_TYPE_ILLEGAL_IN_CONTAINER:
    return "ERROR_DYNAMICALLY_SIZED_TYPE_ILLEGAL_IN_CONTAINER";
  case ERROR_ACCESS_OUT_OF_BOUNDS:
    return "ERROR_ACCESS_OUT_OF_BOUNDS";
  case ERROR_NOT_REACHED:
    return "ERROR_NOT_REACHED";
  case ERROR_ILLEGAL_ZERO_HASHCODE_VALUE:
    return "ERROR_ILLEGAL_ZERO_HASHCODE_VALUE";
  case ERROR_UNIMPLEMENTED:
    return "ERROR_UNIMPLEMENTED";
  case ERROR_ILLEGAL_NULL_ARGUMENT:
    return "ERROR_ILLEGAL_NULL_ARGUMENT";
  case ERROR_ILLEGAL_ARGUMENT:
    return "ERROR_ILLEGAL_ARGUMENT";
  case ERROR_MEMORY_START_PADDING_ERROR:
    return "ERROR_MEMORY_START_PADDING_ERROR";
  case ERROR_MEMORY_END_PADDING_ERROR:
    return "ERROR_MEMORY_END_PADDING_ERROR";

  default:
    return "error";
  }
}

void print_error_code_name(int error_code) {
  fprintf(stderr, " ");
  fprintf(stderr, "*** ");
  fprintf(stderr, "%s", fatal_error_code_to_string(error_code));
  fprintf(stderr, " ***\n");
}

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

/**
 * @file allocate.c
 *
 * Hook into malloc and free to make them more convenient and possibly
 * safer. For example, allocated memory is always zero'd and macros
 * like malloc_struct are more readable. We also have a novel LRU
 * memory-bounds checker that found a bug that valgrind didn't.
 *
 * For missing calls to free, we are fully compatbile with valgrind
 * (since we just call malloc/free). (Valgrind also has a memcheck
 * mode though it actually masked a bug instead of finding it.)
 *
 * We also have our own memcheck mode. The basic idea is to maintain a
 * LRU style lossy hashtable and to repeatedly scan this set of
 * allocations for overwrites. While this will not detect all
 * instances of an overwrite, when it does, it will be pretty
 * convenient to use in conjuction with a debugger to track the bug
 * down.
 *
 * There should be no run-time penalty when our additional debugging
 * options are turned off (though I still like how valgrind doesn't
 * even require recompilation so maybe if our different techniques
 * pays off, perhaps we can port it into valgrind so please send
 * feedback if you find armyknife-lib's memcheck mode helpful or not.
 */

#ifndef _ALLOCATE_H_
#define _ALLOCATE_H_

#include <stdint.h>

extern uint8_t* checked_malloc(char* file, int line, uint64_t amount);
extern uint8_t* checked_malloc_copy_of(char* file, int line, uint8_t* source,
                                       uint64_t amount);
extern void checked_free(char* file, int line, void* pointer);

extern void check_memory_hashtable_padding();

/**
 * @macro malloc_bytes
 *
 * This is essentially the same as malloc but the memory is always
 * zeroed before return it to the user. We use a macro here to call
 * checked_malloc so that the file and line number can be passed.
 */
#define malloc_bytes(amount) (checked_malloc(__FILE__, __LINE__, amount))

/**
 * @macro free_bytes
 *
 * This is essentially the same as free.
 */
#define free_bytes(ptr) (checked_free(__FILE__, __LINE__, ptr))

/**
 * @macro malloc_struct
 *
 * This provides a convenient way to allocate a zero-filled space big
 * enough to hold the given structure with sizeof automatically used
 * and the result automatically casted to a pointer to the given type.
 */
#define malloc_struct(struct_name)                                             \
  ((struct_name*) (checked_malloc(__FILE__, __LINE__, sizeof(struct_name))))

/**
 * @macro malloc_copy_of
 *
 * This provides a convenient way to allocate a copy of a given
 * "source". Generally you would only use it with a pointer to a
 * structure though in theory it could be used on other things.
 *
 * See also: string_duplicate which automatically calls strlen, etc.
 */
#define malloc_copy_of(source, number_of_bytes)                                \
  (checked_malloc_copy_of(__FILE__, __LINE__, source, number_of_bytes))

// TODO(jawilson): malloc_copy_of_struct

#endif /* _ALLOCATE_H_ */

// ======================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

boolean_t is_initialized = false;
boolean_t should_log_value = false;

uint64_t number_of_bytes_allocated = 0;
uint64_t number_of_malloc_calls = 0;
uint64_t number_of_free_calls = 0;

static inline boolean_t should_log_memory_allocation() {
  if (is_initialized) {
    return should_log_value;
  }
  char* var = getenv("ARMYKNIFE_LOG_MEMORY_ALLOCATION");
  is_initialized = true;
  if (var != NULL && strcmp(var, "true") == 0) {
    should_log_value = true;
  }
  return should_log_value;
}

/**
 * @debug_compiliation_option ARMYKNIFE_MEMORY_ALLOCATION_START_PADDING
 *
 * The amount of padding to place in front of each allocation. We have
 * several ways to then check that no one overwrites this padding
 * though we won't catch every case.
 *
 * This should be a multiple of 8 or else the expected alignment
 * (which malloc doesn't make that explicit...) will be broken.
 */
#ifndef ARMYKNIFE_MEMORY_ALLOCATION_START_PADDING
#define ARMYKNIFE_MEMORY_ALLOCATION_START_PADDING 8
#endif

/**
 * @debug_compiliation_option ARMYKNIFE_MEMORY_ALLOCATION_END_PADDING
 *
 * The amount of padding to place at the back of each allocation. We
 * have several ways to then check that no one overwrites this padding
 * though we won't catch every case.
 *
 * Unlike start padding, this does not effect alignment and so values
 * as small as 1 make perfect sense though I still recommend 4 or 8
 * bytes especially on 64bit big-endian architectures.
 */
#ifndef ARMYKNIFE_MEMORY_ALLOCATION_END_PADDING
#define ARMYKNIFE_MEMORY_ALLOCATION_END_PADDING 8
#endif

/**
 * @debug_compiliation_option ARMYKNIFE_MEMORY_ALLOCATION_HASHTABLE_SIZE
 *
 * This determine how big the lossy hashtable is. On every allocation
 * or deallocation the *entire* lossy hashtable is scanned to see if
 * the padding bytes have been perturbed which makes it possible to
 * find some memory overwrite errors earlier than waiting for the free
 * call (or potentially even if the memory isn't ever freed).
 *
 * It makes no sense to set this unless either
 * ARMYKNIFE_MEMORY_ALLOCATION_START_PADDING or
 * ARMYKNIFE_MEMORY_ALLOCATION_END_PADDING is non-zero.
 */
#ifndef ARMYKNIFE_MEMORY_ALLOCATION_HASHTABLE_SIZE
#define ARMYKNIFE_MEMORY_ALLOCATION_HASHTABLE_SIZE 16
#endif

/**
 * @debug_compiliation_option ARMYKNIFE_MEMORY_ALLOCATION_MAXIMUM_AMOUNT
 *
 * This is just a fail-safe to catch extremely dumb allocation amounts
 * (at least as of 2023). If you know you will have a fixed size
 * amount of memory, you could set this lower and potentially get a
 * slightly nicer error message.
 */
#ifndef ARMYKNIFE_MEMORY_ALLOCATION_MAXIMUM_AMOUNT
#define ARMYKNIFE_MEMORY_ALLOCATION_MAXIMUM_AMOUNT (1L << 48)
#endif

#define START_PADDING_BYTE (170 & 0xff)
#define END_PADDING_BYTE ((~START_PADDING_BYTE) & 0xff)

struct memory_hashtable_bucket_S {
  // malloc will never allocated at address 0 so if this field is
  // zero, then this spot in the hashtable is occupied.
  uint64_t malloc_address;
  uint64_t malloc_size;
  char* allocation_filename;
  uint64_t allocation_line_number;
};

typedef struct memory_hashtable_bucket_S memory_hashtable_bucket_t;

memory_hashtable_bucket_t memory_ht[ARMYKNIFE_MEMORY_ALLOCATION_HASHTABLE_SIZE];

void check_start_padding(uint8_t* address) {
  for (int i = 0; i < ARMYKNIFE_MEMORY_ALLOCATION_START_PADDING; i++) {
    if (address[i] != START_PADDING_BYTE) {
      fatal_error(ERROR_MEMORY_START_PADDING_ERROR);
    }
  }
}

void check_end_padding(uint8_t* address, char* filename, uint64_t line) {
  for (int i = 0; i < ARMYKNIFE_MEMORY_ALLOCATION_END_PADDING; i++) {
    if (address[i] != END_PADDING_BYTE) {
      fprintf(stderr,
              "FATAL: someone clobbered past an allocation %lu. (allocated "
              "here: %s:%lu)\n",
              cast(uint64_t, address), filename, line);
      fatal_error(ERROR_MEMORY_END_PADDING_ERROR);
    }
  }
}

void check_memory_hashtable_padding() {
  for (int i = 0; i < ARMYKNIFE_MEMORY_ALLOCATION_HASHTABLE_SIZE; i++) {
    if (memory_ht[i].malloc_address != 0) {
      uint64_t malloc_start_address = memory_ht[i].malloc_address;
      uint64_t malloc_size = memory_ht[i].malloc_size;
      check_start_padding(cast(uint8_t*, malloc_start_address));
      check_end_padding(
          cast(uint8_t*,
               (malloc_start_address + ARMYKNIFE_MEMORY_ALLOCATION_START_PADDING
                + malloc_size)),
          memory_ht[i].allocation_filename,
          memory_ht[i].allocation_line_number);
    }
  }
}

// I got this from a blog post by Daniel Lemire (who was actually
// pushing a different scheme...) A terrible hash function will sink
// our scheme but anything that isn't terrible just gets us closer to
// some ideal.
uint64_t mumurhash64_mix(uint64_t h) {
  h *= h >> 33;
  h *= 0xff51afd7ed558ccdL;
  h *= h >> 33;
  h *= 0xc4ceb9fe1a85ec53L;
  h *= h >> 33;
  return h;
}

// Start tracking padding for a given allocated address. This includes
// setting the padding to particular values and of course putting the
// address into the tracking table.
void track_padding(char* file, int line, uint8_t* address, uint64_t amount) {
  // First set the padding to predicatable values
  for (int i = 0; i < ARMYKNIFE_MEMORY_ALLOCATION_START_PADDING; i++) {
    address[i] = START_PADDING_BYTE;
  }
  uint8_t* end_padding_address
      = address + amount + ARMYKNIFE_MEMORY_ALLOCATION_START_PADDING;
  for (int i = 0; i < ARMYKNIFE_MEMORY_ALLOCATION_END_PADDING; i++) {
    end_padding_address[i] = END_PADDING_BYTE;
  }

  if (ARMYKNIFE_MEMORY_ALLOCATION_HASHTABLE_SIZE > 0) {
    // Now replace whatever entry we might already have there. This is
    // why we have more LRU semantics. We could use another signal to
    // probalistically delay updating the hashtable when the bucket is
    // already occupied but I think LRU might work well most of the
    // time. (Mostly a hunch I will admit.).
    int bucket = mumurhash64_mix(cast(uint64_t, address))
                 % ARMYKNIFE_MEMORY_ALLOCATION_HASHTABLE_SIZE;
    memory_ht[bucket].malloc_address = cast(uint64_t, address);
    memory_ht[bucket].malloc_size = amount;
    memory_ht[bucket].allocation_filename = file;
    memory_ht[bucket].allocation_line_number = line;
  }
}

void untrack_padding(uint8_t* malloc_address) {
  check_start_padding(malloc_address);
  // Unfortunately, since we don't know the size of the allocation, we
  // can't actually check the end padding! When there is enough start
  // padding (say at least 64bits), then we could potentially store
  // say 48bits worth of an allocation amount in it.
  //
  // On the other hand, we do check the end padding if it is still
  // tracked in the lossy memory hashtable.

  if (ARMYKNIFE_MEMORY_ALLOCATION_HASHTABLE_SIZE > 0) {
    // Now finally zero-out the memory hashtable.
    int bucket = mumurhash64_mix(cast(uint64_t, malloc_address))
                 % ARMYKNIFE_MEMORY_ALLOCATION_HASHTABLE_SIZE;
    memory_ht[bucket].malloc_address = 0;
    memory_ht[bucket].malloc_size = 0;
    memory_ht[bucket].allocation_filename = 0;
    memory_ht[bucket].allocation_line_number = 0;
  }
}

/**
 * @function checked_malloc
 *
 * Allocate the given amount bytes or cause a fatal error. The memory
 * is also zeroed.
 *
 * If possible, use the macros malloc_bytes or malloc_struct instead
 * for an easier to use interface. Those macros simply call
 * checked_malloc.
 */
uint8_t* checked_malloc(char* file, int line, uint64_t amount) {

  if (amount == 0 || amount > ARMYKNIFE_MEMORY_ALLOCATION_MAXIMUM_AMOUNT) {
    fatal_error(ERROR_BAD_ALLOCATION_SIZE);
  }

  if (should_log_memory_allocation()) {
    fprintf(stderr,
            "ALLOCATE %s:%d -- n_bytes=%lu already_allocated=%lu n_calls=%lu\n",
            file, line, amount, number_of_bytes_allocated,
            number_of_malloc_calls);
  }

  check_memory_hashtable_padding();

  uint64_t amount_with_padding = ARMYKNIFE_MEMORY_ALLOCATION_START_PADDING
                                 + amount
                                 + ARMYKNIFE_MEMORY_ALLOCATION_END_PADDING;
  uint8_t* result = malloc(amount_with_padding);
  if (result == NULL) {
    fatal_error_impl(file, line, ERROR_MEMORY_ALLOCATION);
  }

  if (should_log_memory_allocation()) {
    fprintf(stderr, "ALLOCATE %s:%d -- %lu -- ptr=%lu\n", file, line, amount,
            cast(unsigned long, result));
  }

  memset(result, 0, amount_with_padding);
  track_padding(file, line, result, amount);

  number_of_bytes_allocated += amount;
  number_of_malloc_calls++;

  return result + ARMYKNIFE_MEMORY_ALLOCATION_START_PADDING;
}

/**
 * @function checked_malloc_copy_of
 *
 * Allocate amount bytes and initialize it with a copy of that many
 * bytes from source.
 */
uint8_t* checked_malloc_copy_of(char* file, int line, uint8_t* source,
                                uint64_t amount) {
  uint8_t* result = checked_malloc(file, line, amount);
  memcpy(result, source, amount);
  return result;
}

/**
 * @function checked_free
 *
 * Allocate amount bytes or cause a fatal error. The memory is also
 * zeroed.
 *
 * If possible, use the macros malloc_bytes or malloc_struct instead
 * for an easier to use interface. Those macros simply call
 * checked_malloc.
 */
void checked_free(char* file, int line, void* pointer) {
  if (should_log_memory_allocation()) {
    fprintf(stderr, "DEALLOCATE %s:%d -- %lu\n", file, line,
            cast(uint64_t, pointer));
  }
  if (pointer == NULL) {
    fatal_error_impl(file, line, ERROR_MEMORY_FREE_NULL);
  }

  // Check all of the padding we know about
  check_memory_hashtable_padding();

  uint8_t* malloc_pointer
      = cast(uint8_t*, pointer) - ARMYKNIFE_MEMORY_ALLOCATION_START_PADDING;

  // Check this entries padding (in case it got lossed from the global
  // hashtable), and also remove it from the hashtable if it was
  // found.
  untrack_padding(malloc_pointer);
  number_of_free_calls++;
  free(malloc_pointer);
}
/**
 * @file uint64.c
 *
 * Implement a couple of useful operations on uint64_t (which can
 * often be used for smaller types).
 */

#ifndef _UINT64_H_
#define _UINT64_H_

#include <stdint.h>

extern int uint64_highest_bit_set(uint64_t n);

#endif /* _UINT64_H_ */

/**
 * @function uint64_highest_bit_set
 *
 */
int uint64_highest_bit_set(uint64_t n) {
  if (n >= 1ULL << 32) {
    return uint64_highest_bit_set(n >> 32) + 32;
  } else if (n >= 1ULL << 16) {
    return uint64_highest_bit_set(n >> 16) + 16;
  } else if (n >= 1ULL << 8) {
    return uint64_highest_bit_set(n >> 8) + 8;
  } else if (n >= 1ULL << 4) {
    return uint64_highest_bit_set(n >> 4) + 4;
  } else if (n >= 1ULL << 2) {
    return uint64_highest_bit_set(n >> 2) + 2;
  } else if (n >= 1ULL << 1) {
    return uint64_highest_bit_set(n >> 1) + 1;
  } else {
    return 0;
  }
}
/**
 * @file string-util.c
 *
 * This contains additional string function to operate on "strings"
 * since the C libary has only basic routines.
 */

// ======================================================================
// This is block is extraced to allocate.h
// ======================================================================

#ifndef _STRING_UTIL_H_
#define _STRING_UTIL_H_

#include <stdint.h>

extern int string_is_null_or_empty(const char* str1);
extern int string_equal(const char* str1, const char* str2);
extern int string_starts_with(const char* str1, const char* str2);
extern int string_ends_with(const char* str1, const char* str2);
extern boolean_t string_contains_char(const char* str, char ch);
extern int string_index_of_char(const char* a, char ch);
extern char* uint64_to_string(uint64_t number);
extern uint64_t string_hash(const char* str);
extern char* string_substring(const char* str, int start, int end);
extern value_result_t string_parse_uint64(const char* string);
extern value_result_t string_parse_uint64_dec(const char* string);
extern value_result_t string_parse_uint64_hex(const char* string);
extern value_result_t string_parse_uint64_bin(const char* string);
extern char* string_duplicate(const char* src);
extern char* string_append(const char* a, const char* b);
extern char* string_left_pad(const char* a, int count, char ch);
extern char* string_right_pad(const char* a, int count, char ch);
__attribute__((format(printf, 1, 2))) extern char* string_printf(char* format,
                                                                 ...);
char* string_truncate(char* str, int limit, char* at_limit_suffix);

#endif /* _STRING_UTIL_H_ */

// ======================================================================

#include <stdlib.h>
#include <string.h>

uint64_t fasthash64(const void* buf, size_t len, uint64_t seed);

/**
 * @function string_is_null_or_empty
 *
 * Return true if the given string is NULL or strlen is zero.
 */
int string_is_null_or_empty(const char* str) {
  return (str == NULL) || (strlen(str) == 0);
}

/**
 * @function string_equal
 *
 * Return true if two strings are equal.
 */
int string_equal(const char* str1, const char* str2) {
  if (string_is_null_or_empty(str1)) {
    return string_is_null_or_empty(str2);
  }
  return strcmp(str1, str2) == 0;
}

/**
 * @function string_starts_with
 *
 * Return true if str1 starts with all of str2.
 */
int string_starts_with(const char* str1, const char* str2) {
  return strncmp(str1, str2, strlen(str2)) == 0;
}

/**
 * @function string_ends_with
 *
 * Return true if str1 ends with all of str2.
 */
int string_ends_with(const char* str1, const char* str2) {
  size_t len1 = strlen(str1);
  size_t len2 = strlen(str2);

  if (len2 > len1) {
    return 0;
  }

  return strcmp(str1 + (len1 - len2), str2) == 0;
}

/**
 * @function string_contains_char
 *
 * Return true if str contains that given character ch.
 */
boolean_t string_contains_char(const char* str, char ch) {
  return string_index_of_char(str, ch) >= 0;
}

// TODO(jawilson): string_contains_code_point

// TODO(jawilson): string_index_of_string

/**
 * @function string_index_of_char
 *
 * Return the index of the given character in a string or a value less
 * than zero if the character isn't inside of the string.
 */
int string_index_of_char(const char* str, char ch) {
  if (string_is_null_or_empty(str)) {
    return -1;
  }
  int str_length = strlen(str);
  for (int i = 0; i < str_length; i++) {
    if (str[i] == ch) {
      return i;
    }
  }
  return -1;
}

/**
 * @function string_hash
 *
 * Return a fast but generally high-quality 64bit hash of an input
 * string.
 */
uint64_t string_hash(const char* str) {
  return fasthash64(str, strlen(str), 0);
}

/**
 * @function string_substring
 *
 * Return a substring of the given string as a newly allocated string.
 */
char* string_substring(const char* str, int start, int end) {
  uint64_t len = strlen(str);
  if (start >= len || start >= end || end < start) {
    fatal_error(ERROR_ILLEGAL_ARGUMENT);
  }
  int result_size = end - start + 1;
  char* result = cast(char*, malloc_bytes(result_size));
  for (int i = start; (i < end); i++) {
    result[i - start] = str[i];
  }
  result[result_size - 1] = '\0';
  return result;
}

value_result_t string_parse_uint64_dec(const char* string) {
  uint64_t len = strlen(string);
  uint64_t integer = 0;

  if (len == 0) {
    return compound_literal(
        value_result_t, {.u64 = 0, .nf_error = NF_ERROR_NOT_PARSED_AS_NUMBER});
  }

  for (int i = 0; i < len; i++) {
    char ch = string[i];
    if (ch < '0' || ch > '9') {
      return compound_literal(
          value_result_t,
          {.u64 = 0, .nf_error = NF_ERROR_NOT_PARSED_AS_NUMBER});
    }
    uint64_t digit = string[i] - '0';
    integer = integer * 10 + digit;
  }

  return compound_literal(value_result_t, {.u64 = integer, .nf_error = NF_OK});
}

/**
 * Parse a sequence of zeros and ones. A prefix like 0b should be
 * stripped before calling this routine and the string must only
 * contain the digits 0 and 1.
 */
value_result_t string_parse_uint64_bin(const char* string) {
  uint64_t len = strlen(string);
  uint64_t integer = 0;

  if (len == 0) {
    return compound_literal(
        value_result_t, {.u64 = 0, .nf_error = NF_ERROR_NOT_PARSED_AS_NUMBER});
  }

  for (int i = 0; i < len; i++) {
    char ch = string[i];
    if (ch < '0' || ch > '1') {
      return compound_literal(
          value_result_t,
          {.u64 = 0, .nf_error = NF_ERROR_NOT_PARSED_AS_NUMBER});
    }
    uint64_t digit = string[i] - '0';
    integer = integer << 1 | digit;
  }

  return compound_literal(value_result_t, {.u64 = integer, .nf_error = NF_OK});
}

static inline boolean_t is_hex_digit(char ch) {
  return (ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f');
}

static inline uint64_t hex_digit_to_value(char ch) {
  if (ch >= '0' && ch <= '9') {
    return ch - '0';
  } else {
    return (ch - 'a') + 10;
  }
}

/**
 * Parse a sequence of characters "0123456789abcdef" to an uint64_t. A
 * prefix like 0x should be striped before calling this routine.
 *
 */
value_result_t string_parse_uint64_hex(const char* string) {
  uint64_t len = strlen(string);
  uint64_t integer = 0;

  if (len == 0) {
    return compound_literal(
        value_result_t, {.u64 = 0, .nf_error = NF_ERROR_NOT_PARSED_AS_NUMBER});
  }

  for (int i = 0; i < len; i++) {
    char ch = string[i];
    if (!is_hex_digit(ch)) {
      return compound_literal(
          value_result_t,
          {.u64 = 0, .nf_error = NF_ERROR_NOT_PARSED_AS_NUMBER});
    }
    uint64_t digit = hex_digit_to_value(ch);
    integer = integer << 4 | digit;
  }

  return compound_literal(value_result_t, {.u64 = integer, .nf_error = NF_OK});
}

/**
 * @function string_parse_uint64
 *
 * Parse a string as a uint64_t.
 *
 * If the string begins with "0x", the the number should be a well
 * formed hexidecimal number (in all lower-case).
 *
 * If the string begins with "0b", the the number should be a well
 * formed binary number.
 *
 * Ortherwise the number should be a well-formed decimal number.
 *
 * While currently overflow is not detected, that is likely to be
 * detected in future versions of the library.
 */
value_result_t string_parse_uint64(const char* string) {
  if (string_starts_with(string, "0x")) {
    return string_parse_uint64_hex(&string[2]);
  } else if (string_starts_with(string, "0b")) {
    return string_parse_uint64_bin(&string[2]);
  } else {
    return string_parse_uint64_dec(string);
  }
}

/**
 * @function string_duplicate
 *
 * Just like strdup except NULL is a valid source argument and we use
 * malloc_bytes which checks the return result from malloc.
 */
char* string_duplicate(const char* src) {
  if (src == NULL) {
    return NULL;
  }
  int len = strlen(src) + 1;
  char* result = cast(char*, malloc_bytes(len));
  memcpy(result, src, len);

  return result;
}

/**
 * @function string_append
 *
 * Return a freshly allocated string that is the concatentation of the
 * two input strings (neither of which should be NULL);
 */
char* string_append(const char* a, const char* b) {
  if (a == NULL || b == NULL) {
    fatal_error(ERROR_ILLEGAL_NULL_ARGUMENT);
  }
  int total_length = strlen(a) + strlen(b) + 1;
  char* result = cast(char*, malloc_bytes(total_length));
  strcat(result, a);
  strcat(result, b);
  return result;
}

/**
 * @function uint64_to_string
 *
 * Convert a uint64_t number to a string.
 */
char* uint64_to_string(uint64_t number) {
  char buffer[32];
  sprintf(buffer, "%lu", number);
  return string_duplicate(buffer);
}

/**
 * @function string_left_pad
 *
 * Prepend left left padding (if necessary) to make it at least N
 * bytes long.
 */
char* string_left_pad(const char* str, int n, char ch) {
  if (n < 0) {
    fatal_error(ERROR_ILLEGAL_RANGE);
  }

  int input_length = strlen(str);

  // Calculate padding needed
  int padding_needed = n - input_length;

  // As usual, since buffer's grow as needed, we are tolerant of a
  // wrong initial computation of the length though getting this wrong
  // is wasteful... In this case we do the wasteful thing knowing that
  // we will free everything shortly. We just want the correct result,
  // not necessarily as fast as possible.

  int len = 1; // max(padding_needed + input_length, input_length) + 1;

  buffer_t* buffer = make_buffer(len);
  for (int i = 0; i < padding_needed; i++) {
    buffer = buffer_append_byte(buffer, ch);
  }
  buffer = buffer_append_string(buffer, str);
  char* result = buffer_to_c_string(buffer);
  free_bytes(buffer);
  return result;
}

/**
 * @function string_right_pad
 *
 * Append right padding to a string (if necessary) to make it at least
 * N bytes long.
 */
char* string_right_pad(const char* str, int n, char ch) {
  if (n < 0) {
    fatal_error(ERROR_ILLEGAL_RANGE);
  }

  int input_length = strlen(str);

  // Calculate padding needed
  int padding_needed = n - input_length;

  // As usual, since buffer's grow as needed, we are tolerant of a
  // wrong initial computation of the length though getting this wrong
  // is wasteful... In this case we do the wasteful thing knowing that
  // we will free everything shortly. We just want the correct result,
  // not necessarily as fast as possible.

  int len = 1; // max(padding_needed + input_length, input_length) + 1;

  buffer_t* buffer = make_buffer(len);
  buffer = buffer_append_string(buffer, str);
  for (int i = 0; i < padding_needed; i++) {
    buffer = buffer_append_byte(buffer, ch);
  }
  char* result = buffer_to_c_string(buffer);
  free_bytes(buffer);
  return result;
}

/**
 * @function string_truncate
 *
 * Return a copy of the string truncated to limit number of *bytes*
 * (excluding the trailing zero). This is currently not unicode safe!
 *
 * When the string is truncated, we also add 'at_limit_suffix' which
 * may make the returned string actually that many characters
 * longer. This behavior is likely to change in a future version.
 */
char* string_truncate(char* str, int limit, char* at_limit_suffix) {
  // limit is just a guess, buffer's always grow as needed.
  buffer_t* buffer = make_buffer(limit);
  for (int i = 0;; i++) {
    char ch = str[i];
    if (ch == '\0') {
      char* result = buffer_to_c_string(buffer);
      free_bytes(buffer);
      return result;
    }
    buffer = buffer_append_byte(buffer, ch);
  }
  if (at_limit_suffix) {
    buffer = buffer_append_string(buffer, at_limit_suffix);
  }
  char* result = buffer_to_c_string(buffer);
  free_bytes(buffer);
  return result;
}

// Allows tests to make the temporary buffer small to more easily test
// the case where vsnprintf is called twice because the first time it
// was called we didn't have a large enough buffer.
#ifndef STRING_PRINTF_INITIAL_BUFFER_SIZE
#define STRING_PRINTF_INITIAL_BUFFER_SIZE 1024
#endif /* STRING_PRINTF_INITIAL_BUFFER_SIZE */

/**
 * @function string_printf
 *
 * Perform printf to a buffer and return the result as a dynamically
 * allocated string. The string is automatically allocated to the
 * appropriate size.
 */
__attribute__((format(printf, 1, 2))) char* string_printf(char* format, ...) {
  char buffer[STRING_PRINTF_INITIAL_BUFFER_SIZE];
  int n_bytes = 0;
  do {
    va_list args;
    va_start(args, format);
    n_bytes
        = vsnprintf(buffer, STRING_PRINTF_INITIAL_BUFFER_SIZE, format, args);
    va_end(args);
  } while (0);

  if (n_bytes < STRING_PRINTF_INITIAL_BUFFER_SIZE) {
    char* result = cast(char*, malloc_bytes(n_bytes + 1));
    strcat(result, buffer);
    return result;
  } else {
    char* result = cast(char*, malloc_bytes(n_bytes + 1));
    va_list args;
    va_start(args, format);
    int n_bytes_second = vsnprintf(result, n_bytes + 1, format, args);
    va_end(args);
    if (n_bytes_second != n_bytes) {
      fatal_error(ERROR_INTERNAL_ASSERTION_FAILURE);
    }
    return result;
  }
}

/* ================================================================ */

/* The MIT License

   Copyright (C) 2012 Zilong Tan (eric.zltan@gmail.com)

   Permission is hereby granted, free of charge, to any person
   obtaining a copy of this software and associated documentation
   files (the "Software"), to deal in the Software without
   restriction, including without limitation the rights to use, copy,
   modify, merge, publish, distribute, sublicense, and/or sell copies
   of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be
   included in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
   BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
   ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
   CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
*/

// #include "fasthash.h"

// Compression function for Merkle-Damgard construction.
// This function is generated using the framework provided.
static inline uint64_t mix(uint64_t h) {
  h ^= h >> 23;
  h *= 0x2127599bf4325c37ULL;
  h ^= h >> 47;
  return h;
}

// security: if the system allows empty keys (len=3) the seed is exposed, the
// reverse of mix. objsize: 0-1fd: 509
uint64_t fasthash64(const void* buf, size_t len, uint64_t seed) {
  const uint64_t m = 0x880355f21e6d1965ULL;
  const uint64_t* pos = cast(const uint64_t*, buf);
  const uint64_t* end = pos + (len / 8);
  const unsigned char* pos2;
  uint64_t h = seed ^ (len * m);
  uint64_t v;

  while (pos != end) {
    v = *pos++;
    h ^= mix(v);
    h *= m;
  }

  pos2 = cast(const unsigned char*, pos);
  v = 0;

  switch (len & 7) {
  case 7:
    v ^= cast(uint64_t, pos2[6]) << 48;
  case 6:
    v ^= cast(uint64_t, pos2[5]) << 40;
  case 5:
    v ^= cast(uint64_t, pos2[4]) << 32;
  case 4:
    v ^= cast(uint64_t, pos2[3]) << 24;
  case 3:
    v ^= cast(uint64_t, pos2[2]) << 16;
  case 2:
    v ^= cast(uint64_t, pos2[1]) << 8;
  case 1:
    v ^= cast(uint64_t, pos2[0]);
    h ^= mix(v);
    h *= m;
  }

  return mix(h);
}

/**
 * @file logger.c
 *
 * A "logger" is a type of code instrumentation and provides the
 * ability to put explicit "print statements" into your code without
 * necessarily having a large impact on the performance of that code
 * as long as the logger is turned off for that "level" of detail.
 *
 * If you've temporarily inserted print statements into your program,
 * you've probably already learned that logging is very useful and
 * also somewhat expensive in terms of run-time performance. [^1]
 *
 * Since logging is expensive, it is useful to be able to turn it on
 * and off (even without recompiling) which is done based on the
 * logging level. When logging is turned off, the cost is meant to be
 * equivalent to a load, compare, and branch though logging can effect
 * the C compiler's optimizations, so it is **not** recommended to be
 * left in critical loops. Obviously if the code is compiled into the
 * binary, even if the code to skip the logging doesn't considerably
 * increase run-time performance, it may still have an impact for
 * example on the output binary size.
 *
 * The default log level is "WARN" though it is possible to override
 * this with #define ARMYKNIFE_LIB_DEFAULT_LOG_LEVEL <level> when
 * including the library implementation (or from your build command
 * which allows C preprocessor definitions to be injected into your
 * source code, one reason you may want a debug vs production builds).
 *
 * Additionally, when the first log statement is encountered, we
 * examine the environment variable named ARMYKNIFE_LIB_LOG_LEVEL if
 * you want to adjust the level after compilation. Future versions
 * will certainly provide more control such as turn on logging only
 * for specific files as well as giving the C compiler enough
 * information to remove some logging code completely from the
 * binary).
 *
 * There are a set number of levels and they are defined like so:
 *
 * OFF = 0
 * TRACE = 1
 * DEBUG = 2
 * INFO = 3
 * WARN = 4
 * FATAL = 5
 * TEST = 6
 *
 * The most overlooked part of logging may be that putting PII or
 * other information into logs may violate GDPR and other privacy laws
 * depending on how the logs are processed and retained. Our
 * recommendation is to never intentionally log PII. It's especially
 * important to keep this in mind if you are developing an internet
 * application that the user isn't running on their own machine which
 * isn't an initial focus of this library.
 *
 * [^1]: For this implementation, getting a timestamp is probably one
 * kernel call and doing the actual output, since logging is less
 * useful when buffered, requires at least another kernel
 * call. Finally, formatting strings for human readability is
 * relatively expensive itself. For example, printing a large number
 * may require dozens or hundreds of cycles while adding two numbers
 * may take less than a single cycle on a modern pipelined processor).
 */

#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <stdarg.h>
#include <stdio.h>

#define LOGGER_OFF 0
#define LOGGER_TRACE 1
#define LOGGER_DEBUG 2
#define LOGGER_INFO 3
#define LOGGER_WARN 4
#define LOGGER_FATAL 5
#define LOGGER_TEST 6

struct logger_state_S {
  boolean_t initialized;
  int level;
  char* logger_output_filename;
  FILE* output;
};

typedef struct logger_state_S logger_state_t;

#ifndef LOGGER_DEFAULT_LEVEL
#define LOGGER_DEFAULT_LEVEL LOGGER_WARN
#endif /* LOGGER_DEFAULT_LEVEL */

extern logger_state_t global_logger_state;

extern void logger_init(void);

__attribute__((format(printf, 5, 6))) extern void
    logger_impl(char* file, int line_number, const char* function, int level,
                char* format, ...);

/**
 * @macro log_none
 *
 * This will never ever log and should have essentially zero impact on
 * compilation (including detecting errors). In other words it should
 * behave like an empty statment ";".
 *
 * On the other hand, errors in these statements will not be caught
 * and therefore it can't be depended on to keep working as you
 * refactor code and decide later that you want to turn it on.
 */
#define log_none(format, ...)                                                  \
  do {                                                                         \
  } while (0);

/**
 * @macro log_off
 *
 * This will never log however the compiler *should* still check to
 * make sure the code is legal and compiles. Any sufficiently smart
 * compiler with some level of optimization turned on should not
 * change it's code generation strategy at all if you leave one of these
 * statements in your source code and you should easily be able to
 * upgrade them to a real level later.
 */
#define log_off(format, ...)                                                   \
  do {                                                                         \
    if (0) {                                                                   \
      logger_impl(__FILE__, __LINE__, __FUNCTION__, LOGGER_TRACE, format,      \
                  ##__VA_ARGS__);                                              \
    }                                                                          \
  } while (0)

/**
 * @macro log_trace
 *
 * Log at the TRACE level using printf style formatting.
 */
#define log_trace(format, ...)                                                 \
  do {                                                                         \
    if (global_logger_state.level <= LOGGER_TRACE) {                           \
      logger_impl(__FILE__, __LINE__, __FUNCTION__, LOGGER_TRACE, format,      \
                  ##__VA_ARGS__);                                              \
    }                                                                          \
  } while (0)

/**
 * @macro log_debug
 *
 * Log at the DEBUG level using printf style formatting.
 */
#define log_debug(format, ...)                                                 \
  do {                                                                         \
    if (global_logger_state.level <= LOGGER_DEBUG) {                           \
      logger_impl(__FILE__, __LINE__, __FUNCTION__, LOGGER_DEBUG, format,      \
                  ##__VA_ARGS__);                                              \
    }                                                                          \
  } while (0)

/**
 * @macro log_info
 *
 * Log at the INFO level using printf style formatting.
 */
#define log_info(format, ...)                                                  \
  do {                                                                         \
    if (global_logger_state.level <= LOGGER_INFO) {                            \
      logger_impl(__FILE__, __LINE__, __FUNCTION__, LOGGER_INFO, format,       \
                  ##__VA_ARGS__);                                              \
    }                                                                          \
  } while (0)

/**
 * Determine if logging at the INFO level is enabled.
 */
static inline boolean_t should_log_info() {
  return global_logger_state.level <= LOGGER_INFO;
}

/**
 * @macro log_warn
 *
 * Log at the WARN level using printf style formatting.
 */
#define log_warn(format, ...)                                                  \
  do {                                                                         \
    if (global_logger_state.level <= LOGGER_WARN) {                            \
      logger_impl(__FILE__, __LINE__, __FUNCTION__, LOGGER_WARN, format,       \
                  ##__VA_ARGS__);                                              \
    }                                                                          \
  } while (0)

/**
 * @macro log_fatal
 *
 * Log at the FATAL level using printf style formatting.
 *
 * Typically this is only done before invoking fatal_error though I
 * don't have a convenient way to enforce this.
 */
#define log_fatal(format, ...)                                                 \
  do {                                                                         \
    if (global_logger_state.level <= LOGGER_FATAL) {                           \
      logger_impl(__FILE__, __LINE__, __FUNCTION__, LOGGER_FATAL, format,      \
                  ##__VA_ARGS__);                                              \
    }                                                                          \
  } while (0)

/**
 * @macro log_test
 *
 * Log at the TEST level using printf style formatting. This should
 * only be used inside of test code to communicate very basic
 * information back to the user when running a test and is therefore
 * independent of the actual log level.
 */
#define log_test(format, ...)                                                  \
  do {                                                                         \
    logger_impl(__FILE__, __LINE__, __FUNCTION__, LOGGER_TEST, format,         \
                ##__VA_ARGS__);                                                \
  } while (0)

#endif /* _LOGGER_H_ */

logger_state_t global_logger_state
    = compound_literal(logger_state_t, {.level = LOGGER_DEFAULT_LEVEL});

value_result_t parse_log_level_enum(char* str) {
  if (strcmp("FATAL", str) == 0 || strcmp("fatal", str) == 0) {
    return compound_literal(value_result_t, {.u64 = LOGGER_FATAL});
  } else if (strcmp("WARN", str) == 0 || strcmp("warn", str) == 0) {
    return compound_literal(value_result_t, {.u64 = LOGGER_WARN});
  } else if (strcmp("INFO", str) == 0 || strcmp("info", str) == 0) {
    return compound_literal(value_result_t, {.u64 = LOGGER_INFO});
  } else if (strcmp("DEBUG", str) == 0 || strcmp("debug", str) == 0) {
    return compound_literal(value_result_t, {.u64 = LOGGER_DEBUG});
  } else if (strcmp("TRACE", str) == 0 || strcmp("trace", str) == 0) {
    return compound_literal(value_result_t, {.u64 = LOGGER_TRACE});
  } else if (strcmp("OFF", str) == 0 || strcmp("off", str) == 0) {
    return compound_literal(value_result_t, {.u64 = LOGGER_OFF});
  } else {
    return compound_literal(value_result_t,
                            {.nf_error = NF_ERROR_NOT_PARSED_AS_EXPECTED_ENUM});
  }
}

// FORWARD DECLARATION
char* logger_level_to_string(int level);

/**
 * @function logger_init
 *
 * This function modifies the logging level based on the environment
 * variable ARMYKNIFE_LIB_LOG_LEVEL (which currently must be a
 * number).
 *
 * While not required to actually use logging, the logging level will
 * be set to LOGGER_WARN unless you change it in a debugger, and those
 * logging statements will be sent to stderr which is probably not
 * convenient.
 */
void logger_init(void) {
  char* level_string = getenv("ARMYKNIFE_LIB_LOG_LEVEL");
  if (level_string != NULL) {
    value_result_t parsed = string_parse_uint64(level_string);
    if (is_ok(parsed)) {
      global_logger_state.level = parsed.u64;
    } else {
      value_result_t parsed = parse_log_level_enum(level_string);
      if (is_ok(parsed)) {
        global_logger_state.level = parsed.u64;
      } else {
        log_warn("%s could not be converted to a log level.", level_string);
      }
    }
  }

  fprintf(stderr, "Log level is set to %s (%d)\n",
          logger_level_to_string(global_logger_state.level),
          global_logger_state.level);

  char* output_file_name = getenv("ARMYKNIFE_LIB_LOG_FILE");

  // It's pretty standard to include the "pid" in the filename at
  // least when writing to /tmp/. We aren't quite doing that yet...
  //
  // pid_t pid = getpid(); -- pid is a number of some sort...

  if (output_file_name != NULL) {
    global_logger_state.output = fopen(output_file_name, "w");
    if (!global_logger_state.output) {
      fatal_error(ERROR_OPEN_LOG_FILE);
    }
    // Set the stream to unbuffered
    // if (setvbuf(log_file, NULL, _IONBF, 0) != 0) {
    // perror("Failed to set stream to unbuffered");
    // exit(EXIT_FAILURE);
    // }
    global_logger_state.logger_output_filename = output_file_name;
  } else {
    global_logger_state.output = stderr;
    global_logger_state.initialized = true;
  }
}

// Convert the level to a human readable string (which will also
// appear as the name in the log file).
char* logger_level_to_string(int level) {
  switch (level) {
  case LOGGER_OFF:
    return "LOGGER_OFF";
  case LOGGER_TRACE:
    return "TRACE";
  case LOGGER_DEBUG:
    return "DEBUG";
  case LOGGER_INFO:
    return "INFO";
  case LOGGER_WARN:
    return "WARN";
  case LOGGER_FATAL:
    return "FATAL";
  default:
    return "LEVEL_UNKNOWN";
  }
}

/**
 * @function logger_impl
 *
 * This is the non macro version entry point into the logger. Normally
 * it wouldn't be called directly since it is less convenient than the
 * macro versions.
 */
__attribute__((format(printf, 5, 6))) void
    logger_impl(char* file, int line_number, const char* function, int level,
                char* format, ...) {

  FILE* output = global_logger_state.output;

  // Ensure that logging to somewhere will happen though a later call
  // to logger_init() may send the output to somewhere else.
  if (output == NULL) {
    output = stderr;
  }

  if (level >= global_logger_state.level) {
    fprintf(output, "%s ", logger_level_to_string(level));
    va_list args;
    fprintf(output, "%s:%d %s | ", file, line_number, function);

    va_start(args, format);
    vfprintf(output, format, args);
    va_end(args);

    fprintf(output, "\n");
  }
}
/**
 * @file utf8-decoder.c
 *
 * A very basic UTF-8 decoder.
 */
#ifndef _UTF8_DECODER_H_
#define _UTF8_DECODER_H_

#include <stdint.h>

/**
 * @struct utf8_decode_result_t
 *
 * Holds the result of utf8_decode.
 */
struct utf8_decode_result_S {
  uint32_t code_point;
  uint8_t num_bytes;
  boolean_t error;
};

typedef struct utf8_decode_result_S utf8_decode_result_t;

extern utf8_decode_result_t utf8_decode(const uint8_t* utf8_bytes);

#endif /* _UTF8_DECODER_H_ */

/**
 * @function utf8_decode
 *
 * Decodes the next code-point from a uint8_t* pointer.
 */
utf8_decode_result_t utf8_decode(const uint8_t* array) {
  uint8_t firstByte = array[0];
  if ((firstByte & 0x80) == 0) {
    return compound_literal(utf8_decode_result_t,
                            {.code_point = firstByte, .num_bytes = 1});
  } else if ((firstByte & 0xE0) == 0xC0) {
    return compound_literal(
        utf8_decode_result_t,
        {.code_point = ((firstByte & 0x1F) << 6) | (array[1] & 0x3F),
         .num_bytes = 2});
  } else if ((firstByte & 0xF0) == 0xE0) {
    return compound_literal(utf8_decode_result_t,
                            {.code_point = ((firstByte & 0x0F) << 12)
                                           | ((array[1] & 0x3F) << 6)
                                           | (array[2] & 0x3F),
                             .num_bytes = 3});
  } else if ((firstByte & 0xF8) == 0xF0) {
    return compound_literal(
        utf8_decode_result_t,
        {.code_point = ((firstByte & 0x07) << 18) | ((array[1] & 0x3F) << 12)
                       | ((array[2] & 0x3F) << 6) | (array[3] & 0x3F),
         .num_bytes = 4});
  } else {
    return compound_literal(utf8_decode_result_t, {.error = true});
  }
}
#ifndef _BUFFER_H_
#define _BUFFER_H_

/**
 * @file buffer.c
 *
 * A growable array of bytes.
 *
 * Buffers are useful for constructing UTF-8 strings (for example by
 * using buffer_printf) though they can hold any binary data including
 * interior NUL bytes. Buffers automatically grow as data is appended
 * (or inserted into) them reducing large classes of errors.
 */

#include <ctype.h>
#include <stdint.h>
#include <string.h>

struct buffer_S {
  uint32_t length;
  uint32_t capacity;
  uint8_t* elements;
};

/**
 * @struct buffer_t
 *
 * Buffers are non-thread-safe memory regions that generally "grow"
 * over time. Whenever a buffer grows beyound it's capacity, it is
 * moved to accomodates its new capacity (even if there may be other
 * pointers to it's insides, so don't do that unless you have decided
 * the buffer will never grow again).
 *
 * While buffers may seem scary, used properly from a single thread,
 * they are actually quite predictable.
 */
typedef struct buffer_S buffer_t;

extern buffer_t* make_buffer(uint64_t initial_capacity);

extern uint64_t buffer_length(buffer_t* buffer);

extern uint8_t buffer_get(buffer_t* buffer, uint64_t position);

extern char* buffer_c_substring(buffer_t* buffer, uint64_t start, uint64_t end);

extern char* buffer_to_c_string(buffer_t* buffer);

extern void buffer_clear(buffer_t* buffer);

extern buffer_t* buffer_increase_capacity(buffer_t* buffer, uint64_t capacity);

extern buffer_t* buffer_append_byte(buffer_t* buffer, uint8_t byte);

extern buffer_t* buffer_append_bytes(buffer_t* buffer, uint8_t* bytes,
                                     uint64_t n_bytes);

extern buffer_t* buffer_append_buffer(buffer_t* buffer, buffer_t* src_buffer);

extern buffer_t* buffer_append_sub_buffer(buffer_t* buffer,
                                          uint64_t start_position,
                                          uint64_t end_position,
                                          buffer_t* src_buffer);

extern buffer_t* buffer_append_string(buffer_t* buffer, const char* str);

__attribute__((format(printf, 2, 3))) extern buffer_t*
    buffer_printf(buffer_t* buffer, char* format, ...);

extern buffer_t* buffer_append_repeated_byte(buffer_t* buffer, uint8_t byte,
                                             int count);

utf8_decode_result_t buffer_utf8_decode(buffer_t* buffer, uint64_t position);

extern buffer_t* buffer_append_code_point(buffer_t* buffer,
                                          uint32_t code_point);

boolean_t buffer_match_string_at(buffer_t* buffer, uint64_t start_position,
                                 char* str);

buffer_t* buffer_from_string(char* string);

buffer_t* buffer_adjust_region(buffer_t* buffer, uint64_t original_start,
                               uint64_t original_end, uint64_t new_width);

buffer_t* buffer_replace_all(buffer_t* buffer, char* original_text,
                             char* replacement_text);

buffer_t* buffer_replace_matching_byte(buffer_t* buffer, uint8_t original,
                                       uint8_t replacement);

boolean_t buffer_region_contains(buffer_t* buffer, uint64_t start, uint64_t end,
                                 char* text);

uint64_t buffer_beginning_of_line(buffer_t* buffer, uint64_t start);

uint64_t buffer_end_of_line(buffer_t* buffer, uint64_t start);

buffer_t* buffer_to_uppercase(buffer_t* buffer);

buffer_t* buffer_to_lowercase(buffer_t* buffer);

typedef struct line_and_column_S {
  uint64_t line;
  uint64_t column;
} line_and_column_t;

line_and_column_t buffer_position_to_line_and_column(buffer_t* buffer,
                                                     uint64_t position);

#endif /* _BUFFER_H_ */

// ======================================================================

#include <stdlib.h>

/**
 * @function make_buffer
 *
 * Make an empty byte array with the given initial capacity.
 */
buffer_t* make_buffer(uint64_t initial_capacity) {
  buffer_t* result = malloc_struct(buffer_t);
  if (initial_capacity < 16) {
    initial_capacity = 16;
  }
  if (initial_capacity > 0) {
    result->capacity = initial_capacity;
    result->elements = malloc_bytes(initial_capacity);
  }
  return result;
}

/**
 * @function buffer_length
 *
 * Return the number of bytes that have been added to this byte array.
 */
uint64_t buffer_length(buffer_t* array) { return array->length; }

/**
 * @function buffer_length
 *
 * Clear a buffer without resizing it.
 */
void buffer_clear(buffer_t* buffer) {
  for (int i = 0; i < buffer->capacity; i++) {
    buffer->elements[i] = 0;
  }
  buffer->length = 0;
}

/**
 * @function buffer_get
 *
 * Get a single byte from a byte array.
 */
uint8_t buffer_get(buffer_t* buffer, uint64_t position) {
  if (position < buffer->length) {
    return buffer->elements[position];
  } else {
    fatal_error(ERROR_ACCESS_OUT_OF_BOUNDS);
#ifdef __TINYC__
    /* gcc and clang know fatal_error is _Noreturn but tcc doesn't */
    return 0;
#endif
  }
}

/**
 * @function buffer_c_substring
 *
 * Extract a newly allocated string that contains the bytes from start
 * to end (appending a zero byte to make sure it's a legal C string).
 */
char* buffer_c_substring(buffer_t* buffer, uint64_t start, uint64_t end) {
  if (buffer == NULL) {
    fatal_error(ERROR_ILLEGAL_NULL_ARGUMENT);
  }

  if (start > end) {
    fatal_error(ERROR_ILLEGAL_RANGE);
  }

  uint64_t copy_length = (end - start);
  char* result = cast(char*, malloc_bytes(copy_length + 1));
  if (copy_length > 0) {
    memcpy(result, &buffer->elements[start], copy_length);
  }
  result[copy_length] = '\0';
  return result;
}

/**
 * @function buffer_to_c_string
 *
 * Extract a newly allocated string that contain all of the bytes in the byte
 * buffer as a NUL (zero byte) terminated C string.
 */
char* buffer_to_c_string(buffer_t* buffer) {
  return buffer_c_substring(buffer, 0, buffer->length);
}

/**
 * @function buffer_append_byte
 *
 * Append a single byte to the byte array.
 */
buffer_t* buffer_append_byte(buffer_t* buffer, uint8_t element) {
  if (buffer->length < buffer->capacity) {
    buffer->elements[buffer->length] = element;
    buffer->length++;
    return buffer;
  }
  buffer = buffer_increase_capacity(buffer, buffer->capacity * 2);
  return buffer_append_byte(buffer, element);
}

/**
 * @function buffer_append_bytes
 *
 * Append multiple bytes to the byte array.
 */
buffer_t* buffer_append_bytes(buffer_t* buffer, uint8_t* bytes,
                              uint64_t n_bytes) {
  // Obviously this can be optimized...
  for (int i = 0; i < n_bytes; i++) {
    buffer = buffer_append_byte(buffer, bytes[i]);
  }
  return buffer;
}

/**
 * @function buffer_append_string
 *
 * Append all of the bytes from a C string (except the ending NUL
 * char).
 */
buffer_t* buffer_append_string(buffer_t* buffer, const char* str) {
  return buffer_append_bytes(buffer, cast(uint8_t*, str), strlen(str));
}

/**
 * @function buffer_increase_capacity
 *
 * Allow the capacity of a buffer to be increased (or return if the
 * buffer already exceeds the given capacity). Generally this is not
 * necessary to properly use a buffer (and occasionally could actually
 * hurt performance if done incorrectly).
 */
extern buffer_t* buffer_increase_capacity(buffer_t* buffer, uint64_t capacity) {
  if (buffer->capacity < capacity) {
    uint8_t* new_elements = malloc_bytes(capacity);
    memcpy(new_elements, buffer->elements, buffer->length);
    free_bytes(buffer->elements);
    buffer->elements = new_elements;
    buffer->capacity = capacity;
  }
  return buffer;
}

#ifndef BUFFER_PRINTF_INITIAL_BUFFER_SIZE
#define BUFFER_PRINTF_INITIAL_BUFFER_SIZE 1024
#endif

/**
 * @function buffer_printf
 *
 * Format like printf but append the result to the passed in buffer
 * (returning a new buffer in case the buffer exceeded it's capacity).
 */
__attribute__((format(printf, 2, 3))) buffer_t*
    buffer_printf(buffer_t* buffer, char* format, ...) {
  char cbuffer[BUFFER_PRINTF_INITIAL_BUFFER_SIZE];
  int n_bytes = 0;
  do {
    va_list args;
    va_start(args, format);
    n_bytes
        = vsnprintf(cbuffer, BUFFER_PRINTF_INITIAL_BUFFER_SIZE, format, args);
    va_end(args);
  } while (0);

  if (n_bytes < BUFFER_PRINTF_INITIAL_BUFFER_SIZE) {
    return buffer_append_string(buffer, cbuffer);
  } else {
    // Be lazy for now and just copy the code from string_printf for
    // this case but we should be able to do ensure capacity and just
    // put the bytes directly at the end of the buffer...
    char* result = cast(char*, malloc_bytes(n_bytes + 1));
    va_list args;
    va_start(args, format);
    int n_bytes_second = vsnprintf(result, n_bytes + 1, format, args);
    va_end(args);
    if (n_bytes_second != n_bytes) {
      fatal_error(ERROR_INTERNAL_ASSERTION_FAILURE);
    }
    buffer = buffer_append_string(buffer, result);
    free_bytes(result);
    return buffer;
  }
}

/**
 * @function buffer_append_repeated_byte
 *
 * Append 'count' copies of byte to the passed in buffer. This can be
 * used for things like indentation or horizontal rules (composed from
 * say '-', '=', or '*').
 */
extern buffer_t* buffer_append_repeated_byte(buffer_t* buffer, uint8_t byte,
                                             int count) {
  for (int i = 0; i < count; i++) {
    buffer = buffer_append_byte(buffer, byte);
  }
  return buffer;
}

/**
 * @function buffer_utf8_decode
 *
 * Similar to utf8_decode but operating on a buffer_t. (This routine
 * adds bounds checking beyound utf8_decode).
 */
utf8_decode_result_t buffer_utf8_decode(buffer_t* buffer, uint64_t position) {
  if (position >= buffer->length) {
    return compound_literal(utf8_decode_result_t, {.error = true});
  }
  utf8_decode_result_t result = utf8_decode(&buffer->elements[position]);
  if (result.error) {
    return result;
  }
  if ((position + result.num_bytes) > buffer->length) {
    return compound_literal(utf8_decode_result_t, {.error = true});
  }
  return result;
}

/**
 * @function buffer_append_code_point()
 *
 * Append a single code-point according to UTF-8 encoding (so 1 to 4
 * bytes). While you can put anything you want into a buffer_t (not
 * just valid UTF-8 sequences), if you then try to make a C string
 * from the buffer then it might end up with a NUL ('\0') byte in the
 * middle of it if you add code_point == 0 somewhere besides the end
 * of the string.
 *
 * @see utf8_decode(const uint8_t* utf8_bytes).
 */
extern buffer_t* buffer_append_code_point(buffer_t* buffer,
                                          uint32_t code_point) {
  if (code_point < 0x80) {
    // 1-byte sequence for code points in the range 0-127
    buffer = buffer_append_byte(buffer, code_point);
    return buffer;
  } else if (code_point < 0x800) {
    // 2-byte sequence for code points in the range 128-2047
    buffer = buffer_append_byte(buffer, 0xc0 | (code_point >> 6));
    buffer = buffer_append_byte(buffer, 0x80 | (code_point & 0x3f));
    return buffer;
  } else if (code_point < 0x10000) {
    // 3-byte sequence for code points in the range 2048-65535
    buffer = buffer_append_byte(buffer, 0xe0 | (code_point >> 12));
    buffer = buffer_append_byte(buffer, 0x80 | ((code_point >> 6) & 0x3f));
    buffer = buffer_append_byte(buffer, 0x80 | (code_point & 0x3f));
    return buffer;
  } else if (code_point <= 0x10FFFF) {
    // 4-byte sequence for code points in the range 65536-1114111
    buffer = buffer_append_byte(buffer, 0xf0 | (code_point >> 18));
    buffer = buffer_append_byte(buffer, 0x80 | ((code_point >> 12) & 0x3f));
    buffer = buffer_append_byte(buffer, 0x80 | ((code_point >> 6) & 0x3f));
    buffer = buffer_append_byte(buffer, 0x80 | (code_point & 0x3f));
    return buffer;
  } else {
    // Code points beyond the valid UTF-8 range (0-0x10FFFF) are not supported
    fatal_error(ERROR_ILLEGAL_UTF_8_CODE_POINT);
    return 0; // Not Reached.
  }
}
/**
 * @function buffer_match_string_at
 *
 * Determine if the buffer contains "str" at start_position.
 */
boolean_t buffer_match_string_at(buffer_t* buffer, uint64_t start_position,
                                 char* str) {
  for (uint64_t pos = start_position; true; pos++) {
    uint8_t byte_str = cast(uint8_t*, str)[pos - start_position];
    if (byte_str == 0) {
      return true;
    }
    if (pos >= buffer_length(buffer)) {
      return false;
    }
    uint8_t byte_buf = buffer_get(buffer, pos);
    if (byte_str != byte_buf) {
      return false;
    }
  }
  /* NOT REACHED */
  return false;
}

/**
 * @function buffer_from_string
 *
 * Return a buffer initialized with the contents of a particular
 * string. The trailing NUL byte C string terminator byte is not
 * included in the buffer.
 */
buffer_t* buffer_from_string(char* string) {
  buffer_t* result = make_buffer(strlen(string));
  result = buffer_append_string(result, string);
  return result;
}

/**
 * @function buffer_adjust_region
 *
 * This is primarily used as a helper for buffer_replace_all though it
 * can also be used to efficiently delete a region or simply "open up"
 * space within a buffer.
 */
buffer_t* buffer_adjust_region(buffer_t* buffer, uint64_t start, uint64_t end,
                               uint64_t new_width) {
  // TODO(jawilson): more range testing.
  if (start > end) {
    fatal_error(ERROR_ILLEGAL_RANGE);
  }
  uint64_t original_width = end - start;
  if (original_width > new_width) {
    // Copy the tail of the buffer downwards and then decrease the
    // length
    uint64_t difference = original_width - new_width;
    uint64_t tail_length = buffer->length - end;
    memmove(&buffer->elements[end - difference], &buffer->elements[end],
            tail_length);
    buffer->length -= difference;
  } else if (original_width < new_width) {
    // Increase the capacity of the buffer if necessary and then copy
    // the tail of the buffer upwards and finally increase the length
    uint64_t difference = new_width - original_width;
    uint64_t tail_length = buffer->length - end;
    buffer = buffer_increase_capacity(buffer, buffer->length + difference);
    memmove(&buffer->elements[end + difference], &buffer->elements[end],
            tail_length);
    buffer->length += difference;
  }
  return buffer;
}

/**
 * @function buffer_replace_all
 *
 * Find all occurences of original_text and replace them with
 * replacement_text.
 */
buffer_t* buffer_replace_all(buffer_t* buffer, char* original_text,
                             char* replacement_text) {
  int len_original = strlen(original_text);
  int len_replacement = strlen(replacement_text);
  if (len_original < buffer->length) {
    uint64_t pos = 0;
    while (pos <= (buffer->length - len_original)) {
      if (buffer_match_string_at(buffer, pos, original_text)) {
        buffer = buffer_adjust_region(buffer, pos, pos + len_original,
                                      len_replacement);
        memmove(&buffer->elements[pos], replacement_text, len_replacement);
        pos += len_replacement;
      } else {
        pos++;
      }
    }
  }
  return buffer;
}

line_and_column_t buffer_position_to_line_and_column(buffer_t* buffer,
                                                     uint64_t position) {
  uint64_t line = 1;
  uint64_t column = 1;

  // TODO(jawilson): process as code points
  // TODO(jawilson): write unit test
  for (uint64_t pos = 0; pos < position; pos++) {
    uint8_t ch = buffer_get(buffer, pos);
    if (ch == '\n') {
      line++;
      column = 1;
    } else {
      column++;
    }
  }
  return compound_literal(line_and_column_t, {
                                                 .line = line,
                                                 .column = column,
                                             });
}

/**
 * @function buffer_region_contains
 *
 * Determine if a buffer contains the specified text within a region.
 */
boolean_t buffer_region_contains(buffer_t* buffer, uint64_t start, uint64_t end,
                                 char* text) {
  for (int i = start; i < end; i++) {
    if (buffer_match_string_at(buffer, i, text)) {
      return true;
    }
  }
  return false;
}

/**
 * @function buffer_replace_matching_byte
 *
 * Replace all occurences of the original byte value with the
 * replacement value. This is useful for example to turn NUL bytes
 * into something more useful like a space character so the buffer can
 * be turned into a C string without getting truncated.
 */
buffer_t* buffer_replace_matching_byte(buffer_t* buffer, uint8_t original,
                                       uint8_t replacement) {
  for (int i = 0; i < buffer->length; i++) {
    if (buffer->elements[i] == original) {
      buffer->elements[i] = replacement;
    }
  }
  return buffer;
}

/**
 * @function buffer_beginning_of_line
 *
 * Look backwards starting at the start position until we reach a
 * position where the previous character is a newline or the beginning
 * of the buffer.
 */
uint64_t buffer_beginning_of_line(buffer_t* buffer, uint64_t start) {
  uint64_t position = start;
  while (position > 0) {
    position--;
    if (buffer_get(buffer, position) == '\n') {
      return position + 1;
    }
  }
  return position;
}

/**
 * @function buffer_end_of_line
 *
 * Look forwards starting at the start position until we reach the
 * position of the first newline. If we reach the end of the buffer
 * without encountering a newline, simply return the length of the
 * buffer.
 */
uint64_t buffer_end_of_line(buffer_t* buffer, uint64_t start) {
  uint64_t position = start;
  while (position < buffer->length && buffer_get(buffer, position) != '\n') {
    position++;
  }
  return position;
}

/**
 * @function buffer_append_sub_buffer
 *
 * Append all of the bytes of the src_buffer to a buffer.
 *
 * It is currently illegal to append parts of a buffer to itself.
 */
extern buffer_t* buffer_append_buffer(buffer_t* buffer, buffer_t* src_buffer) {
  return buffer_append_sub_buffer(buffer, 0, src_buffer->length, src_buffer);
}

/**
 * @function buffer_append_sub_buffer
 *
 * Append all of the bytes between src_buffer[start_position,
 * end_position) to the dst_buffer. It is illegal to append parts of a
 * buffer to itself because of how buffer's are implemented which we
 * are considering changing for this and other reasons.
 */
extern buffer_t* buffer_append_sub_buffer(buffer_t* buffer,
                                          uint64_t start_position,
                                          uint64_t end_position,
                                          buffer_t* src_buffer) {
  if (buffer == src_buffer) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  for (uint64_t position = start_position; position < end_position;
       position++) {
    buffer = buffer_append_byte(buffer, buffer_get(src_buffer, position));
  }
  return buffer;
}

/**
 * @function buffer_to_uppercase
 *
 * Call toupper on each character in the buffer.
 *
 * TODO(jawilson): make sure this process full unicode even if it can
 * only uppercase ASCII latin characters.
 */

buffer_t* buffer_to_uppercase(buffer_t* buffer) {
  for (uint64_t i = 0; i < buffer->length; i++) {
    buffer->elements[i] = toupper(buffer->elements[i]);
  }
  return buffer;
}

/**
 * @function buffer_to_lowercase
 *
 * Call tolower on each character in the buffer.
 *
 * TODO(jawilson): make sure this process full unicode even if it can
 * only uppercase ASCII latin characters.
 */

buffer_t* buffer_to_lowercase(buffer_t* buffer) {
  for (uint64_t i = 0; i < buffer->length; i++) {
    buffer->elements[i] = tolower(buffer->elements[i]);
  }
  return buffer;
}

/**
 * @file value-array.c
 *
 * This file contains a growable array of "values".
 *
 * Certain algorithms require that growth occurs based on the current
 * capacity of an array, not a fixed amount. For now we simply double
 * the current capacity when more space in the backing array is
 * required though we may scale this back to something more like 1.5X
 * for "large" arrays to save space.
 */

#ifndef _VALUE_ARRAY_H_
#define _VALUE_ARRAY_H_

struct value_array_S {
  uint32_t length;
  uint32_t capacity;
  value_t* elements;
};

/**
 * @typedef value_array_t
 *
 * A growable array of 64bit "values" (so integers, doubles, and
 * pointers).
 */
typedef struct value_array_S value_array_t;

extern value_array_t* make_value_array(uint64_t initial_capacity);
extern value_t value_array_get(value_array_t* array, uint32_t index);
extern void value_array_replace(value_array_t* array, uint32_t index,
                                value_t element);
extern void value_array_add(value_array_t* array, value_t element);
extern void value_array_push(value_array_t* array, value_t element);
extern value_t value_array_pop(value_array_t* array);
extern void value_array_insert_at(value_array_t* array, uint32_t position,
                                  value_t element);
extern value_t value_array_delete_at(value_array_t* array, uint32_t position);

#define value_array_get_ptr(array, index_expression, cast_type)                \
  (cast(cast_type, value_array_get(array, index_expression).ptr))

#endif /* _VALUE_ARRAY_H_ */

/**
 * @function make_value_array
 *
 * Make a value array with the given initial capacity.
 *
 * An initial capacity of zero is automatically converted to a
 * capacity of at least 1 or more since this makes the "growth" code a
 * bit simpler.
 *
 * When the array runs out of capacity because of calls to add, push,
 * etc., then the backing array is automatically increased *based on
 * the current capacity* which generally leads to better big-O
 * properties.
 *
 * If the initial or later increases in capacity are not fulfillable,
 * then a fatal_error occurs since these are generally not recoverable
 * anyways.
 */
value_array_t* make_value_array(uint64_t initial_capacity) {
  if (initial_capacity == 0) {
    initial_capacity = 1;
  }

  value_array_t* result = malloc_struct(value_array_t);
  result->capacity = initial_capacity;
  result->elements
      = cast(value_t*, malloc_bytes(sizeof(value_t) * initial_capacity));

  return result;
}

void value_array_ensure_capacity(value_array_t* array,
                                 uint32_t required_capacity) {
  if (array->capacity < required_capacity) {
    uint32_t new_capacity = array->capacity * 2;
    if (new_capacity < required_capacity) {
      new_capacity = required_capacity;
    }
    value_t* new_elements
        = cast(value_t*, malloc_bytes(sizeof(value_t) * new_capacity));
    for (int i = 0; i < array->length; i++) {
      new_elements[i] = array->elements[i];
    }
    array->capacity = new_capacity;
    free_bytes(array->elements);
    array->elements = new_elements;
    return;
  }
}

/**
 * @function value_array_get
 *
 * Get the value stored at index `index`. If the index is outside of
 * the range of valid elements, then a fatal_error is signaled.
 */
value_t value_array_get(value_array_t* array, uint32_t index) {
  if (index < array->length) {
    return array->elements[index];
  }
  fatal_error(ERROR_ACCESS_OUT_OF_BOUNDS);
  /* gcc and clang know fatal_error is _Noreturn but tcc doesn't */
  return compound_literal(value_t, {0});
}

/**
 * @function value_array_replace
 *
 * Replace the value at a given `index`. If the index is outside of
 * the range of valid elements, then a `fatal_error` is signaled.
 */
void value_array_replace(value_array_t* array, uint32_t index,
                         value_t element) {
  if (index < array->length) {
    array->elements[index] = element;
    return;
  }
  fatal_error(ERROR_ACCESS_OUT_OF_BOUNDS);
}

/**
 * @function value_array_add
 *
 * Add an element to the end of an array. If more space is required
 * then the backing array is automatically resized. This resizing
 * means that a fatal_error() may occur if malloc() can not satisfy the
 * allocation request.
 */
void value_array_add(value_array_t* array, value_t element) {
  value_array_ensure_capacity(array, array->length + 1);
  array->elements[(array->length)++] = element;
}

/**
 * @function value_array_push
 *
 * This is a synonym for value_array_add which serves to make it more
 * obvious that the array is actually being used like a stack.
 */
void value_array_push(value_array_t* array, value_t element) {
  value_array_add(array, element);
}

/**
 * @function value_array_pop
 *
 * Returns the last element of the array (typically added via push)
 * and modifies the length of the array so that the value isn't
 * accessible any longer. (We also "zero out" the element in case you
 * are using a conservative garbage collector.)
 *
 * If the array is currently empty, then
 * `fatal_error(ERROR_ACCESS_OUT_OF_BOUNDS)` is called.
 */
value_t value_array_pop(value_array_t* array) {
  if (array->length == 0) {
    fatal_error(ERROR_ACCESS_OUT_OF_BOUNDS);
  }
  uint32_t last_index = array->length - 1;
  value_t result = value_array_get(array, last_index);
  array->elements[last_index] = u64_to_value(0);
  array->length--;
  return result;
}

/**
 * @function value_array_insert_at
 *
 * Insert an element into some existing position in the array (or at
 * the end if position == the current array length).
 *
 * This operation is not efficient for large arrays as potentially the
 * entire array must be moved to new locations (there are
 * data-structures like trees that can make this more efficient though
 * such a data-structure isn't in this library yet).
 *
 * If the position is > than the length length, then
 * fatal_error(ERROR_ACCESS_OUT_OF_BOUNDS) is called.
 *
 * If memory allocation is required and malloc() fails, then
 * fatal_error(ERROR_MEMORY_ALLOCATION) is called.
 */
void value_array_insert_at(value_array_t* array, uint32_t position,
                           value_t element) {
  if (position == array->length) {
    value_array_add(array, element);
    return;
  }

  if (position > array->length) {
    fatal_error(ERROR_ACCESS_OUT_OF_BOUNDS);
    return;
  }

  value_array_ensure_capacity(array, array->length + 1);

  // This is the standard loop but we now need to use a signed index
  // because when the position is zero, zero - 1 is 0xffffffff which
  // is still greater than zero (and hence greater than position).
  for (int64_t i = array->length - 1; i >= position; i--) {
    array->elements[i + 1] = array->elements[i];
  }
  array->length++;
  array->elements[position] = element;
}

/**
 * @function value_array_delete_at
 *
 * Deletes the element at the given position (and return it so that it
 * can potentially be freed by the caller).
 *
 * If the position doesn't point to a valid element then
 * fatal_error(ERROR_ACCESS_OUT_OF_BOUNDS) is called.
 */
value_t value_array_delete_at(value_array_t* array, uint32_t position) {
  value_t result = value_array_get(array, position);
  for (int i = position; i < array->length - 1; i++) {
    array->elements[i] = array->elements[i + 1];
  }
  array->length--;
  return result;
}
/**
 * @file value-alist.c
 *
 * An association list (a type of map) from value_t to value_t.
 */

#ifndef _VALUE_ALIST_H_
#define _VALUE_ALIST_H_

struct value_alist_S {
  struct value_alist_S* next;
  value_t key;
  value_t value;
};

typedef struct value_alist_S value_alist_t;

extern value_result_t value_alist_find(value_alist_t* list,
                                       value_comparison_fn cmp_fn, value_t key);

__attribute__((warn_unused_result)) extern value_alist_t*
    value_alist_insert(value_alist_t* list, value_comparison_fn cmp_fn,
                       value_t key, value_t value);

__attribute__((warn_unused_result)) extern value_alist_t*
    value_alist_delete(value_alist_t* list, value_comparison_fn cmp_fn,
                       value_t key);

__attribute__((warn_unused_result)) extern uint64_t
    value_alist_length(value_alist_t* list);

/**
 * @macro value_alist_foreach
 *
 * Allows iteration over the keys and values in a string association
 * list.
 */
#define value_alist_foreach(alist, key_var, value_var, statements)             \
  do {                                                                         \
    value_alist_t* head = alist;                                               \
    while (head) {                                                             \
      value_t key_var = head->key;                                             \
      value_t value_var = head->value;                                         \
      statements;                                                              \
      head = head->next;                                                       \
    }                                                                          \
  } while (0)

#endif /* _VALUE_ALIST_H_ */

/**
 * @function value_alist_insert
 *
 * Insert a new key and value into an assocation list.
 */
value_alist_t* value_alist_insert(value_alist_t* list,
                                  value_comparison_fn cmp_fn, value_t key,
                                  value_t value) {
  value_alist_t* result = malloc_struct(value_alist_t);
  result->next = value_alist_delete(list, cmp_fn, key);
  result->key = key;
  result->value = value;
  return result;
}

/**
 * @function value_alist_delete
 *
 * Delete the key and associated value from the given association
 * list. Neither the key nor the value associated are themselves
 * freed.
 */
value_alist_t* value_alist_delete(value_alist_t* list,
                                  value_comparison_fn cmp_fn, value_t key) {
  // This appears to be logically correct but could easily blow out
  // the stack with a long list.
  if (list == NULL) {
    return list;
  }
  if ((*cmp_fn)(key, list->key) == 0) {
    value_alist_t* result = list->next;
    free_bytes(list);
    return result;
  }
  list->next = value_alist_delete(list->next, cmp_fn, key);
  return list;
}

/**
 * @function value_alist_find
 *
 * Find the value associate with the given key. Use is_ok() or
 * is_not_ok() to see if the value is valid (i.e., if the key was
 * actually found).
 */
value_result_t value_alist_find(value_alist_t* list, value_comparison_fn cmp_fn,
                                value_t key) {
  while (list) {
    if (cmp_fn(key, list->key) == 0) {
      return compound_literal(value_result_t, {.val = list->value});
    }
    list = list->next;
  }
  return compound_literal(value_result_t, {.nf_error = NF_ERROR_NOT_FOUND});
}

/**
 * @function value_alist_length
 *
 * Determine the length of an alist.
 *
 * The alist argument MAY be null.
 */
__attribute__((warn_unused_result)) extern uint64_t
    value_alist_length(value_alist_t* list) {
  uint64_t result = 0;
  while (list) {
    result++;
    list = list->next;
  }
  return result;
}
/**
 * @file string-alist.c
 *
 * An association list (a type of map) from a string to a value_t.
 *
 * This simply wraps value-alist.c.
 */

#ifndef _STRING_ALIST_H_
#define _STRING_ALIST_H_

struct string_alist_S {};

typedef struct string_alist_S string_alist_t;

/**
 * @function alist_find
 *
 * Find the value associate with the given key. Use is_ok() or
 * is_not_ok() to see if the value is valid (i.e., if the key was
 * actually found).
 */
static inline value_result_t alist_find(string_alist_t* list, char* key) {
  return value_alist_find(cast(value_alist_t*, list), cmp_string_values,
                          str_to_value(key));
}

/**
 * @function alist_insert
 *
 * Insert a new key and value into an assocation list.
 */
__attribute__((warn_unused_result)) static inline string_alist_t*
    alist_insert(string_alist_t* list, char* key, value_t value) {
  return cast(string_alist_t*,
              value_alist_insert(cast(value_alist_t*, list), cmp_string_values,
                                 str_to_value(key), value));
}

/**
 * @function alist_delete
 *
 * Delete the key and associated value from the given association
 * list. Neither the key nor the value associated are themselves
 * freed.
 */
__attribute__((warn_unused_result)) static inline string_alist_t*
    alist_delete(string_alist_t* list, char* key) {
  return cast(string_alist_t*,
              value_alist_delete(cast(value_alist_t*, list), cmp_string_values,
                                 str_to_value(key)));
}

/**
 * @function alist_length
 *
 * Determine the length of an alist.
 *
 * The alist argument MAY be null.
 */
__attribute__((warn_unused_result)) static inline uint64_t
    alist_length(string_alist_t* list) {
  return value_alist_length(cast(value_alist_t*, list));
}

/**
 * @macro string_alist_foreach
 *
 * Allows iteration over the keys and values in a string association
 * list.
 */
#define string_alist_foreach(alist, key_var, value_var, statements)            \
  do {                                                                         \
    value_alist_foreach(cast(value_alist_t*, alist), key_var##_value,          \
                        value_var, {                                           \
                          char* key_var = (key_var##_value).str;               \
                          statements;                                          \
                        });                                                    \
  } while (0)

#endif /* _STRING_ALIST_H_ */
/**
 * @file value-hashtable.c
 *
 * A very thread-unsafe hash map of value_t to value_t.
 *
 * Please don't expect C++, JVM, or Rust level of performance since we
 * use chaining which is considered slower than open addressing.
 */

#ifndef _VALUE_HASHTABLE_H_
#define _VALUE_HASHTABLE_H_

/**
 * @compiliation_option ARMYKNIFE_HT_LOAD_FACTOR
 *
 * The "load factor" is the ratio of the number of keys in the hash
 * table to the most optimistic capacity for the table if every key
 * happened to be hashed to a different bucket. When the load factor
 * reaches this value, the hash table will be resized to a larger
 * capacity to improve performance. A higher value allows for a denser
 * hash table but can lead to more collisions and slower lookups and
 * insertions. A lower value wastes memory but reduces collisions.
 */
#ifndef ARMYKNIFE_HT_LOAD_FACTOR
#define ARMYKNIFE_HT_LOAD_FACTOR 0.75
#endif /* ARMYKNIFE_HT_LOAD_FACTOR */

/**
 * @compiliation_option AK_HT_UPSCALE_MULTIPLIER
 *
 * In all cases this should be a number > 1.0.
 */
#ifndef AK_HT_UPSCALE_MULTIPLIER
#define AK_HT_UPSCALE_MULTIPLIER 1.75
#endif /* AK_HT_UPSCALE_MULTIPLIER */

struct value_hashtable_S {
  uint64_t n_buckets;
  uint64_t n_entries;
  value_alist_t** buckets;
};

typedef struct value_hashtable_S value_hashtable_t;

extern value_hashtable_t* make_value_hashtable(uint64_t n_buckets);

extern value_hashtable_t* value_ht_insert(value_hashtable_t* ht,
                                          value_hash_fn hash_fn,
                                          value_comparison_fn cmp_fn,
                                          value_t key, value_t value);

extern value_hashtable_t* value_ht_delete(value_hashtable_t* ht,
                                          value_hash_fn hash_fn,
                                          value_comparison_fn cmp_fn,
                                          value_t key);

extern value_result_t value_ht_find(value_hashtable_t* ht,
                                    value_hash_fn hash_fn,
                                    value_comparison_fn cmp_fn, value_t key);

extern void value_hashtable_upsize_internal(value_hashtable_t* ht,
                                            value_hash_fn hash_fn,
                                            value_comparison_fn cmp_fn);

/**
 * @function value_ht_num_entries
 *
 * Returns the number of entries in the hashtable.
 */
static inline uint64_t value_ht_num_entries(value_hashtable_t* ht) {
  return ht->n_entries;
}

/**
 * @macro value_ht_foreach
 *
 * Allows traversing all elements of a hashtable in an unspecified
 * order.
 */
#define value_ht_foreach(ht, key_var, value_var, statements)                   \
  do {                                                                         \
    for (int ht_index = 0; ht_index < ht->n_buckets; ht_index++) {             \
      value_alist_t* alist = ht->buckets[ht_index];                            \
      if (alist != NULL) {                                                     \
        value_alist_foreach(alist, key_var, value_var, statements);            \
      }                                                                        \
    }                                                                          \
  } while (0)

#endif /* _VALUE_HASHTABLE_H_ */

/**
 * @function make_value_hashtable
 *
 * Create a hashtable with the given number of buckets.
 *
 * When the initial number of buckets is less than a small integer
 * (currently 2), then we automatically increase the initial number of
 * buckets to make the fractional growth algorithm work easier to
 * reason about.
 */
value_hashtable_t* make_value_hashtable(uint64_t n_buckets) {
  if (n_buckets < 2) {
    n_buckets = 2;
  }
  value_hashtable_t* result = malloc_struct(value_hashtable_t);
  result->n_buckets = n_buckets;
  result->buckets
      = cast(value_alist_t**,
             malloc_bytes(sizeof(typeof(value_alist_t*)) * n_buckets));
  return result;
}

/**
 * @function value_ht_insert
 *
 * Insert an association into the hashtable.
 */
value_hashtable_t* value_ht_insert(value_hashtable_t* ht, value_hash_fn hash_fn,
                                   value_comparison_fn cmp_fn, value_t key,
                                   value_t value) {
  uint64_t hashcode = hash_fn(key);
  int bucket = hashcode % ht->n_buckets;
  value_alist_t* list = ht->buckets[bucket];
  uint64_t len = value_alist_length(list);
  list = value_alist_insert(list, cmp_fn, key, value);
  ht->buckets[bucket] = list;
  uint64_t len_after = value_alist_length(list);
  if (len_after > len) {
    ht->n_entries++;
    // Without this, a hash table would never grow and thus as the
    // number of entries grows large, the hashtable would only improve
    // performance over an alist by a constant amount (which could
    // still be an impressive speedup...)
    if (ht->n_entries >= (ht->n_buckets * ARMYKNIFE_HT_LOAD_FACTOR)) {
      value_hashtable_upsize_internal(ht, hash_fn, cmp_fn);
    }
  }
  return ht;
}

/**
 * @function value_ht_delete
 *
 * Delete an association from the hashtable. It is not an error to
 * delete a key that doesn't exist in the hashtable.
 */
value_hashtable_t* value_ht_delete(value_hashtable_t* ht, value_hash_fn hash_fn,
                                   value_comparison_fn cmp_fn, value_t key) {
  uint64_t hashcode = hash_fn(key);
  int bucket = hashcode % ht->n_buckets;
  value_alist_t* list = ht->buckets[bucket];
  uint64_t len = value_alist_length(list);
  list = value_alist_delete(list, cmp_fn, key);
  ht->buckets[bucket] = list;
  uint64_t len_after = value_alist_length(list);
  if (len_after < len) {
    ht->n_entries--;
  }
  return ht;
}

/**
 * @function value_ht_find
 *
 * Find an association in the hashtable.
 */
value_result_t value_ht_find(value_hashtable_t* ht, value_hash_fn hash_fn,
                             value_comparison_fn cmp_fn, value_t key) {
  uint64_t hashcode = hash_fn(key);
  int bucket = hashcode % ht->n_buckets;
  value_alist_t* list = ht->buckets[bucket];
  return value_alist_find(list, cmp_fn, key);
}

/**
 * @function value_hashtable_upsize_internal
 *
 * This function is called automatically when an insert brings the
 * number of entries above the number of buckets times
 * ARMYKNIFE_HT_LOAD_FACTOR (defaults to 75%). We don't even check
 * that constraint is valid (hence the _internal suffix).
 *
 * Hopefully based on the name you can infer this function will only
 * ever "grow" a hashtable by deciding on a size of the new larger
 * hash-table and copying all of the entries to the bigger hashtable
 * (using AK_HT_UPSCALE_MULTIPLIER to compute the new number of
 * buckets, currently 1.75).
 */
void value_hashtable_upsize_internal(value_hashtable_t* ht,
                                     value_hash_fn hash_fn,
                                     value_comparison_fn cmp_fn) {
  uint64_t new_num_buckets = ht->n_buckets * AK_HT_UPSCALE_MULTIPLIER;
  value_hashtable_t* new_ht = make_value_hashtable(new_num_buckets);
  // clang-format off
  value_ht_foreach(ht, key, value, block_expr({
      value_hashtable_t* should_be_result = value_ht_insert(new_ht, hash_fn, cmp_fn, key, value);
      // If an insertion into the bigger hashtable results in it's own
      // resize, then the results will be unpredictable (at least
      // without more code). This is likely to only happen when
      // growing a very small hashtable and depends on values choosen
      // for ARMYKNIFE_HT_LOAD_FACTOR and AK_HT_UPSCALE_MULTIPLIER.
      if (new_ht != should_be_result) {
	fatal_error(ERROR_ILLEGAL_STATE);
      }
  }));
  // clang-format on
  value_alist_t** old_buckets = ht->buckets;
  ht->buckets = new_ht->buckets;
  ht->n_buckets = new_ht->n_buckets;
  ht->n_entries = new_ht->n_entries;
  free_bytes(old_buckets);
  free_bytes(new_ht);
}
/**
 * @file string-hashtable.c
 *
 * A very thread-unsafe hash map of C style zero terminated byte
 * "strings" to a value_t. This is an oqaque reference around a
 * value_hashtable.
 */

#ifndef _STRING_HASHTABLE_H_
#define _STRING_HASHTABLE_H_

struct string_hashtable_S {};

typedef struct string_hashtable_S string_hashtable_t;

static inline value_hashtable_t* to_value_hashtable(string_hashtable_t* ht) {
  return cast(value_hashtable_t*, ht);
}

/**
 * @function make_string_hashtable
 *
 * Create a hashtable with the given number of buckets.
 *
 * The minimum number of buckets is currently 2 to make it less likely
 * we run into some resize loop depending on the values of
 * ARMYKNIFE_HT_LOAD_FACTOR and AK_HT_UPSCALE_MULTIPLIER).
 */
static inline string_hashtable_t* make_string_hashtable(uint64_t n_buckets) {
  return cast(string_hashtable_t*, make_value_hashtable(n_buckets));
}

/**
 * @function string_ht_insert
 *
 * Insert an association into the hashtable.
 */
__attribute__((warn_unused_result)) static inline string_hashtable_t*
    string_ht_insert(string_hashtable_t* ht, char* key, value_t value) {
  return cast(string_hashtable_t*,
              value_ht_insert(to_value_hashtable(ht), hash_string_value,
                              cmp_string_values, str_to_value(key), value));
}

/**
 * @function string_ht_delete
 *
 * Delete an association from the hashtable. It is not an error to
 * delete a key that doesn't exist in the hashtable.
 */
__attribute__((warn_unused_result)) static inline string_hashtable_t*
    string_ht_delete(string_hashtable_t* ht, char* key) {
  return cast(string_hashtable_t*,
              value_ht_delete(to_value_hashtable(ht), hash_string_value,
                              cmp_string_values, str_to_value(key)));
}

/**
 * @function string_ht_find
 *
 * Find an association in the hashtable.
 */
static inline value_result_t string_ht_find(string_hashtable_t* ht, char* key) {
  return value_ht_find(to_value_hashtable(ht), hash_string_value,
                       cmp_string_values, str_to_value(key));
}

/**
 * @function string_ht_num_entries
 *
 * Returns the number of entries in the hashtable.
 */
static inline uint64_t string_ht_num_entries(string_hashtable_t* ht) {
  return value_ht_num_entries(to_value_hashtable(ht));
}

/**
 * @macro string_ht_foreach
 *
 * Allows traversing all elements of a hashtable in an unspecified
 * order.
 */
#define string_ht_foreach(ht, key_var, value_var, statements)                  \
  do {                                                                         \
    value_ht_foreach(to_value_hashtable(ht), key_var##_value, value_var, {     \
      char* key_var = (key_var##_value).str;                                   \
      statements;                                                              \
    });                                                                        \
  } while (0)

#endif /* _STRING_HASHTABLE_H_ */

/**
 * @file value-tree.c
 *
 * This is a balanced binary tree to associate a value to another
 * value.
 *
 * Generally an alist is prefered for small "maps", and hashtables are
 * prefered for large maps, but value_tree is the easiest way to get
 * sorted results (which is most important for reproducibility).
 *
 * Currently we are using "AA" trees (see
 * https://en.wikipedia.org/wiki/AA_tree) since it has simpler code
 * than many other balanced trees (like red-block trees) and the
 * Wikipedia article and paper spell out *most* of the non-trivial
 * details.
 */

#ifndef _VALUE_TREE_H_
#define _VALUE_TREE_H_

struct value_tree_S {
  value_t key;
  value_t value;
  uint32_t level;
  struct value_tree_S* left;
  struct value_tree_S* right;
};

typedef struct value_tree_S value_tree_t;

extern value_result_t value_tree_find(value_tree_t* t,
                                      value_comparison_fn cmp_fn, value_t key);

__attribute__((warn_unused_result)) extern value_tree_t*
    value_tree_insert(value_tree_t* t, value_comparison_fn cmp_fn, value_t key,
                      value_t value);

__attribute__((warn_unused_result)) extern value_tree_t*
    value_tree_delete(value_tree_t* t, value_comparison_fn cmp_fn, value_t key);

/**
 * @macro value_tree_foreach
 *
 * Perform an inorder traversal of a value-tree.
 *
 * key_var is created in a new block scope with type value_t.
 *
 * value_var is created in a new block scope with type value_t and you
 * will probably want to use something like ".ptr" or ".u64" on the
 * value to obtain the actual value.
 *
 * statements should be a normal C block, aka, something like:
 * ```
 * {
 *   statement1();
 *   statement2();
 * }
 * ```
 *
 * Unforunately it is not possible to use "break" or "continue" with
 * this style of loop (and worse, there will be no compilation error
 * or warning if you accidentally do that...)
 */
#define value_tree_foreach(tree, key_var, value_var, statements)               \
  do {                                                                         \
    int stack_n_elements = 0;                                                  \
    value_tree_t* stack[64];                                                   \
    value_tree_t* current = tree;                                              \
    while (current != NULL || stack_n_elements > 0) {                          \
      while (current != NULL) {                                                \
        stack[stack_n_elements++] = current;                                   \
        current = current->left;                                               \
      }                                                                        \
      current = stack[--stack_n_elements];                                     \
      value_t key_var = current->key;                                          \
      value_t value_var = current->value;                                      \
      statements;                                                              \
      current = current->right;                                                \
    }                                                                          \
  } while (0)

#endif /* _VALUE_TREE_H_ */

/**
 * @function value_tree_find
 *
 * Find the value associate with the key in the tree.
 */
value_result_t value_tree_find(value_tree_t* t, value_comparison_fn cmp_fn,
                               value_t key) {
  if (t == NULL) {
    return compound_literal(value_result_t, {.nf_error = NF_ERROR_NOT_FOUND});
  }

  int cmp_result = cmp_fn(key, t->key);
  if (cmp_result < 0) {
    return value_tree_find(t->left, cmp_fn, key);
  } else if (cmp_result > 0) {
    return value_tree_find(t->right, cmp_fn, key);
  } else {
    return compound_literal(value_result_t, {
                                                .val = t->value,
                                            });
  }
}

value_tree_t* value_tree_skew(value_tree_t* t) {
  if (t == NULL) {
    return NULL;
  }
  if (t->left == NULL) {
    return t;
  }
  if (t->left->level == t->level) {
    value_tree_t* L = t->left;
    t->left = L->right;
    L->right = t;
    return L;
  }
  return t;
}

value_tree_t* value_tree_split(value_tree_t* t) {
  if (t == NULL) {
    return NULL;
  }
  if (t->right == NULL || t->right->right == NULL) {
    return t;
  }
  if (t->level == t->right->right->level) {
    // We have two horizontal right links.  Take the middle node,
    // elevate it, and return it.
    value_tree_t* R = t->right;
    t->right = R->left;
    R->left = t;
    R->level++;
    return R;
  }
  return t;
}

value_tree_t* make_value_tree_leaf(value_t key, value_t value) {
  value_tree_t* result = malloc_struct(value_tree_t);
  result->level = 1;
  result->key = key;
  result->value = value;
  return result;
}

/**
 * @function value_tree_insert
 *
 * Insert an association of key and a value (or update the current
 * value stored in the tree).
 */
value_tree_t* value_tree_insert(value_tree_t* t, value_comparison_fn cmp_fn,
                                value_t key, value_t value) {
  if (t == NULL) {
    // Create a new leaf node
    return make_value_tree_leaf(key, value);
  }
  int cmp_result = cmp_fn(key, t->key);
  if (cmp_result < 0) {
    t->left = value_tree_insert(t->left, cmp_fn, key, value);
  } else if (cmp_result > 0) {
    t->right = value_tree_insert(t->right, cmp_fn, key, value);
  } else {
    // Either key or t->key might need to be freed but it isn't even
    // possible to tell if either has been "malloced" so good luck
    // figuring that out.
    t->value = value;
    return t;
  }

  t = value_tree_skew(t);
  t = value_tree_split(t);

  return t;
}

static inline uint64_t value_tree_min_level(uint32_t a, uint32_t b) {
  return a < b ? a : b;
}

value_tree_t* value_tree_decrease_level(value_tree_t* t) {
  if (t->left && t->right) {
    uint32_t should_be
        = value_tree_min_level(t->left->level, t->right->level) + 1;
    if (should_be < t->level) {
      t->level = should_be;
      if (should_be < t->right->level) {
        t->right->level = should_be;
      }
    }
  }
  return t;
}

value_tree_t* value_tree_predecessor(value_tree_t* t) {
  t = t->left;
  while (t->right != NULL) {
    t = t->right;
  }
  return t;
}

value_tree_t* value_tree_successor(value_tree_t* t) {
  t = t->right;
  while (t->left != NULL) {
    t = t->left;
  }
  return t;
}

static inline boolean_t value_tree_is_leaf(value_tree_t* t) {
  return t->left == NULL && t->right == NULL;
}

/**
 * @function value_tree_delete
 *
 * Delete the association of key (if it exists in the tree). It is not
 * an error to delete a key that isn't present in the table.
 */
value_tree_t* value_tree_delete(value_tree_t* t, value_comparison_fn cmp_fn,
                                value_t key) {

  if (t == NULL) {
    return t;
  }

  int cmp_result = cmp_fn(key, t->key);
  if (cmp_result < 0) {
    t->left = value_tree_delete(t->left, cmp_fn, key);
  } else if (cmp_result > 0) {
    t->right = value_tree_delete(t->right, cmp_fn, key);
  } else {
    if (value_tree_is_leaf(t)) {
      // Since we are a leaf, nothing special to do except make sure
      // this leaf node is no longer in the tree. wikipedia says
      // "return right(T)" which is technically correct, but this is
      // clearer.
      return NULL;
    } else if (t->left == NULL) {
      value_tree_t* L = value_tree_successor(t);
      // Note: wikipedia or the orginal article may have a bug. Doing
      // the delete and then the key/value assignment leads to a
      // divergence with a reference implementation.
      t->key = L->key;
      t->value = L->value;
      t->right = value_tree_delete(t->right, cmp_fn, L->key);
    } else {
      value_tree_t* L = value_tree_predecessor(t);
      // Note: wikipedia or the orginal article may have a bug. Doing
      // the delete and then the key/value assignment leads to a
      // divergence with a reference implementation.
      t->key = L->key;
      t->value = L->value;
      t->left = value_tree_delete(t->left, cmp_fn, L->key);
    }
  }

  // Rebalance the tree. Decrease the level of all nodes in this level
  // if necessary, and then skew and split all nodes in the new level.

  t = value_tree_decrease_level(t);
  t = value_tree_skew(t);
  t->right = value_tree_skew(t->right);
  if (t->right != NULL) {
    t->right->right = value_tree_skew(t->right->right);
  }
  t = value_tree_split(t);
  t->right = value_tree_split(t->right);
  return t;
}

/**
 * @file string-tree.c
 *
 * This is a balanced binary tree to associate a string to a value
 * using an underlying value-tree.c.
 */

#ifndef _STRING_TREE_H_
#define _STRING_TREE_H_

struct string_tree_S {};

typedef struct string_tree_S string_tree_t;

/**
 * @function string_tree_find
 *
 * Find the value associate with the key in the tree.
 */
static inline value_result_t string_tree_find(string_tree_t* t, char* key) {
  return value_tree_find(cast(value_tree_t*, t), cmp_string_values,
                         str_to_value(key));
}

/**
 * @function string_tree_insert
 *
 * Insert an association of key and a value (or update the current
 * value stored in the tree).
 */
__attribute__((warn_unused_result)) static inline string_tree_t*
    string_tree_insert(string_tree_t* t, char* key, value_t value) {
  return cast(string_tree_t*,
              value_tree_insert(cast(value_tree_t*, t), cmp_string_values,
                                str_to_value(key), value));
}

/**
 * @function string_tree_delete
 *
 * Delete the association of key (if it exists in the tree). It is not
 * an error to delete a key that isn't present in the table.
 */
__attribute__((warn_unused_result)) static inline string_tree_t*
    string_tree_delete(string_tree_t* t, char* key) {
  return cast(string_tree_t*,
              value_tree_delete(cast(value_tree_t*, t), cmp_string_values,
                                str_to_value(key)));
}

/**
 * @macro string_tree_foreach
 *
 * Perform an inorder traversal of a string-tree.
 *
 * key_var is created in a new block scope with type char*.
 *
 * value_var is created in a new block scope with type value_t and you
 * will probably want to use something like ".ptr" or ".u64" on the
 * value to obtain the actual value.
 *
 * statements should be a normal C block, aka, something like:
 * ```
 * {
 *   statement1();
 *   statement2();
 * }
 * ```
 *
 * Unforunately it is not possible to use "break" or "continue" with
 * this style of loop (and worse, there will be no compilation error
 * or warning if you accidentally do that...)
 */
#define string_tree_foreach(tree, key_var, value_var, statements)              \
  do {                                                                         \
    value_tree_foreach(cast(value_tree_t*, tree), key_var##_value, value_var,  \
                       {                                                       \
                         char* key_var = (key_var##_value).str;                \
                         statements;                                           \
                       });                                                     \
  } while (0)

#endif /* _STRING_TREE_H_ */
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
/**
 * @file io.c
 *
 * This contains routines to read the contents of a file or write a
 * new file.
 */

// ======================================================================
// This is block is extraced to allocate.h
// ======================================================================

#ifndef _IO_H_
#define _IO_H_

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>

__attribute__((warn_unused_result)) extern buffer_t*
    buffer_append_file_contents(buffer_t* bytes, char* file_name);

__attribute__((warn_unused_result)) extern buffer_t*
    buffer_append_all(buffer_t* buffer, FILE* input);

extern void buffer_write_file(buffer_t* bytes, char* file_name);

__attribute__((warn_unused_result)) extern buffer_t*
    buffer_read_until(buffer_t* buffer, FILE* input, char end_of_line);

extern buffer_t* buffer_read_ready_bytes(buffer_t* buffer, FILE* input,
                                         uint64_t max_bytes);

extern buffer_t* buffer_read_ready_bytes_file_number(buffer_t* buffer,
                                                     int file_number,
                                                     uint64_t max_bytes);

int file_peek_byte(FILE* input);

boolean_t file_eof(FILE* input);

void file_copy_stream(FILE* input, FILE* output, boolean_t until_eof,
                      uint64_t size);

void file_skip_bytes(FILE* input, uint64_t n_bytes);

#endif /* _IO_H_ */

// ======================================================================

#include <stdio.h>

// This is optional...
#include <sys/stat.h>

/**
 * @function buffer_append_file_contents
 *
 * Completely reads a file and appends the contents to the passed in
 * buffer. This is often much more convenient than streaming a file.
 */
__attribute__((warn_unused_result)) buffer_t*
    buffer_append_file_contents(buffer_t* bytes, char* file_name) {

  uint64_t capacity = bytes->capacity;

  // This is optional
  {
    struct stat st;
    if (stat(file_name, &st) < 0) {
      log_fatal("file does not exist: %s", file_name);
      fatal_error(ERROR_ILLEGAL_STATE);
    }
    capacity = st.st_size;
  }

  bytes = buffer_increase_capacity(bytes, capacity);

  FILE* file = fopen(file_name, "r");
  bytes = buffer_append_all(bytes, file);
  fclose(file);

  return bytes;
}

/**
 * @function buffer_append_all
 *
 * Completely reads everything from the input FILE* putting everything
 * into a buffer.
 */
__attribute__((warn_unused_result)) extern buffer_t*
    buffer_append_all(buffer_t* bytes, FILE* input) {
  uint8_t buffer[1024];
  while (1) {
    uint64_t n_read = fread(buffer, 1, sizeof(buffer), input);
    if (n_read == 0) {
      break;
    }
    bytes = buffer_append_bytes(bytes, buffer, n_read);
  }
  return bytes;
}

/**
 * @function buffer_write_file
 *
 * Writes the contents of the buffer to the given file.
 */
void buffer_write_file(buffer_t* bytes, char* file_name) {
  FILE* file = fopen(file_name, "w");
  if (file == NULL) {
    log_fatal("Failed to open file for writing: %s", file_name);
    log_fatal("strerror(errno) = %s", strerror(errno));
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  size_t bytes_written = fwrite(bytes->elements, 1, bytes->length, file);
  if (bytes_written != bytes->length) {
    log_fatal("Failed to write %d bytes to %s", bytes->length, file_name);
    log_fatal("strerror(errno) = %s", strerror(errno));
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  if (fclose(file) != 0) {
    log_fatal("Failed to close file: %s", file_name);
    log_fatal("strerror(errno) = %s", strerror(errno));
    fatal_error(ERROR_ILLEGAL_STATE);
  }
}

/**
 * @function buffer_read_until
 *
 * Read from a FILE* until either the end of file is reached or a
 * particular "end-of-line" character is read. Every character except
 * the end_of_line character is appended to the buffer.
 *
 * This can be used to read normal Unix "lines" or another use is to
 * read "lines" that end with NUL (U+0000) or until say a seperator
 * like "," is encountered.
 *
 * This function (and string_tokenize) should take a character set
 * which can unify their interface and allow U+0000 to be a member of
 * the set (unlike using a C string (aka char*) as the character set).
 *
 * TODO(jawilson): write a proper test!
 */
buffer_t* buffer_read_until(buffer_t* buffer, FILE* input, char end_of_line) {
  while (!feof(input)) {
    int ch = fgetc(input);
    if (ch < 0) {
      return buffer;
    }
    if (ch == end_of_line) {
      return buffer;
    }
    buffer = buffer_append_byte(buffer, ch);
  }
  return buffer;
}

/**
 * @function buffer_read_ready_bytes
 *
 * Read from a FILE* until either the end of file is reached,
 * max_bytes has been read, or there are no ready bytes. This function
 * should never block.
 */
extern buffer_t* buffer_read_ready_bytes(buffer_t* buffer, FILE* input,
                                         uint64_t max_bytes) {
  int file_number = fileno(input);
  return buffer_read_ready_bytes_file_number(buffer, file_number, max_bytes);
}

/**
 * @function buffer_read_ready_bytes_file_number
 *
 * Read from a file_number until either the end of file is reached,
 * max_bytes has been read, or there are no ready bytes. This function
 * should never block.
 */
extern buffer_t* buffer_read_ready_bytes_file_number(buffer_t* buffer,
                                                     int file_number,
                                                     uint64_t max_bytes) {
  fcntl(file_number, F_SETFL, fcntl(file_number, F_GETFL) | O_NONBLOCK);

  uint64_t bytes_remaining = max_bytes - buffer_length(buffer);
  char read_buffer[1024];

  // Loop until either blocking would occur or max_bytes have been added
  while (bytes_remaining > 0) {
    int bytes_read = read(file_number, read_buffer, sizeof(read_buffer));
    if (bytes_read > 0) {
      for (int i = 0; i < bytes_read; i++) {
        buffer = buffer_append_byte(buffer, cast(uint8_t, read_buffer[i]));
        bytes_remaining--;
      }
    } else if (bytes_read == 0) {
      // End-of-file (write end of pipe closed)
      break;
    } else {
      // bytes_read < 0 (so presumably -1).
      if (errno != EAGAIN && errno != EWOULDBLOCK) {
        // A real error occurred
        log_fatal("Error reading from file descriptor %d: %s", file_number,
                  strerror(errno));
        fatal_error(ERROR_ILLEGAL_STATE);
      }
      // No data available without blocking, so break out of the loop
      break;
    }
  }

  return buffer;
}

/**
 * @function file_peek_char
 *
 * Returns the next byte from the input (as an int not u8_t) or -1 if
 * the end of the file input has been reached. A a byte is read, then
 * the byte is "pushed back" into the input stream so that if
 * file_peek_char, fgetc, or a host of other functions attempt to read
 * the input then
 */
int file_peek_byte(FILE* input) {
  if (feof(input)) {
    return -1;
  }
  int result = fgetc(input);
  // ungetc doesn't "push back" -1 according to
  // https://en.cppreference.com/w/c/io/ungetc
  // But who is going to trust that...
  if (result >= 0) {
    ungetc(result, input);
  }
  return result;
}

/**
 * @function file_eof
 *
 * Return true if an input stream is at the end of the file. I don't
 * know what "feof" really does but it doesn't actually do this.
 */
boolean_t file_eof(FILE* input) {
  return feof(input) || file_peek_byte(input) < 0;
}

#define FILE_COPY_STREAM_BUFFER_SIZE 1024

/**
 * @funtion file_copy_stream
 *
 * Copy some or all of an input stream to an output stream.
 */
void file_copy_stream(FILE* input, FILE* output, boolean_t until_eof,
                      uint64_t size) {
  if (until_eof) {
    size = ULLONG_MAX;
  }

  uint8_t buffer[FILE_COPY_STREAM_BUFFER_SIZE];
  while (size > 0) {
    int minimum = size < FILE_COPY_STREAM_BUFFER_SIZE
                      ? size
                      : FILE_COPY_STREAM_BUFFER_SIZE;
    uint64_t n_read = fread(buffer, 1, minimum, input);
    if (n_read == 0) {
      break;
    }
    fwrite(buffer, 1, n_read, output);
    size -= n_read;
  }
}

/**
 * @function file_skip_bytes
 *
 * Skip n_bytes from the given input stream unless the end of the file
 * is reached first in which case we silently return without
 * indicating that condition. Gemini claims that fseek may not work on
 * "stdin" and further claims there may be a seekable proprety
 * associated with an input stream (which seems to match up when
 * running on Debian Bookworm...).
 */
void file_skip_bytes(FILE* input, uint64_t n_bytes) {

  // We'd try to do it like this but Gemini claims that this doesn't
  // reliably work for stdin. That is bonkers!
  //
  // fseek(in, size, SEEK_CUR);

  while (1) {
    if (n_bytes == 0 || feof(input)) {
      return;
    }
    int ch = fgetc(input);
    if (ch < 0) {
      // TODO(jawilson): fixme?
      return;
    }
    n_bytes--;
  }
}
#ifndef _TERMINAL_H_
#define _TERMINAL_H_

#include <termios.h>

/**
 * @file terminal.c
 *
 * A simplistic terminal library that doesn't depend on ncurses,
 * etc. Realistically most terminals are now decendants of xterm
 * (itelf like vt100 or ANSI deep down) and tend to converge on how
 * new features work so we are going to just initialy code for Konsole
 * and go from there.
 *
 * Based on this document, I feel we can assume we have 24bit color
 * support and handle other terminals differently in the future
 * (perhaps by just not emitting colors):
 *
 * https://github.com/termstandard/colors
 */

/**
 * @struct box_drawing_t
 *
 * A structure to hold unicode code-points for each part of a box to
 * draw in the terminal.
 */
struct box_drawing_S {
  uint32_t upper_left_corner;
  uint32_t upper_right_corner;
  uint32_t lower_left_corner;
  uint32_t lower_right_corner;
  uint32_t top_edge;
  uint32_t left_edge;
  uint32_t right_edge;
  uint32_t bottom_edge;
};

typedef struct box_drawing_S box_drawing_t;

/**
 * @struct term_keypress_t
 *
 * Represents a keypress from the terminal. Either one of code_point
 * or key_code will be non-zero if n_bytes_consumed is greater than
 * zero.
 */
struct term_keypress_S {
  uint32_t code_point;
  uint8_t key_code;
  uint8_t n_bytes_consumed;
  uint8_t shift : 1;
  uint8_t ctrl : 1;
  uint8_t meta : 1;
  uint8_t super : 1;
  uint8_t hyper : 1;
};

// Names and descriptions are from ncurses but the values are
// different.
#define KEY_DOWN 1      /* down-arrow key */
#define KEY_UP 2        /* up-arrow key */
#define KEY_LEFT 3      /* left-arrow key */
#define KEY_RIGHT 4     /* right-arrow key */
#define KEY_HOME 5      /* home key */
#define KEY_BACKSPACE 6 /* backspace key */
#define KEY_F0 7        /* Function keys */
#define KEY_F1 8
#define KEY_F2 9
#define KEY_F3 10
#define KEY_F4 11
#define KEY_F5 12
#define KEY_F6 13
#define KEY_F7 14
#define KEY_F8 15
#define KEY_F9 16
#define KEY_F10 17
#define KEY_F11 18
#define KEY_F12 19
#define KEY_F13 20
#define KEY_F14 21
#define KEY_F15 22
#define KEY_DL 23        /* delete-line key */
#define KEY_IL 24        /* insert-line key */
#define KEY_DC 25        /* delete-character key */
#define KEY_IC 26        /* insert-character key */
#define KEY_EIC 27       /* sent by rmir or smir in insert mode */
#define KEY_CLEAR 28     /* clear-screen or erase key */
#define KEY_EOS 29       /* clear-to-end-of-screen key */
#define KEY_EOL 30       /* clear-to-end-of-line key */
#define KEY_SF 31        /* scroll-forward key */
#define KEY_SR 32        /* scroll-backward key */
#define KEY_NPAGE 33     /* next-page key */
#define KEY_PPAGE 34     /* previous-page key */
#define KEY_STAB 35      /* set-tab key */
#define KEY_CTAB 36      /* clear-tab key */
#define KEY_CATAB 37     /* clear-all-tabs key */
#define KEY_ENTER 38     /* enter/send key */
#define KEY_PRINT 39     /* print key */
#define KEY_LL 40        /* lower-left key (home down) */
#define KEY_A1 41        /* upper left of keypad */
#define KEY_A3 42        /* upper right of keypad */
#define KEY_B2 43        /* center of keypad */
#define KEY_C1 44        /* lower left of keypad */
#define KEY_C3 45        /* lower right of keypad */
#define KEY_BTAB 46      /* back-tab key */
#define KEY_BEG 47       /* begin key */
#define KEY_CANCEL 48    /* cancel key */
#define KEY_CLOSE 49     /* close key */
#define KEY_COMMAND 50   /* command key */
#define KEY_COPY 51      /* copy key */
#define KEY_CREATE 52    /* create key */
#define KEY_END 53       /* end key */
#define KEY_EXIT 54      /* exit key */
#define KEY_FIND 55      /* find key */
#define KEY_HELP 56      /* help key */
#define KEY_MARK 57      /* mark key */
#define KEY_MESSAGE 58   /* message key */
#define KEY_MOVE 59      /* move key */
#define KEY_NEXT 60      /* next key */
#define KEY_OPEN 61      /* open key */
#define KEY_OPTIONS 62   /* options key */
#define KEY_PREVIOUS 63  /* previous key */
#define KEY_REDO 64      /* redo key */
#define KEY_REFERENCE 65 /* reference key */
#define KEY_REFRESH 66   /* refresh key */
#define KEY_REPLACE 67   /* replace key */
#define KEY_RESTART 68   /* restart key */
#define KEY_RESUME 69    /* resume key */
#define KEY_SAVE 70      /* save key */
#define KEY_SBEG 71      /* shifted begin key */
#define KEY_SCANCEL 72   /* shifted cancel key */
#define KEY_SCOMMAND 73  /* shifted command key */
#define KEY_SCOPY 74     /* shifted copy key */
#define KEY_SCREATE 75   /* shifted create key */
#define KEY_SDC 76       /* shifted delete-character key */
#define KEY_SDL 77       /* shifted delete-line key */
#define KEY_SELECT 78    /* select key */
#define KEY_SEND 79      /* shifted end key */
#define KEY_SEOL 80      /* shifted clear-to-end-of-line key */
#define KEY_SEXIT 81     /* shifted exit key */
#define KEY_SFIND 82     /* shifted find key */
#define KEY_SHELP 83     /* shifted help key */
#define KEY_SHOME 84     /* shifted home key */
#define KEY_SIC 85       /* shifted insert-character key */
#define KEY_SLEFT 86     /* shifted left-arrow key */
#define KEY_SMESSAGE 87  /* shifted message key */
#define KEY_SMOVE 88     /* shifted move key */
#define KEY_SNEXT 89     /* shifted next key */
#define KEY_SOPTIONS 90  /* shifted options key */
#define KEY_SPREVIOUS 91 /* shifted previous key */
#define KEY_SPRINT 92    /* shifted print key */
#define KEY_SREDO 93     /* shifted redo key */
#define KEY_SREPLACE 94  /* shifted replace key */
#define KEY_SRIGHT 95    /* shifted right-arrow key */
#define KEY_SRSUME 96    /* shifted resume key */
#define KEY_SSAVE 97     /* shifted save key */
#define KEY_SSUSPEND 98  /* shifted suspend key */
#define KEY_SUNDO 99     /* shifted undo key */
#define KEY_SUSPEND 100  /* suspend key */
#define KEY_UNDO 101     /* undo key */
#define KEY_MOUSE 102    /* Mouse event has occurred */
#define KEY_RESIZE 103   /* Terminal resize event */

typedef struct term_keypress_S term_keypress_t;

__attribute__((warn_unused_result)) extern buffer_t*
    term_clear_screen(buffer_t* buffer);

__attribute__((warn_unused_result)) extern buffer_t*
    term_set_foreground_color(buffer_t* buffer, uint32_t color);

__attribute__((warn_unused_result)) extern buffer_t*
    term_set_background_color(buffer_t* buffer, uint32_t color);

__attribute__((warn_unused_result)) extern buffer_t*
    term_move_cursor_absolute(buffer_t* buffer, int x, int y);

__attribute__((warn_unused_result)) extern buffer_t*
    term_move_cursor_relative(buffer_t* buffer, int x, int y);

__attribute__((warn_unused_result)) extern buffer_t*
    term_bold(buffer_t* buffer);

__attribute__((warn_unused_result)) extern buffer_t* term_dim(buffer_t* buffer);

__attribute__((warn_unused_result)) extern buffer_t*
    term_italic(buffer_t* buffer);

__attribute__((warn_unused_result)) extern buffer_t*
    term_underline(buffer_t* buffer);

__attribute__((warn_unused_result)) extern buffer_t*
    term_reset_formatting(buffer_t* buffer);

__attribute__((warn_unused_result)) extern buffer_t*
    term_draw_box(buffer_t* buffer, uint16_t x0, uint16_t y0, uint16_t x1,
                  uint16_t y1, box_drawing_t* box);

extern struct termios term_echo_off();
extern void term_echo_restore(struct termios oldt);

// TODO(jawilson): terminal queries like request cursor position

#endif /* _TERMINAL_H_ */

#define TERM_ESCAPE_START_STR "\033["
#define TERM_ESCAPE_END_STR "m"

#define TERM_ESCAPE_STRING_START_AND_END(str)                                  \
  (TERM_ESCAPE_START_STR str TERM_ESCAPE_END_STR)
#define TERM_ESCAPE_STRING(str) (TERM_ESCAPE_START_STR str)

/**
 * @function term_set_foreground_color
 *
 * Append a terminal escape sequence to a buffer that instructs the
 * terminal to change to foreground color to a 24bit RGB value.
 *
 * (Currently we don't automatically "downgrade" to 256 color or 16
 * color terminals).
 */
__attribute__((warn_unused_result)) extern buffer_t*
    term_set_foreground_color(buffer_t* buffer, uint32_t color) {
  uint8_t blue = color & 0xff;
  uint8_t green = (color >> 8) & 0xff;
  uint8_t red = (color >> 16) & 0xff;

  // Escape sequence for setting foreground color (ESC [ 38; 2; r; g; b m)
  return buffer_printf(buffer,
                       TERM_ESCAPE_STRING_START_AND_END("38;2;%d;%d;%d"), red,
                       green, blue);
}

/**
 * @function term_set_background_color
 *
 * Append a terminal escape sequence to a buffer that instructs the
 * terminal to change to background color to a 24bit RGB value.
 *
 * (Currently we don't automatically "downgrade" to 256 color or 16
 * color terminals).
 */
__attribute__((warn_unused_result)) extern buffer_t*
    term_set_background_color(buffer_t* buffer, uint32_t color) {
  uint8_t blue = color & 0xff;
  uint8_t green = (color >> 8) & 0xff;
  uint8_t red = (color >> 16) & 0xff;

  // Escape sequence for setting background color (ESC [ 48; 2; r; g; b m)
  return buffer_printf(buffer,
                       TERM_ESCAPE_STRING_START_AND_END("48;2;%d;%d;%d"), red,
                       green, blue);
}

/**
 * @function term_move_cursor_absolute
 *
 * Append a terminal escape sequence to a buffer that instructs the
 * terminal to move it's "cursor" to a zero based x, y absolute
 * coordinate.
 *
 * (Traditionally terminals operate on a 1 based coordinate system but
 * zero based indexes would perhaps be choosen now since most
 * programming languages are zero based).
 */
__attribute__((warn_unused_result)) extern buffer_t*
    term_move_cursor_absolute(buffer_t* buffer, int x, int y) {
  // Escape sequence for cursor movement (ESC [ y; x H)
  return buffer_printf(buffer, TERM_ESCAPE_STRING("%d;%dH"), y + 1, x + 1);
}

/**
 * @function term_move_cursor_relative
 *
 * Append a terminal escape sequence to a buffer that instructs the
 * terminal to move it's "cursor" relative to it's current poition. 0
 * doesn't change the column or row, while negative will move either
 * left or to the "top" of the terminal, while positive numbers move
 * right or downwards towards the bottom of the terminal.
 */
__attribute__((warn_unused_result)) extern buffer_t*
    term_move_cursor_relative(buffer_t* buffer, int x, int y) {
  // First handle the x position
  if (x > 0) {
    buffer = buffer_printf(buffer, TERM_ESCAPE_STRING("%dC"), x);
  } else if (x < 0) {
    buffer = buffer_printf(buffer, TERM_ESCAPE_STRING("%dD"), -x);
  }
  if (y > 0) {
    buffer = buffer_printf(buffer, TERM_ESCAPE_STRING("%dB"), y);
  } else {
    buffer = buffer_printf(buffer, TERM_ESCAPE_STRING("%dA"), -y);
  }
  return buffer;
}

/**
 * @function term_bold
 *
 * Append a terminal escape sequence to a buffer that turns on "bold"
 * text.
 */
__attribute__((warn_unused_result)) extern buffer_t*
    term_bold(buffer_t* buffer) {
  return buffer_printf(buffer, TERM_ESCAPE_STRING("1m"));
}

/**
 * @function term_dim
 *
 * Append a terminal escape sequence to a buffer that turns on "dim"
 * text.
 */
__attribute__((warn_unused_result)) extern buffer_t*
    term_dim(buffer_t* buffer) {
  return buffer_printf(buffer, TERM_ESCAPE_STRING("2m"));
}

/**
 * @function term_dim
 *
 * Append a terminal escape sequence to a buffer that turns on "italic"
 * text.
 */
__attribute__((warn_unused_result)) extern buffer_t*
    term_italic(buffer_t* buffer) {
  return buffer_printf(buffer, TERM_ESCAPE_STRING("3m"));
}

/**
 * @function term_underline
 *
 * Append a terminal escape sequence to a buffer that turns on
 * "underline" text.
 */
__attribute__((warn_unused_result)) extern buffer_t*
    term_underline(buffer_t* buffer) {
  return buffer_printf(buffer, TERM_ESCAPE_STRING("4m"));
}

/**
 * @function term_reset_formatting
 *
 * Append a terminal escape sequence to a buffer that resets the
 * formatting (and appears to cancel the foreground and background
 * color as well).
 */
__attribute__((warn_unused_result)) extern buffer_t*
    term_reset_formatting(buffer_t* buffer) {
  return buffer_printf(buffer, TERM_ESCAPE_STRING("0m"));
}

/**
 * @function term_clear_screen
 *
 * Append a terminal escape sequence to a buffer that clears the
 * entire terminal.
 */
__attribute__((warn_unused_result)) extern buffer_t*
    term_clear_screen(buffer_t* buffer) {
  return buffer_printf(buffer, TERM_ESCAPE_STRING("2J"));
}

/**
 * @function term_draw_box
 *
 * Append the terminal escape sequences to a buffer that draws a box.
 */
__attribute__((warn_unused_result)) extern buffer_t*
    term_draw_box(buffer_t* buffer, uint16_t x0, uint16_t y0, uint16_t x1,
                  uint16_t y1, box_drawing_t* box) {
  // top
  buffer = term_move_cursor_absolute(buffer, x0, y0);
  buffer = buffer_append_code_point(buffer, box->upper_left_corner);
  for (uint64_t x = x0 + 1; x < x1; x++) {
    buffer = buffer_append_code_point(buffer, box->top_edge);
  }
  buffer = buffer_append_code_point(buffer, box->upper_right_corner);

  // bottom
  buffer = term_move_cursor_absolute(buffer, x0, y1);
  buffer = buffer_append_code_point(buffer, box->lower_left_corner);
  for (uint64_t x = x0 + 1; x < x1; x++) {
    buffer = buffer_append_code_point(buffer, box->bottom_edge);
  }
  buffer = buffer_append_code_point(buffer, box->lower_right_corner);

  // the sides (and the middle)
  for (int y = y0 + 1; y < y1; y++) {
    buffer = term_move_cursor_absolute(buffer, x0, y);
    buffer = buffer_append_code_point(buffer, box->left_edge);

    for (int x = x0 + 1; x < x1; x++) {
      buffer = buffer_append_code_point(buffer, ' ');
    }
    // buffer = term_move_cursor_absolute(buffer, x1, y);

    buffer = buffer_append_code_point(buffer, box->right_edge);
  }

  return buffer;
}

/**
 * @function term_echo_off
 *
 * Turn off canonical input mode and echo and return the original
 * terminal settings so they can be restored.
 */
extern struct termios term_echo_off() {
  struct termios oldt;
  struct termios newt;
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;

  // Get the original terminal settings
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;

  // Disable canonical input mode and echo
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);

  return oldt;
}

/**
 * @function term_echo_off
 *
 * Append a terminal escape sequence to turn on hardware echoing.
 */
extern void term_echo_restore(struct termios oldt) {
  // Restore the original terminal settings
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

// Set window title.
// ESC ]0;this is the window title BEL

// ESC ]8;;link ST (hyperlink)
/**
 * @file tokenizer.c
 *
 */

#ifndef _TOKENIZER_H_
#define _TOKENIZER_H_

extern value_array_t* string_tokenize(const char* str, const char* delimiters);
extern value_array_t* buffer_tokenize(buffer_t* buffer, const char* delimiters);
extern value_array_t* tokenize_memory_range(uint8_t* start, uint64_t length,
                                            const char* delimiters);

// TODO(jawilson):

#endif /* _TOKENIZER_H_ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void add_duplicate(value_array_t* token_array, const char* data);

/**
 * @function string_tokenize
 *
 * Tokenize a string into a a value_array_t of (non-empty) strings.
 *
 * Delimiters terminate the current token and are thrown away. The
 * delimiters string is treated as a sequence of delimiter characters,
 * it does not mean a delimiter can be multiple characters.
 */
value_array_t* string_tokenize(const char* str, const char* delimiters) {
  return tokenize_memory_range(cast(uint8_t*, str), strlen(str), delimiters);
}

/**
 * @function buffer_tokenize
 *
 * Tokenize the current contents of a buffer into a value_array_t of
 * (non-empty) strings. The input buffer should contain a valid UTF-8
 * encoded string.
 *
 * NUL bytes inside the buffer are automatically treated as an
 * additional delimiter.
 */
value_array_t* buffer_tokenize(buffer_t* buffer, const char* delimiters) {
  return tokenize_memory_range(&(buffer->elements[0]), buffer->length,
                               delimiters);
}

/**
 * @function tokenize_memory_range
 *
 * Tokenize a memory range into a value_array_t of (non-empty) strings. The
 * range should contain a valid UTF-8 encoded string.
 *
 * NUL bytes are automatically treated as an additional delimiter.
 */
value_array_t* tokenize_memory_range(uint8_t* str, uint64_t length,
                                     const char* delimiters) {
  value_array_t* result = make_value_array(1);
  char token_data[1024];
  int cpos = 0;
  for (int i = 0; (i < length); i++) {
    uint8_t ch = str[i];
    if (ch == 0 || string_contains_char(delimiters, ch)) {
      token_data[cpos++] = '\0';
      if (strlen(token_data) > 0) {
        add_duplicate(result, token_data);
      }
      cpos = 0;
    } else {
      token_data[cpos++] = ch;
    }
  }
  token_data[cpos++] = '\0';
  if (strlen(token_data) > 0) {
    add_duplicate(result, token_data);
  }

  return result;
}

// Add a *copy* of the string named data to the token list.
void add_duplicate(value_array_t* token_array, const char* data) {
  value_array_add(token_array, str_to_value(string_duplicate(data)));
}

/**
 * @file random.c
 *
 * An implementation of "xorshiro128**", a pseudo-random number
 * generator.
 *
 * This is not a high quality source of entropy and is intended for
 * use in tests or other places where determinism is important
 * (including across platforms and C library implementations).
 *
 * See: https://prng.di.unimi.it/xoroshiro128starstar.c
 */

#ifndef _RANDOM_H_
#define _RANDOM_H_

struct random_state_S {
  uint64_t a;
  uint64_t b;
};

typedef struct random_state_S random_state_t;

extern random_state_t random_state_for_test(void);
extern uint64_t random_next_uint64(random_state_t* state);
extern uint64_t random_next_uint64_below(random_state_t* state,
                                         uint64_t maximum);

#endif /* _RANDOM_H_ */

#include <time.h>

/**
 * @function random_state_for_test
 *
 * Return a consistent initial random state for tests.
 */
random_state_t random_state_for_test(void) {
  return compound_literal(random_state_t,
                          {.a = 0x1E1D43C2CA44B1F5, .b = 0x4FDD267452CEDBAC});
}

static random_state_t shared_random_state = {0};

/**
 * @function random_state
 *
 * Return a shared random state. If the random state has not been
 * initialized yet, it is initialized based off the timestamp.
 */
random_state_t* random_state(void) {
  if (shared_random_state.a == 0) {
    shared_random_state.a = 0x1E1D43C2CA44B1F5 ^ cast(uint64_t, time(NULL));
    shared_random_state.b = 0x4FDD267452CEDBAC ^ cast(uint64_t, time(NULL));
  }

  return &shared_random_state;
}


static inline uint64_t rotl(uint64_t x, int k) {
  return (x << k) | (x >> (64 - k));
}

/**
 * @function random_next
 *
 * Return a random uint64_t from the current state (and update the
 * state).
 */
uint64_t random_next(random_state_t* state) {
  uint64_t s0 = state->a;
  uint64_t s1 = state->b;
  uint64_t result = rotl(s0 * 5, 7) * 9;
  s1 ^= s0;
  state->a = rotl(s0, 24) ^ s1 ^ (s1 << 16); // a, b
  state->b = rotl(s1, 37);                   // c

  return result;
}

/**
 * @function random_next_uint64_below
 *
 * Return a random `uint64_t` that is below some maximum. As much as
 * the underlying random number generartor allows, this should be
 * uniform.
 */
uint64_t random_next_uint64_below(random_state_t* state, uint64_t maximum) {
  if (maximum == 0) {
    fatal_error(ERROR_ILLEGAL_ARGUMENT);
  }
#if 1
  // This is simpler and works well in practice (it seems).
  return random_next(state) % maximum;
#else
  // This version in theory should be a bit more fair than modulous
  // but I can't really detect a difference.
  int mask = (1ULL << (uint64_highest_bit_set(maximum) + 1)) - 1;
  while (1) {
    uint64_t n = random_next(state);
    n &= mask;
    if (n < maximum) {
      return n;
    }
  }
#endif /* 0 */
}

/**
 * @file cdl-printer.c
 *
 * There isn't a formal specification for CDL however it's very easy
 * to *pretty print* and definitely on the simpler side for
 * parsing. Most importantly, it tends to look "clean" or "open"
 * versus other formats.
 *
 * At first glance, CDL looks just like JSON if you imagine removing
 * all commas and replacing ":" with the more readable " = " sequence.
 *
 * CDL only really has tables (aka maps), arrays, and strings. Parsers
 * should utilize a schema to determine when to treat a string as
 * something else like a boolean (the standard is true/false),
 * integer, float, etc.
 *
 * The key to making CDL look sane is the string quoting rule. As long
 * as all of the characters (aka unicode code-points) in string are
 * "safe", there is no need to put double quotes around the
 * string. The "unsafe" characters are of a very limited set: code
 * points less than 32, code-point 44, and these code-points: " ",
 * "\\"", "'", "{", "}", "[", "]", "(", ")", "`", '#', ":", ",", "=".
 *
 * This library only provides a printer. For reading, we'll delay that
 * until Omni C can use it's reflection API to automatically write
 * readers for us.
 *
 * CDL stands for "clear data language", "C data language", or
 * "comma-less data language", take your pick!
 */

#ifndef _CDL_PRINTER_H_
#define _CDL_PRINTER_H_

typedef struct {
  buffer_t* buffer;
  char* key_token;
  int indention_level;
} cdl_printer_t;

cdl_printer_t* make_cdl_printer(buffer_t* buffer);

void cdl_boolean(cdl_printer_t* printer, boolean_t bolean);
void cdl_string(cdl_printer_t* printer, char* string);
void cdl_int64(cdl_printer_t* printer, int64_t number);
void cdl_uint64(cdl_printer_t* printer, uint64_t number);
void cdl_double(cdl_printer_t* printer, double number);

void cdl_start_array(cdl_printer_t* printer);
void cdl_end_array(cdl_printer_t* printer);

void cdl_start_table(cdl_printer_t* printer);
void cdl_key(cdl_printer_t* printer, char* key);
void cdl_end_table(cdl_printer_t* printer);

#endif /* _CDL_PRINTER_H_ */

cdl_printer_t* make_cdl_printer(buffer_t* buffer) {
  cdl_printer_t* result = malloc_struct(cdl_printer_t);
  result->buffer = buffer;
  return result;
}

void cdl_indent(cdl_printer_t* printer) {
  buffer_append_repeated_byte(printer->buffer, ' ',
                              4 * printer->indention_level);
}

boolean_t is_safe_string(char* string) {
  buffer_t* buffer = buffer_from_string(string);
  for (int pos = 0; pos < buffer_length(buffer);) {
    utf8_decode_result_t decode_result = buffer_utf8_decode(buffer, pos);
    if (decode_result.error) {
      fatal_error(ERROR_ILLEGAL_UTF_8_CODE_POINT);
    }
    uint32_t code_point = decode_result.code_point;
    if (code_point <= 32) {
      return false;
    }
    switch (code_point) {
    case '"':
    case '#':
    case '(':
    case ')':
    case ',':
    case ':':
    case '=':
    case '[':
    case '\'':
    case ']':
    case '`':
    case '{':
    case '}':
      return false;
    }
    pos += decode_result.num_bytes;
  }
  return true;
}

void cdl_output_token(cdl_printer_t* printer, char* string) {
  cdl_indent(printer);
  if (printer->key_token != NULL) {
    buffer_printf(printer->buffer, "%s = %s\n", printer->key_token, string);
    printer->key_token = NULL;
  } else {
    buffer_printf(printer->buffer, "%s\n", string);
  }
}

void cdl_boolean(cdl_printer_t* printer, boolean_t boolean) {
  cdl_output_token(printer, boolean ? "true" : "false");
}

void cdl_string(cdl_printer_t* printer, char* string) {
  if (!is_safe_string(string)) {
    cdl_output_token(printer, string_printf("\"%s\"", string));
  } else {
    cdl_output_token(printer, string);
  }
}

void cdl_int64(cdl_printer_t* printer, int64_t number) {
  cdl_output_token(printer, string_printf("%ld", number));
}

void cdl_uint64(cdl_printer_t* printer, uint64_t number) {
  cdl_output_token(printer, uint64_to_string(number));
}

void cdl_double(cdl_printer_t* printer, double number) {
  cdl_output_token(printer, string_printf("%lf", number));
}

void cdl_start_array(cdl_printer_t* printer) {
  cdl_output_token(printer, "[");
  printer->indention_level += 1;
}

void cdl_end_array(cdl_printer_t* printer) {
  printer->indention_level -= 1;
  cdl_output_token(printer, "]");
}

void cdl_start_table(cdl_printer_t* printer) {
  cdl_output_token(printer, "{");
  printer->indention_level += 1;
}

void cdl_key(cdl_printer_t* printer, char* key) { printer->key_token = key; }

void cdl_end_table(cdl_printer_t* printer) {
  printer->indention_level -= 1;
  cdl_output_token(printer, "}");
}
#ifndef _SUB_PROCESS_H_
#define _SUB_PROCESS_H_

/**
 * @file sub-process.c
 *
 * Utility to launch a sub-processes and interfact with them.
 *
 * (This version will probably only work on Unix like operating
 * systems but we should be able to make an MS-Windows version at some
 * point.)
 */

/**
 * @enum sub_process_exit_status_t
 */
typedef enum {
  EXIT_STATUS_UNKNOWN,
  EXIT_STATUS_NORMAL_EXIT,
  EXIT_STATUS_SIGNAL,
  EXIT_STATUS_ABNORMAL
} sub_process_exit_status_t;

typedef struct {
  // launch parameters
  value_array_t* argv; // argv[0] is the program executable path

  // child process information
  pid_t pid;
  int stdin;
  int stdout;
  int stderr;

  // exit info
  sub_process_exit_status_t exit_status;
  int exit_code;
  int exit_signal;
} sub_process_t;

sub_process_t* make_sub_process(value_array_t* argv);

boolean_t sub_process_launch(sub_process_t* sub_process);

uint64_t sub_process_write(sub_process_t* sub_process, buffer_t* data,
                           uint64_t start_position);

void sub_process_close_stdin(sub_process_t* sub_process);

void sub_process_read(sub_process_t* sub_process, buffer_t* stdout,
                      buffer_t* stderr);

void sub_process_wait(sub_process_t* sub_process);

void sub_process_launch_and_wait(sub_process_t* sub_process,
                                 buffer_t* child_stdin, buffer_t* child_stdout,
                                 buffer_t* child_stderr);

#endif /* _SUB_PROCESS_H_ */

#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/**
 * @function make_sub_process
 *
 * Make and return an *unlaunched* sub-process.
 */
sub_process_t* make_sub_process(value_array_t* argv) {
  sub_process_t* result = malloc_struct(sub_process_t);
  result->argv = argv;
  result->exit_code = -1;
  return result;
}

/**
 * @function sub_process_launch
 *
 * Do the actual sub process launch using the "arguments" in
 * sub_process_t.
 */
boolean_t sub_process_launch(sub_process_t* sub_process) {
  uint64_t length = sub_process->argv->length;
  if (length < 1) {
    log_fatal("Expected at least the program path in argv");
    fatal_error(ERROR_ILLEGAL_STATE);
  }

  // 1. Convert value_array_t to char** for execvp
  char** argv = cast(typeof(char**),
                     malloc_bytes((length + 1) * sizeof(typeof(char*))));
  for (int i = 0; i < length; i++) {
    argv[i] = value_array_get_ptr(sub_process->argv, i, typeof(char*));
  }
  // This shouldn't be necessary because we used malloc_bytes which
  // zeros allocations but it might look like an error.
  argv[length] = NULL;
  // char** envp = NULL;

  // 2. Create pipes for stdin of the sub process as well as to
  // capture stdout and stderr.
  int stdin_pipe[2] = {0};
  int stdout_pipe[2] = {0};
  int stderr_pipe[2] = {0};
  if (pipe(stdin_pipe) == -1 || pipe(stdout_pipe) == -1
      || pipe(stderr_pipe) == -1) {
    log_fatal("Failed to create pipe for stdin, stdout or stderr");
    fatal_error(ERROR_ILLEGAL_STATE);
    return false;
  }

  // 3. Fork the process
  pid_t pid = fork();
  if (pid == -1) {
    log_fatal("fork() failed.");
    fatal_error(ERROR_ILLEGAL_STATE);
    return false;
  }

  if (pid == 0) {
    /* ====================================================================== */
    /* Child Process */
    /* ====================================================================== */

    // 4. Redirect stdin, stdout and stderr to pipes
    dup2(stdin_pipe[0], STDIN_FILENO);
    dup2(stdout_pipe[1], STDOUT_FILENO);
    dup2(stderr_pipe[1], STDERR_FILENO);
    close(stdin_pipe[0]);
    close(stdin_pipe[1]);
    close(stdout_pipe[0]);
    close(stdout_pipe[1]);
    close(stderr_pipe[0]);
    close(stderr_pipe[1]);

    // 5. Final "exec" to the command
    int exec_exit_status = execvp(argv[0], argv);
    // execvp should not return!
    log_fatal("execvp returned non zero exit status %d", exec_exit_status);
    fatal_error(ERROR_ILLEGAL_STATE);
    return false;
  } else {
    /* ====================================================================== */
    /* Parent Process */
    /* ====================================================================== */

    // 6. Close write ends of the pipes in the parent since we will
    // only be reading from the pipe.
    close(stdin_pipe[0]);  // Close read end of stdin pipe
    close(stdout_pipe[1]); // Close write end of stdout pipe
    close(stderr_pipe[1]); // Close write end of stderr pipe

    // 7. Record the pid, stdout, and stderr.
    sub_process->pid = pid;
    sub_process->stdin = stdin_pipe[1];
    sub_process->stdout = stdout_pipe[0];
    sub_process->stderr = stderr_pipe[0];

    free_bytes(argv);

    return true;
  }
}

uint64_t sub_process_write(sub_process_t* sub_process, buffer_t* data,
                           uint64_t start_position) {
  int stdin_fd = sub_process->stdin;

  // Set the file descriptor to non-blocking mode
  int flags = fcntl(stdin_fd, F_GETFL, 0);
  fcntl(stdin_fd, F_SETFL, flags | O_NONBLOCK);

  ssize_t bytes_written = write(stdin_fd, &data->elements[start_position],
                                (data->length - start_position));

  if (bytes_written == -1) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      return 0;
    } else {
      // An actual error occurred
      log_fatal("Error writing to subprocess stdin: %s", strerror(errno));
      fatal_error(ERROR_ILLEGAL_STATE);
    }
  }

  return bytes_written;
}

void sub_process_close_stdin(sub_process_t* sub_process) {
  if (sub_process->stdin != -1) { // Check if stdin is still open
    if (close(sub_process->stdin) == -1) {
      log_fatal("Error closing subprocess stdin: %s", strerror(errno));
      fatal_error(ERROR_ILLEGAL_STATE);
    }
    sub_process->stdin = -1; // Mark stdin as closed
  }
}

void sub_process_read(sub_process_t* sub_process, buffer_t* stdout,
                      buffer_t* stderr) {
  if (stdout != NULL) {
    buffer_read_ready_bytes_file_number(stdout, sub_process->stdout,
                                        0xffffffff);
  }
  if (stderr != NULL) {
    buffer_read_ready_bytes_file_number(stderr, sub_process->stderr,
                                        0xffffffff);
  }
}

// (Private) common tail to is_sub_process_running and
// sub_process_wait. This should only be called once per sub-process.
void sub_process_record_exit_status(sub_process_t* sub_process, pid_t pid,
                                    int status) {
  if (pid == -1) {
    sub_process->exit_status = EXIT_STATUS_ABNORMAL;
    return;
  }

  // Check the exit status and return the exit code
  if (WIFEXITED(status)) {
    sub_process->exit_status = EXIT_STATUS_NORMAL_EXIT;
    sub_process->exit_code = WEXITSTATUS(status);
  } else if (WIFSIGNALED(status)) {
    sub_process->exit_status = EXIT_STATUS_SIGNAL;
    sub_process->exit_signal = WTERMSIG(status);
  } else {
    sub_process->exit_status = EXIT_STATUS_ABNORMAL;
  }
}

/**
 * @function is_sub_process_running
 *
 * Return true if we recorded that the process already exited or else
 * do a non-blocking call to waitpid to see if it has already
 * exited. This function is idempotent once we reach the exit state.
 */
boolean_t is_sub_process_running(sub_process_t* sub_process) {
  if (sub_process->exit_status != EXIT_STATUS_UNKNOWN) {
    return false;
  }

  int status = 0;
  pid_t result = waitpid(sub_process->pid, &status, WNOHANG);
  if (result == 0) {
    return true;
  }
  sub_process_record_exit_status(sub_process, result, status);
  return false;
}

/**
 * @function sub_process_wait
 *
 * Do a blocking call to waitpid unless we've already recorded that
 * this sub-process has exited in which case it is a NOP. This
 * function is idempotent once we reach the exit state.
 */
void sub_process_wait(sub_process_t* sub_process) {
  if (sub_process->exit_status != EXIT_STATUS_UNKNOWN) {
    int status = 0;
    pid_t result = waitpid(sub_process->pid, &status, 0);
    sub_process_record_exit_status(sub_process, result, status);
  }
}

/**
 * @function sub_process_launch_and_wait
 *
 * Make it much more convenient to use sub-processes in the common
 * case where all you want to do is (possibly) send the contents of a
 * buffer to the sub-processes stdin, collect the sub-processes stdout
 * and stderr until all is collected and the sub-processes has exited.
 *
 * Note that buffers may be NULL in which case that data is ignored.
 */
void sub_process_launch_and_wait(sub_process_t* sub_process,
                                 buffer_t* child_stdin, buffer_t* child_stdout,
                                 buffer_t* child_stderr) {
  sub_process_launch(sub_process);
  uint64_t written = 0;
  do {
    if (child_stdin != NULL && written < child_stdin->length) {
      written += sub_process_write(sub_process, child_stdin, written);
      if (written >= child_stdin->length) {
        sub_process_close_stdin(sub_process);
      }
    }
    sub_process_read(sub_process, child_stdout, child_stderr);
    usleep(5);
  } while (is_sub_process_running(sub_process));
  sub_process_read(sub_process, child_stdout, child_stderr);
  sub_process_wait(sub_process);
}

#ifndef _SPLITJOIN_H_
#define _SPLITJOIN_H_

buffer_t* join_array_of_strings(value_array_t* array_of_strings,
                                char* separator);

#endif /* _SPLITJOIN_H_ */

/**
 * @file splitjoin.c
 *
 * This badly named file handles the very common operations of
 * splitting a string into an array of sub-strings or joining an array
 * of strings into one large string. The Guava Java library has some
 * very configurable routines for doing this though we are just
 * implementing some quick and dirty routines for use in the omni-c
 * compiler.
 */

/**
 * @function join_array_of_strings
 *
 * Join an array of strings placing separator (which may be an empty
 * string but not NULL) between strings.
 */
buffer_t* join_array_of_strings(value_array_t* array_of_strings,
                                char* separator) {
  buffer_t* result = make_buffer(1);
  for (int i = 0; i < array_of_strings->length; i++) {
    if (i > 0) {
      buffer_append_string(result, separator);
    }
    buffer_append_string(result, value_array_get(array_of_strings, i).str);
  }
  return result;
}

// TODO(jawilson): add split_buffer_to_array_of_strings - this is
// similar to string_tokenize except instead of splitting on a
// character set it should split on a string.
/**
 * @file test.c
 *
 * A set of macros and inline functions to help write tests for the
 * armyknife library (or your own tests).
 */

// ======================================================================
// This section is extraced to test.h
// ======================================================================

#ifndef _TEST_H_
#define _TEST_H_

/**
 * @macro test_fail
 *
 * Immediately fail a test after printing a message (supplied the same
 * way as printf, log_fatal, etc.)
 *
 * This is a macro instead of a function because it automatically
 * passes the current file and line number to test_fail_and_exit which
 * enables uniform and convenient error messagages according to GNU
 * guidelines (so they will look like gcc compiler errors if your IDE
 * understands those).
 *
 * It is *advised* (at least for now) to not pass complicated
 * expressions to `test_fail` if those are likely to fail. Instead run
 * the test in a debugger and set a break-point on
 * `test_fail_and_exit`.
 */
#define test_fail(format, ...)                                                 \
  do {                                                                         \
    test_fail_and_exit(__FILE__, __LINE__, format, ##__VA_ARGS__);             \
  } while (0)


/**
 * @macro test_assert
 *
 * Evaluate the condition and fail the test if the result is false.
 */
#define test_assert(condition)                                                 \
  do {                                                                         \
    if (!(condition))                                                          \
      test_fail("A test assertion failed. Condition expression was: %s",       \
                #condition);                                                   \
  } while (0)

/**
 * @macro test_assert_uint64_equal
 *
 * Assert that two values are the same by casting both of them to
 * uint64_t and seeing if they are "==".
 *
 * Except that we lose the "location" information, this would be
 * better as an inline function rather than a macro.
 */
#define test_assert_integer_equal(a, b)                                        \
  do {                                                                         \
    unsigned long long casted_a = (unsigned long long) (a);                    \
    unsigned long long casted_b = (unsigned long long) (b);                    \
    if (a != b) {                                                              \
      test_fail(                                                               \
          "An integer comparision failed\n  Expected:\n    ⟦%llu⟧\n  "     \
          "But was:\n    ⟦%llu⟧\n",                                            \
          casted_a, casted_b);                                                 \
    }                                                                          \
  } while (0)

/**
 * @macro test_assert_string_equal
 *
 * Assert that two c strings are the same.
 */
#define test_assert_string_equal(a, b)                                          \
  do {                                                                          \
    if (!b) {                                                                   \
      test_fail(                                                                \
          "A test string equal assertion failed\n  Expected:\n    ⟦%s⟧\n  " \
          "But was:\n    nullptr\n",                                            \
          a);                                                                   \
    }                                                                           \
    if (!string_equal(a, b)) {                                                  \
      test_fail(                                                                \
          "A test string equal assertion failed\n  Expected:\n    ⟦%s⟧\n  " \
          "But was:\n    ⟦%s⟧\n",                                               \
          a, b);                                                                \
    }                                                                           \
  } while (0)

static inline void open_arena_for_test(void) {
  /*
#ifdef C_ARMYKNIFE_LIB_USE_ARENAS
  arena_open(4096 * 256);
#endif
  */
}

static inline void close_arena_for_test(void) {
  /*
#ifdef C_ARMYKNIFE_LIB_USE_ARENAS
  arena_close();
#endif
  */
}

#endif /* _TEST_H_ */

/**
 * @function test_fail_and_exit
 *
 * Set a break-point here to debug a test but normally you will use
 * the macro `test_fail` since it is much more convenient.
 */
__attribute__((format(printf, 3, 4))) void
    test_fail_and_exit(char* file_name, int line_number, char* format, ...) {
  va_list args;
  fprintf(stdout, "%s:%d: ", file_name, line_number);
  va_start(args, format);
  vfprintf(stdout, format, args);
  fprintf(stdout, "\n");
  va_end(args);
  exit(1);
}
#endif /* C_ARMYKNIFE_LIB_IMPL */
