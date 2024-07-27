#line 2 "symbol-table-builder.c"
#ifndef _SYMBOL_TABLE_BUILDER_H_
#define _SYMBOL_TABLE_BUILDER_H_

#include "symbol-table.h"
#include "preprocessor.h"

// Nothing besides protos and includes

#include "symbol-table-builder.c.generated.h"

#endif /* _SYMBOL_TABLE_BUILDER_H_ */

void parse_and_add_top_level_definitions(symbol_table_t* symbol_table,
                                             value_array_t* file_names) {
  value_array_t* files = read_files(file_names);
  for (int i = 0; i < files->length; i++) {
    oc_file_t* file = (oc_file_t*) value_array_get(files, i).ptr;

    oc_tokenizer_result_t tokenizer_result = tokenize(file->data);
    if (tokenizer_result.tokenizer_error_code) {
      log_warn("Tokenizer error: \"%s\"::%d -- %d", file->file_name,
               tokenizer_result.tokenizer_error_position,
               tokenizer_result.tokenizer_error_code);
      fatal_error(ERROR_ILLEGAL_INPUT);
    }
    value_array_t* tokens = tokenizer_result.tokens;

    // add_includes(symbol_table, tokens);

    tokens = transform_tokens(tokens, (token_transformer_options_t){
                                          .keep_whitespace = false,
                                          .keep_comments = false,
                                          .keep_javadoc_comments = false,
                                          .keep_c_preprocessor_lines = false,
                                      });
    parse_result_t declarations_result = parse_declarations(tokens, 0);
    if (is_error_result(declarations_result)) {
      declarations_result.parse_error.file_name = file->file_name;
      buffer_t* buffer = make_buffer(1);
      buffer = buffer_append_human_readable_error(
          buffer, &(declarations_result.parse_error));
      log_fatal("%s", buffer_to_c_string(buffer));
      fatal_error(ERROR_ILLEGAL_INPUT);
    }

    declarations_node_t* root = to_declarations_node(declarations_result.node);
    symbol_table_add_declartions(symbol_table, root);
  }
}

