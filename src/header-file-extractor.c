/**
 * @file header-file-extractor.c
 *
 * DEPRECATED: This is the original tool to put function prototypes
 * into header files though we played around with extracting enums and
 * doing code generation for those at the same time however the new
 * code based on the system_table_t is much simpler.
 *
 * These routines consume parsed top-level C (and eventually omni-c)
 * declarations to produce C (or maybe C++) header files or C
 * fragments.
 *
 * Some routines may also eventually do initial high-level
 * code-generation which isn't that limiting because we can parse the
 * generated code quite easily by lexing and parsing and the output is
 * conveniently placed into buffers instead of files making them more
 * flexible.
 */

#include <c-armyknife-lib.h>

#include "c-file-printer.h"
#include "parser.h"

static inline char* remove_type_suffix(char* typename) {
  if (string_ends_with(typename, "_t")) {
    return string_substring(typename, 0, strlen(typename) - 2);
  } else {
    return typename;
  }
}

__attribute__((warn_unused_result)) buffer_t*
    extract_enums_process_declarations(buffer_t* output,
                                       declarations_node_t* root) {
  uint64_t length = node_list_length(root->declarations);

  for (uint64_t i = 0; i < length; i++) {
    parse_node_t* node = node_list_get(root->declarations, i);
    if (node->tag == PARSE_NODE_TYPEDEF) {
      typedef_node_t* typedef_node = to_typedef_node(node);
      if (is_enum_node(typedef_node->type_node->user_type)) {
        enum_node_t* enum_node
            = to_enum_node(typedef_node->type_node->user_type);
        output = buffer_printf(output, "typedef ");
        output
            = buffer_printf(output, "%s ", token_to_string(typedef_node->name));
        output = buffer_append_enum_node(output, enum_node);
        output = buffer_printf(output, ";\n\n");

        char* enum_node_name = token_to_string(typedef_node->name);
        char* to_string_prefix = remove_type_suffix(enum_node_name);
        char* enum_node_type_string = enum_node_name;

        output = buffer_append_enum_to_string(
            output, enum_node, to_string_prefix, enum_node_type_string);
        output = buffer_append_string_to_enum(
            output, enum_node, to_string_prefix, enum_node_type_string);
      }
    } else if (node->tag == PARSE_NODE_ENUM) {
      enum_node_t* enum_node = to_enum_node(node);
      output = buffer_append_enum_node(output, enum_node);
      output = buffer_printf(output, ";\n\n");
      char* enum_node_name = token_to_string(enum_node->name);
      char* to_string_prefix = remove_type_suffix(enum_node_name);
      char* enum_node_type_string = string_printf("enum %s", enum_node_name);
      output = buffer_append_enum_to_string(output, enum_node, to_string_prefix,
                                            enum_node_type_string);
      output = buffer_append_string_to_enum(output, enum_node, to_string_prefix,
                                            enum_node_type_string);
    }
  }

  return output;
}

__attribute__((warn_unused_result)) buffer_t*
    extract_prototypes_process_declarations(buffer_t* output,
                                            declarations_node_t* root) {
  uint64_t length = node_list_length(root->declarations);

  for (uint64_t i = 0; i < length; i++) {
    parse_node_t* node = node_list_get(root->declarations, i);
    if (node->tag == PARSE_NODE_FUNCTION) {
      function_node_t* fn_node = to_function_node(node);

      // Skip existing prototypes. We assume multiple prototypes are
      // allowed by ISO C is long as they are the same. This means
      // that we can extract prototypes and include them before
      // existing prototypes and the compiler will help make sure we
      // did this correctly before actually removing any
      // prototypes. We could also thus probably emit a prototype
      // for each prototype and besides having a duplicate (or more)
      // prototype, everything should technically be OK. Just
      // ignoring prototypes is certainly useful for debugging.
      if (fn_node->body == NULL) {
        continue;
      }

      // Normally prototypes aren't used for inline functions (and
      // normally they are declared static...) since the inline
      // functions often appears instead of their prototypes in a
      // header file. Honestly I haven't completely figured this out
      // yet. Skipping these prototypes for now if probably fine for
      // the demo especially since the wisely used ones will be be
      // static which we are skipping below anyways (for now).
      if (token_list_contains(fn_node->function_specifiers, "inline")) {
        continue;
      }
      // "static" for a "top-level" declaration means that the
      // declaration is not supposed to be visible outside the
      // compilation unit which sort of corresponds to a single "C"
      // file if the C pre-processor didn't exist -- or at least
      // that is where I eventually want to have Omni-C to
      // behave. At least for now, the simplest thing is to not try
      // to put them into any header files which we can revisit
      // later (along with static inline...)
      if (fn_node->storage_class_specifier != NULL
          && token_matches(fn_node->storage_class_specifier, "static")) {
        continue;
      }

      output = buffer_append_c_function_node_prototype(output, fn_node);
      output = buffer_printf(output, "\n");
    }
  }

  return output;
}

__attribute__((warn_unused_result)) buffer_t*
    extract_structures_process_declarations(buffer_t* output,
                                            declarations_node_t* root) {
  uint64_t length = node_list_length(root->declarations);

  for (uint64_t i = 0; i < length; i++) {
    parse_node_t* node = node_list_get(root->declarations, i);
    if (node->tag == PARSE_NODE_TYPEDEF) {
      typedef_node_t* typedef_node = to_typedef_node(node);
      if (is_struct_node(typedef_node->type_node->user_type)) {
        [[gnu::unused]] struct_node_t* struct_node
            = to_struct_node(typedef_node->type_node->user_type);
        /*
        output = buffer_printf(output, "typedef ");
        output = buffer_printf(output, "%s ",
                               token_to_string(typedef_node->name));
        output = buffer_append_enum_node(output, enum_node);
        output = buffer_printf(output, ";\n\n");

        char* enum_node_name = token_to_string(typedef_node->name);
        char* to_string_prefix = remove_type_suffix(enum_node_name);
        char* enum_node_type_string = enum_node_name;

        output = buffer_append_enum_to_string(
            output, enum_node, to_string_prefix, enum_node_type_string);
        output = buffer_append_string_to_enum(
            output, enum_node, to_string_prefix, enum_node_type_string);
        */
      }
    } else if (node->tag == PARSE_NODE_STRUCT) {
      [[gnu::unused]] struct_node_t* struct_node = to_struct_node(node);
      /*
      output = buffer_append_enum_node(output, enum_node);
      output = buffer_printf(output, ";\n\n");
      char* enum_node_name = token_to_string(enum_node->name);
      char* to_string_prefix = remove_type_suffix(enum_node_name);
      char* enum_node_type_string = string_printf("enum %s", enum_node_name);
      output = buffer_append_enum_to_string(output, enum_node, to_string_prefix,
                                            enum_node_type_string);
      output = buffer_append_string_to_enum(output, enum_node, to_string_prefix,
                                            enum_node_type_string);
      */
    }
  }

  return output;
}
