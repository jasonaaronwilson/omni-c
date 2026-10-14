/**
 * @file fatal-error.c
 *
 * This is for compatibility with older code. fatal_error's aren't
 * necessarily fatal anymore though care must still be taken if error
 * are caught and execution attempts to continue.
 */

/**
 * @macro fatal_error
 *
 * Terminates the program with a fatal error.
 */
#define fatal_error(code) (throw_error(make_error(code)))
