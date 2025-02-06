/**
 * @type boolean_t
 *
 * This is a simple typedef for "bool" (or _Bool from C99) which is
 * available from "stdbool.h" as bool and true and false constants are
 * also defined. We use it for more consistency in primitive types
 * (where only char* is commonly used in this library despite not
 * following the typically naming convention).
 */

#include <stdbool.h>

typedef bool boolean_t;
