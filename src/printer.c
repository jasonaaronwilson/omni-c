#line 2 "printer.c"
#ifndef _PRINTER_H_
#define _PRINTER_H_

#include <c-armyknife-lib.h>
#include <ctype.h>

#include "lexer.h"

/**
 * @file printer.c
 *
 * Provides a wrapper around a buffer_t that allows us to add
 * additional fields like the current indention level (and eventually
 * a mechanism to automatically add whitespace when that is
 * necessary).
 */
typedef struct printer_S {
  buffer_t* buffer;
  uint32_t indent_width;
  uint32_t indent_level;
  // TODO(jawilson): add is_header_file
} printer_t;

#include "printer.c.generated.h"

#endif /* _PRINTER_H_ */

printer_t* make_printer(buffer_t* buffer, int indent_width) {
  printer_t* result = malloc_struct(printer_t);
  result->buffer = buffer;
  result->indent_width = indent_width;
  return result;
}

printer_t* append_string(printer_t* printer, char* string) {
  buffer_append_string(printer->buffer, string);
  return printer;
}

printer_t* append_token(printer_t* printer, token_t* token) {
  buffer_append_sub_buffer(printer->buffer, token->start, token->end,
                           token->buffer);
  return printer;
}

printer_t* printer_newline(printer_t* printer) {
  buffer_append_byte(printer->buffer, '\n');
  return printer;
}

printer_t* printer_space(printer_t* printer) {
  buffer_append_byte(printer->buffer, ' ');
  return printer;
}

printer_t* printer_indent(printer_t* printer) {
  buffer_append_repeated_byte(printer->buffer, ' ',
                              printer->indent_width * printer->indent_level);
  return printer;
}

printer_t* printer_increase_indent(printer_t* printer) {
  printer->indent_level++;
  return printer;
}

printer_t* printer_decrease_indent(printer_t* printer) {
  printer->indent_level--;
  return printer;
}
