#line 2 "symbol-table-builder.c"
#ifndef _SYMBOL_TABLE_BUILDER_H_
#define _SYMBOL_TABLE_BUILDER_H_

#include "preprocessor.h"
#include "symbol-table.h"

// Nothing besides protos and includes

#include "symbol-table-builder.c.generated.h"

#endif /* _SYMBOL_TABLE_BUILDER_H_ */

/**
 * @function parse_and_add_top_level_definitions
 *
 * This is the root of the input side of the compiler. It takes file
 * names, reads them, and puts parse trees into the passed in symbol
 * table. This would be an excellent place to use multi-threading to
 * easily achieve a big speed-up. (To keep the transpiler
 * well-behaved, each file can be parsed into it's own symbol-table
 * and then each symbol-table can be merged into the passed in
 * symbol-table to have the same ordering for reproducible builds.
 */
void parse_and_add_top_level_definitions(symbol_table_t* symbol_table,
                                         value_array_t* file_names) {
  value_array_t* files = read_files(file_names);
  for (int i = 0; i < files->length; i++) {
    oc_file_t* file = (oc_file_t*) value_array_get(files, i).ptr;
    symbol_table_parse_buffer(symbol_table, file->data, file->file_name);
  }
}

/**
 * @function symbol_table_parse_buffer
 *
 * Given a buffer, call the tokenizer, handle C pre-processor
 * directives, and then parse and add the top-level
 * definitions/declaration to the given symbol table. This is normally
 * called by parse_and_add_top_level_definitions though this is also
 * called by source code generation routines like
 * srcgen_enum_to_string_converters.
 */
void symbol_table_parse_buffer(symbol_table_t* symbol_table, buffer_t* buffer,
                               char* file_name) {
  oc_tokenizer_result_t tokenizer_result = tokenize(buffer);
  if (tokenizer_result.tokenizer_error_code) {
    log_warn("Tokenizer error: \"%s\"::%d -- %d", file_name,
             tokenizer_result.tokenizer_error_position,
             tokenizer_result.tokenizer_error_code);
    fatal_error(ERROR_ILLEGAL_INPUT);
  }
  value_array_t* tokens = tokenizer_result.tokens;

  handle_c_preprocessor_directives(
      (c_preprocess_options_t){
          .keep_system_includes = true,
          .keep_user_includes = false,
      },
      symbol_table, tokens);

  tokens = transform_tokens(tokens, (token_transformer_options_t){
                                        .keep_whitespace = false,
                                        .keep_comments = false,
                                        .keep_javadoc_comments = false,
                                        .keep_c_preprocessor_lines = false,
                                    });
  parse_result_t declarations_result = parse_declarations(tokens, 0);
  if (is_error_result(declarations_result)) {
    declarations_result.parse_error.file_name = file_name;
    buffer_t* buffer = make_buffer(1);
    buffer = buffer_append_human_readable_error(
        buffer, &(declarations_result.parse_error));
    log_fatal("%s", buffer_to_c_string(buffer));
    fatal_error(ERROR_ILLEGAL_INPUT);
  }

  declarations_node_t* root = to_declarations_node(declarations_result.node);
  symbol_table_add_declartions(symbol_table, root);
}
