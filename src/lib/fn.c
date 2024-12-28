#line 2 "fn.c"

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
