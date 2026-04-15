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
  position = skip_whitespace_and_comments(buffer, position);
  buffer_clear(token);
  while (position < buffer_length(buffer)) {
    int ch = buffer_get(buffer, position);
    if (isspace(ch)) {
      return position;
    }
    buffer_append_byte(token, ch);
    position++;
  }

  return position;
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

    if (buffer_match_string_at(token, 0, "push")) {
    } else if (buffer_match_string_at(token, 0, "drop")) {
      buffer_append_byte(current_bb->opcodes, ROCI_OPCODE_DROP);
    } else if (buffer_match_string_at(token, 0, "br")) {
      // buffer_append_byte(current_bb->opcodes, ROCI_OPCODE_PUSH_STRING);
      // value_array_add(current_bb->data, ptr_to_value(str_val));
    } else if (buffer_match_string_at(token, 0, "br_false")) {
    } else if (buffer_match_string_at(token, 0, "call")) {
    } else if (buffer_match_string_at(token, 0, "return")) {
    } else if (buffer_match_string_at(token, 0, "trap")) {
      buffer_append_byte(current_bb->opcodes, ROCI_OPCODE_TRAP);
    }
  }

  return bblocks;
}
