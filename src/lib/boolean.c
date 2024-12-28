#line 2 "boolean.c"
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
