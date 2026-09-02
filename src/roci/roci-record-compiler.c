
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

  buffer_t* buffer = make_buffer(256);

  // First emit the consructor

  buffer_printf(buffer, "let make_%s = fn(", record_name);
  for (int i = 0; i < num_fields; i++) {
    if (i != 0) {
      buffer_printf(buffer, ",");
    }
    buffer_printf(buffer, "%s", fields[i]);
  }
  buffer_printf(buffer, "){");
  buffer_printf(buffer, "let result = make_record(\"%s\", %d);", record_name, num_fields);
  for (int i = 0; i < num_fields; i++) {
    buffer_printf(buffer, "record_set(result, %d, %s);", i, fields[i]);
  }
  buffer_printf(buffer, "return result;};");

  // Now the predicate

  buffer_printf(buffer,
                "let is_%s = "
                "fn(record){if(is_record(record)){if(string_equal(record_tag("
                "record), \"%s\")){return true;}}return false;};",
                record_name, record_name);

  // Finally the getters/setters

  char* record_tag_check = string_printf(
      "if(not(string_equal(record_tag(record),\"%s\"))){debug_error(\"\");}",
      record_name);

  for (int i = 0; i < num_fields; i++) {
    buffer_printf(
        buffer, "let %s_get_%s = fn(record){%sreturn record_get(record, %d);};",
        record_name, fields[i], record_tag_check, i);
    buffer_printf(buffer,
                  "let %s_set_%s = fn(record, value){%sreturn "
                  "record_set(record, %d, value);};",
                  record_name, fields[i], record_tag_check, i);
  }

  value_array_t* tokens = roci_tokenize_file(
      state, string_printf("*compile-record*%s", record_name), buffer);
  int position = state->position;
  for (int i = 0; i < tokens->length; i++) {
    value_array_insert_at(state->tokens, position++,
                          value_array_get(tokens, i));
  }
}
