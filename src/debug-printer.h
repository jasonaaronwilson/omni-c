// SSCF generated file from: debug-printer.c

#line 2 "debug-printer.c"
#ifndef _DEBUG_PRINTER_H_
#define _DEBUG_PRINTER_H_

#include "lexer.h"
#include "parser.h"
#include <c-armyknife-lib.h>
#include <ctype.h>

/**
 * @file debug-printer.c
 *
 * A node debug "printer" (appending to a buffer).
 *
 * Normally I would use "s-expressions" but since I don't have an
 * s-expression pretty-printer, I thought trying something else might
 * be interesting. (We did something a bit similar with TSNode's when
 * we were using tree-sitter).
 */

#include "debug-printer.c.generated.h"

__attribute__((warn_unused_result)) static inline buffer_t*
    buffer_indent(buffer_t* buffer, int indention_level) {
  return buffer_append_repeated_byte(buffer, ' ', indention_level * 4);
}

#endif /* _DEBUG_PRINTER_H_ */
