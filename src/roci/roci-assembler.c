/**
 * @file
 *
 * This is a very simple assembler for testing the roci interpreter.
 *
 * It deals with opcodes, constants, labels, and comments.
 *
 * ```
 * ; Simple function implementing
 * ; "return my_func(0x12345, 3.14, "Hello world!")"
 * 0:
 *    push 0x12345
 *    push 3.14
 *    push "Hello world!"
 *    push 'my_func'
 *    push 3
 *    call 1
 * 1:
 *    return
 * ```
 */

uint64_t skip_whitespace_and_comments(buffer_t* buffer, uint64_t position) {
  log_info("skip_whitespace_and_comments");
  uint64_t length = buffer_length(buffer);
  while (position < length) {
    char ch = buffer_get(buffer, position);
    if (isspace(ch)) {
      position++;
    } else if (ch == ';') {
      ch = buffer_get(buffer, ++position);
      while (ch != '\n') {
        ch = buffer_get(buffer, ++position);
      }
      position++;
    } else {
      return position;
    }
  }
  return position;
}

uint64_t read_roci_token(buffer_t* buffer, uint64_t position, buffer_t* token) {
  log_info("read_roci_token");
  buffer_clear(token);
  position = skip_whitespace_and_comments(buffer, position);
  while (position < buffer_length(buffer)) {
    int ch = buffer_get(buffer, position);
    position++;
    if (isspace(ch)) {
      return position;
    }
    buffer_append_byte(token, ch);
  }

  return position;
}

boolean_t token_is_double(buffer_t* token) {
  for (int i = 0; i < token->length; i++) {
    if (buffer_get(token, i) == '.') {
      return true;
    }
  }
  return false;
}

/**
 * @function
 *
 * Parse the assembly string and build out the basic blocks using
 * roci_bb_builder_t and roci_bb_builder_array_t.
 */
roci_bb_builder_array_t* roci_assemble(buffer_t* buffer) {
  log_info("roci_assemble");

  roci_bb_builder_array_t* bblocks = make_value_array(16);
  roci_bb_builder_t* current_bb = NULL;
  buffer_t* token = make_buffer(16);

  uint64_t position = 0;
  while (position < buffer_length(buffer)) {
    position = read_roci_token(buffer, position, token);
    if (buffer_length(buffer) == 0) {
      break;
    }

    if (buffer_ends_with(token, ":")) {
      // Utilize the builder struct explicitly
      current_bb = add_bblock(bblocks);
      current_bb->bblock_label
          = buffer_c_substring(token, 0, buffer_length(token) - 1);
      continue;
    }

    if (!current_bb) {
      log_fatal(
          "Assembler error: Instruction found outside of a basic block label.");
      fatal_error(ERROR_ILLEGAL_STATE);
    }

    if (buffer_equal(token, "push")) {
      position = read_roci_token(buffer, position, token);
      if (buffer_length(buffer) == 0) {
        fatal_error(ERROR_ILLEGAL_STATE);
      }

      int first_char = buffer_get(token, 0);

      if (first_char == '"') {
        buffer_append_byte(current_bb->opcodes, ROCI_OPCODE_PUSH_STRING);
        value_array_add(
            current_bb->data,
            u64_to_value(
                cast(uint64_t,
                     buffer_c_substring(token, 1, buffer_length(token) - 1))));
      } else if (buffer_equal(token, "true")) {
        buffer_append_byte(current_bb->opcodes, ROCI_OPCODE_PUSH_TRUE);
      } else if (buffer_equal(token, "false")) {
        buffer_append_byte(current_bb->opcodes, ROCI_OPCODE_PUSH_FALSE);
      } else {
        if (isdigit(first_char)) {
          if (token_is_double(token)) {
            double dbl = string_parse_double(buffer_to_c_string(token));
            buffer_append_byte(current_bb->opcodes, ROCI_OPCODE_PUSH_DOUBLE);
            value_array_add(current_bb->data, dbl_to_value(dbl));
          } else {
            buffer_append_byte(current_bb->opcodes, ROCI_OPCODE_PUSH_INTEGER);
            value_array_add(
                current_bb->data,
                u64_to_value(
                    string_parse_uint64(buffer_to_c_string(token)).u64));
          }
        } else {
          fatal_error(ERROR_ILLEGAL_STATE);
        }
      }

    } else if (buffer_equal(token, "drop")) {
      buffer_append_byte(current_bb->opcodes, ROCI_OPCODE_DROP);
    } else if (buffer_equal(token, "br")) {
      position = read_roci_token(buffer, position, token);
      if (buffer_length(buffer) == 0) {
        fatal_error(ERROR_ILLEGAL_STATE);
      }
      buffer_append_byte(current_bb->opcodes, ROCI_OPCODE_BR);
      value_array_add(current_bb->data,
                      ptr_to_value(buffer_to_c_string(token)));
    } else if (buffer_equal(token, "br_false")) {
      position = read_roci_token(buffer, position, token);
      if (buffer_length(buffer) == 0) {
        fatal_error(ERROR_ILLEGAL_STATE);
      }
      buffer_append_byte(current_bb->opcodes, ROCI_OPCODE_BR_FALSE);
      value_array_add(current_bb->data,
                      ptr_to_value(buffer_to_c_string(token)));
    } else if (buffer_equal(token, "call")) {
    } else if (buffer_equal(token, "return")) {
      buffer_append_byte(current_bb->opcodes, ROCI_OPCODE_RETURN);
    } else if (buffer_equal(token, "trap")) {
      buffer_append_byte(current_bb->opcodes, ROCI_OPCODE_TRAP);
    } else if (buffer_equal(token, "get_var")) {
      position = read_roci_token(buffer, position, token);
      if (buffer_length(buffer) == 0) {
        fatal_error(ERROR_ILLEGAL_STATE);
      }
      buffer_append_byte(current_bb->opcodes, ROCI_OPCODE_GET_VAR);
      value_array_add(current_bb->data,
                      str_to_value(buffer_to_c_string(token)));
    } else if (buffer_equal(token, "set_var")) {
      position = read_roci_token(buffer, position, token);
      if (buffer_length(buffer) == 0) {
        fatal_error(ERROR_ILLEGAL_STATE);
      }
      buffer_append_byte(current_bb->opcodes, ROCI_OPCODE_SET_VAR);
      value_array_add(current_bb->data,
                      str_to_value(buffer_to_c_string(token)));
    } else if (buffer_equal(token, "define_var")) {
      position = read_roci_token(buffer, position, token);
      if (buffer_length(buffer) == 0) {
        fatal_error(ERROR_ILLEGAL_STATE);
      }
      buffer_append_byte(current_bb->opcodes, ROCI_OPCODE_DEFINE_VAR);
      value_array_add(current_bb->data,
                      str_to_value(buffer_to_c_string(token)));
    } else {
      log_fatal("unrecognized opcode '%s' at position %d",
                buffer_to_c_string(token), buffer_length(token), position);
      fatal_error(ERROR_ILLEGAL_STATE);
    }
  }

  return bblocks;
}
