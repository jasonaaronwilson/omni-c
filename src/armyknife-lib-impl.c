#line 2 "armyknife-lib-impl.c"

/**
 * C armyknife lib is currently only available as a single source
 * header file. This just makes sure the parts of the header file that
 * aren't "inlined" get compiled into our binary. Eventually
 * c-armyknife-lib will be available as a header plus either ".a" or
 * ".so" library making this file unnecessary though this isn't really
 * a pain point by any means or I would have done that already.
 */

#define C_ARMYKNIFE_LIB_IMPL
// #define ARMYKNIFE_LIB_DEFAULT_LOG_LEVEL 4
#include <c-armyknife-lib.h>
