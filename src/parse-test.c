// TODO(jawilson): in theory these should differ only by the fn_t they
// call? (I guess the expression tests also look for a ";" - so two
// bits of deltas?) Probably easier to just make more top-down
// functions.

void parse_expression_string_and_print_parse_tree_from_buffer(
    buffer_t* input_buffer) {
  tokenizer_result_t tokenizer_result = tokenize(input_buffer);
  if (tokenizer_result.tokenizer_error_code) {
    fatal_error(ERROR_ILLEGAL_INPUT);
  }

  value_array_t* tokens = tokenizer_result.tokens;
  tokens = transform_tokens(
      tokens, compound_literal(token_transformer_options_t,
                               {
                                   .keep_whitespace = false,
                                   .keep_comments = false,
                                   .keep_javadoc_comments = false,
                                   .keep_c_preprocessor_lines = false,
                               }));

  pstate_t pstate = compound_literal(pstate_t, {0});
  pstate.use_statement_parser = true;
  pstate.tokens = tokens;
  if (!parse_expression(&pstate)) {
    fprintf(stderr, "FAIL\n");
    exit(1);
  }
  parse_node_t* node = pstate_get_result_node(&pstate);
  if (!pstate_expect_token_string(&pstate, ";")) {
    fprintf(stderr, "FAIL (expected ';')\n");
    exit(1);
  }
  buffer_t* output = make_buffer(1);
  buffer_append_dbg_parse_node(make_cdl_printer(output), node);
  if (FLAG_to_c) {
    buffer_append_string(output, "\n// C Output\n");
    printer_t* printer = make_printer(output, make_symbol_table(), 2);
    append_parse_node(printer, node);
  }
  fprintf(stdout, "%s\n", buffer_to_c_string(output));
}

void parse_statement_string_and_print_parse_tree_from_buffer(buffer_t* input_buffer) {
  tokenizer_result_t tokenizer_result = tokenize(input_buffer);
  if (tokenizer_result.tokenizer_error_code) {
    fatal_error(ERROR_ILLEGAL_INPUT);
  }
  value_array_t* tokens = tokenizer_result.tokens;
  tokens = transform_tokens(
      tokens, compound_literal(token_transformer_options_t,
                               {
                                   .keep_whitespace = false,
                                   .keep_comments = false,
                                   .keep_javadoc_comments = false,
                                   .keep_c_preprocessor_lines = false,
                               }));
  pstate_t state = compound_literal(pstate_t, {0});
  state.use_statement_parser = true;
  state.tokens = tokens;
  pstatus_t status = parse_statement(&state);
  if (!status) {
    fprintf(stderr, "FAIL");
    exit(1);
  }
  parse_node_t* node = pstate_get_result_node(&state);
  buffer_t* output = make_buffer(1);
  buffer_append_dbg_parse_node(make_cdl_printer(output), node);
  buffer_append_string(output, "\n// C Output\n");
  printer_t* printer = make_printer(output, make_symbol_table(), 2);
  append_parse_node(printer, node);
  fprintf(stdout, "%s\n", buffer_to_c_string(output));
}
