void do_print_tokens(value_array_t* tokens, char* message) {
  if (FLAG_print_tokens_show_tokens) {
    // Experimentally, tokens when debug printed are pretty big no
    // matter the size of the actual token and some tokens like
    // comments can be quite large.
    buffer_t* buffer = make_buffer(tokens->length * 60);
    for (int j = 0; j < tokens->length; j++) {
      token_t* token = token_at(tokens, j);
      buffer = append_token_debug_string(buffer, *token);
      buffer = buffer_append_string(buffer, "\n");
    }
    fprintf(stdout, "** %s **\n%s\n", message, buffer_to_c_string(buffer));
  }
}

void print_tokens(void) {
  log_info("print_tokens()");

  value_array_t* files = read_files(FLAG_files);
  for (int i = 0; i < FLAG_files->length; i++) {
    file_t* file = value_array_get_ptr(files, i, typeof(file_t*));

    fprintf(stdout, "====================================================\n");
    fprintf(stdout, "====> Processing %s\n", file->file_name);
    fprintf(stdout, "====================================================\n");

    tokenizer_result_t tokenizer_result = tokenize(file->data);

    if (tokenizer_result.tokenizer_error_code) {
      log_warn("Tokenizer error: \"%s\"::%d -- %d",
               value_array_get(FLAG_files, i).str,
               tokenizer_result.tokenizer_error_position,
               tokenizer_result.tokenizer_error_code);
      continue;
    }

    value_array_t* tokens = tokenizer_result.tokens;

    if (FLAG_print_tokens_show_appended_tokens) {
      buffer_t* appended_tokens = make_buffer(1);
      debug_append_tokens(appended_tokens, tokens);
      fprintf(stdout, "%s", buffer_to_c_string(appended_tokens));
    }

    do_print_tokens(tokens, "before xform tokens");

    tokens = transform_tokens(
        tokens,
        compound_literal(
            token_transformer_options_t,
            {
                .keep_whitespace = FLAG_print_tokens_include_whitespace,
                .keep_comments = FLAG_print_tokens_include_comments,
                .keep_javadoc_comments = FLAG_print_tokens_include_comments,
                .keep_c_preprocessor_lines = false,
            }));

    do_print_tokens(tokens, "after xform tokens");

    if (FLAG_print_tokens_parse_and_print) {
      pstate_t pstate = compound_literal(
          pstate_t, {.tokens = tokens,
                     .use_statement_parser = FLAG_use_statement_parser});
      ;
      if (!parse_declarations(&pstate)) {
        pstate.error.file_name = file->file_name;
        buffer_t* buffer = make_buffer(1);
        buffer = buffer_append_human_readable_error(buffer, &(pstate.error));
        log_fatal("%s", buffer_to_c_string(buffer));
        fatal_error(ERROR_ILLEGAL_INPUT);
      } else {
        buffer_t* buffer = make_buffer(1024);
        buffer_append_dbg_parse_node(make_cdl_printer(buffer),
                                     pstate_get_result_node(&pstate));
        fprintf(stdout, "** Parse Nodes %s **\n%s\n", file->file_name,
                buffer_to_c_string(buffer));
      }
    }
  }
}
