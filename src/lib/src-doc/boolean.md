# @file boolean.c

Make sure that at least true/false from <stdbool.h> are available
and and a new typedef named boolean_t because bool seems ugly. (Use
what you prefer!)
 
## @type boolean_t

This is a simple typedef for "bool" (or _Bool from C99) which is
available from "stdbool.h" as bool and true and false constants are
also defined. We use it for more consistency in primitive types
(where only char* is commonly used in this library despite not
following the typically naming convention).
 
