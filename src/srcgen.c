/**
 * @file srcgen.c
 *
 * Given a symbol table, these routines will automatically generate
 * additional source code such as enum->string and string->enum,
 * various kinds of metadata, etc.
 */

#ifndef _SRCGEN_H_
#define _SRCGEN_H_

#include "c-file-printer.h"
#include "parser.h"
#include "symbol-table-builder.h"
#include <c-armyknife-lib.h>

#include "srcgen.c.generated.h"

#endif /* _SRCGEN_H_ */

static inline char* remove_type_suffix_1(char* typename) {
  if (string_ends_with(typename, "_t")) {
    return string_substring(typename, 0, strlen(typename) - 2);
  } else {
    return typename;
  }
}

/**
 * @function srcgen_enum_to_string_converters
 *
 * For each enum or typedef'd enum, create two additional routines to
 * provice conversion between char* and an enum value.
 *
 * This works by first generate the routines in textual format and
 * then using the parser to parse it since it is quite a bit easier to
 * understand and probably easier to debug.
 */
void srcgen_enum_to_string_converters(symbol_table_t* symbol_table) {
  buffer_t* buffer = make_buffer(1);

  for (int i = 0; i < symbol_table->typedefs->ordered_bindings->length; i++) {
    symbol_table_binding_t* binding = value_array_get_ptr(
        symbol_table->typedefs->ordered_bindings, i, symbol_table_binding_t*);
    typedef_node_t* typedef_node = to_typedef_node(
        value_array_get_ptr(binding->definition_nodes, 0, parse_node_t*));
    if (is_enum_node(typedef_node->type_node->user_type)) {
      enum_node_t* enum_node = to_enum_node(typedef_node->type_node->user_type);
      char* enum_node_name = token_to_string(typedef_node->name);

      log_warn("Generating converters for %s", enum_node_name);

      char* to_string_prefix = remove_type_suffix_1(enum_node_name);
      char* enum_node_type_string = enum_node_name;

      buffer = buffer_append_enum_to_string(buffer, enum_node, to_string_prefix,
                                            enum_node_type_string);
      buffer = buffer_append_string_to_enum(buffer, enum_node, to_string_prefix,
                                            enum_node_type_string);
      buffer = buffer_append_enum_metadata(buffer, enum_node, to_string_prefix,
                                           enum_node_type_string);
    }
  }

  for (int i = 0; i < symbol_table->enums->ordered_bindings->length; i++) {
    symbol_table_binding_t* binding = value_array_get_ptr(
        symbol_table->enums->ordered_bindings, i, symbol_table_binding_t*);
    enum_node_t* enum_node = to_enum_node(
        value_array_get_ptr(binding->definition_nodes, 0, parse_node_t*));
    char* enum_node_name = token_to_string(enum_node->name);
    char* to_string_prefix = remove_type_suffix_1(enum_node_name);
    char* enum_node_type_string = string_printf("enum %s", enum_node_name);
    buffer = buffer_append_enum_to_string(buffer, enum_node, to_string_prefix,
                                          enum_node_type_string);
    buffer = buffer_append_string_to_enum(buffer, enum_node, to_string_prefix,
                                          enum_node_type_string);
    buffer = buffer_append_enum_metadata(buffer, enum_node, to_string_prefix,
                                         enum_node_type_string);
  }

  if (buffer->length > 0) {
    symbol_table_parse_buffer(symbol_table, buffer,
                              "#<internal-code-generator>", false);
  }
}
