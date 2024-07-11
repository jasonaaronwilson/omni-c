#line 2 "sourcer-to-source.c"
#ifndef _SOURCE_TO_SOURCE_H_
#define _SOURCE_TO_SOURCE_H_

#include "symbol-table.h"
#include <c-armyknife-lib.h>
#include "source-to-source.c.generated.h"

#endif /* _SOURCE_TO_SOURCE_H_ */

/**
 * @file source-to-source.c
 *
 * These are a collection of small source to source transformations
 * that make other anaylsis or other transformation a little
 * simpler. While the order of transformation are not always
 * important, the order they appear in this file should be
 * appropriate.
 */

/**
 * @function split_structure_typedefs
 *
 * Given a fairly typical (at least in this code base) construct like:
 *
 * ```
 *   typedef struct foo_S {} foo_t;
 * ```
 *
 * Convert this into two independent pieces:
 *
 * ```
 *  typedef struct foo_S foo_t;
 *  struct foo_S {};
 * ```
 */

void split_structure_typedefs(symbol_table_t* symbol_table) {
}
   




