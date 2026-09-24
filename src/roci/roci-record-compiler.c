
/* ================================================================ */
/* records */
/* ================================================================ */

// record foo { a, b, c };
//
// ==> let make_foo = fn(a,b,c) {...};
// ==> let is_foo = fn(x) {...};
// ==> let foo_get_a = fn(x) {}; and let foo_set_a = fn(x, y) {}; etc.

// Essentially we want to eat all of the tokens and then insert a
// token stream all with line numbers of the initial record token.

void roci_compile_record(roci_compiler_state_t* state) {
  token_t* first_token = roci_peek_token(state);
  roci_expect_token(state, "record");
  char* record_name = token_to_string(roci_next_token(state));
  roci_expect_token(state, "{");

  int num_fields = 0;
  char* fields[128] = {0};

  do {
    token_t* field = roci_next_token(state);
    if (token_matches(field, "}")) {
      break;
    }
    if (num_fields == 128) {
      roci_compiler_error(state, ROCI_COMPILE_TIME_ERROR_TOO_MANY_FIELDS);
    }
    roci_verify_identifier(state, field);
    fields[num_fields++] = token_to_string(field);
    token_t* punc = roci_peek_token(state);
    if (token_matches(punc, ",")) {
      punc = roci_next_token(state);
    } else if (!token_matches(punc, "}")) {
      roci_compiler_error(state, ROCI_COMPILE_TIME_ERROR_BAD_STATEMENT);
    }
  } while (true);

  // TODO(jawilson): Obviously we should have already cached this
  // object so only one copy even if we make 100 records. I think this
  // would be easier if we had handles or something like that... I
  // guess we could smuggle the pointer into an integer...

  roci_record_fields_t* metadata
      = make_roci_record_fields_from_names(record_name, num_fields, fields);
  int record_metadata_num = save_record_metadata(metadata);

  buffer_t* buffer = make_buffer(256);

  // First emit the consructor

  buffer_printf(buffer, "let make_%s = fn(", record_name);
  for (int i = 0; i < num_fields; i++) {
    if (i != 0) {
      buffer_printf(buffer, ",");
    }
    buffer_printf(buffer, "%s", fields[i]);
  }
  buffer_printf(buffer, ") {\n");
  buffer_printf(buffer, "  let result = make_record(%d);\n",
                record_metadata_num);
  for (int i = 0; i < num_fields; i++) {
    buffer_printf(buffer, "  record_set(result, %d, %s);\n", i, fields[i]);
  }
  buffer_printf(buffer, "  return result;\n};\n");

  // Now the predicate

  buffer_printf(buffer, "\nlet is_%s = fn(record) {\n", record_name);
  buffer_printf(buffer, "  if (not(is_record(record))) {\n");
  buffer_printf(buffer, "    return false;\n");
  buffer_printf(buffer, "  }\n");
  buffer_printf(buffer, "  return _match_record_metadata(record, %d);\n",
                record_metadata_num);
  buffer_printf(buffer, "};\n");

  // Finally the getters/setters. We can remove these once we modify
  // the compiler to understand "."

  char* record_tag_check = string_printf(
      "\n  if (not(_match_record_metadata(record, %d))) { debug_error(\"Wrong "
      "record type\"); }",
      record_metadata_num);

  for (int i = 0; i < num_fields; i++) {
    buffer_printf(buffer,
                  "\nlet %s_get_%s = fn(record){%s\n  return "
                  "record_get(record, %d);\n};\n",
                  record_name, fields[i], record_tag_check, i);
    buffer_printf(buffer,
                  "\nlet %s_set_%s = fn(record, value){%s\n  return "
                  "record_set(record, %d, value);\n};\n",
                  record_name, fields[i], record_tag_check, i);
  }

  buffer_printf(buffer, "\n// Done %s\n", record_name);

  // debug generated code...
  // buffer_write_all(stderr, buffer);

  value_array_t* tokens = roci_tokenize_file(
      state, string_printf("*compile-record*%s", record_name), buffer);
  int position = state->position;
  for (int i = 0; i < tokens->length; i++) {
    value_array_insert_at(state->tokens, position++,
                          value_array_get(tokens, i));
  }
}
