// SSCF generated file from: header-file-printer.c

#line 2 "header-file-printer.c"
#ifndef _HEADER_FILE_PRINTER_H_
#define _HEADER_FILE_PRINTER_H_

#include "lexer.h"
#include "parser.h"
#include <c-armyknife-lib.h>
#include <ctype.h>

/**
 * @file header-file-printer.c
 *
 * This file provides primitives to "print" out certain kinds of parse
 * nodes in a suitable format for a C "header" file (which means they
 * will be potentially be the same as for a normal non-header file
 * which we will sort out later).
 *
 * This will eventually support one of our immediate goals which is
 * that no one should ever have to write a header file themselves even
 * if they could care less about any advanced feature of omni-c
 * (namely, overloaded functions, C++ style templates, automatic code
 * generation for things like enum->string and string->enum), let
 * alone the reasons I started this project.
 *
 * The primitives are not smart enough to make decisions about the
 * order things should appear in the header file, etc.
 */

#include "header-file-printer.c.generated.h"

#endif /* _HEADER_FILE_PRINTER_H_ */
