/**
 * @file
 *
 * This is a very simple assembler for testing the roci interpreter.
 *
 * It deals with opcodes, constants, and labels. For now, labels are
 * simple integers followed by a colon and they are expected to come
 * in order starting at zero with no skips. Opcodes are simple
 * mnemonics like "br", strings are in double quotes while symbols are
 * in single quotes, integers and doubles are like they would be in
 * C. Line comments begin with a semi-colon and continue until "\n".
 *
 * ```
 * ; Simple function implementing "return my_func(0x12345, 3.14, "Hello
 * world!")"
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

// Advances pointer past spaces and line comments
static void skip_whitespace_and_comments(const char** p) {
  while (**p) {
    if (isspace(**p)) {
      (*p)++;
    } else if (**p == ';') {
      while (**p && **p != '\n')
        (*p)++;
    } else {
      break;
    }
  }
}

/**
 * @function
 *
 * Parse the assembly string and build out the basic blocks using
 * roci_bb_builder_t and roci_bb_builder_array_t.
 */
roci_bb_builder_array_t* roci_assemble(const char* source) {
  roci_bb_builder_array_t* bblocks = make_value_array(16);
  roci_bb_builder_t* current_bb = NULL;
  const char* p = source;

  while (*p) {
    skip_whitespace_and_comments(&p);
    if (!*p)
      break;

    // 1. Parse Labels (e.g., "0:", "1:")
    if (isdigit(*p)) {
      char* endptr = NULL;
      long long label_num = strtoll(p, &endptr, 10);
      if (*endptr == ':') {
        p = endptr + 1; // Consume the colon

        // Utilize the builder struct explicitly
        current_bb = add_bblock(bblocks);
        current_bb->bblock_number = label_num;
        current_bb->bblock = NULL; // To be populated by build_bblocks later
        continue;
      }
    }

    if (!current_bb) {
      log_fatal(
          "Assembler error: Instruction found outside of a basic block label.");
    }

    // 2. Parse Mnemonics
    char mnemonic[32] = {0};
    int i = 0;
    while (isalpha(*p) || *p == '_') {
      mnemonic[i++] = *(p++);
    }
    mnemonic[i] = '\0';

    skip_whitespace_and_comments(&p);

    // 3. Match Mnemonics and populate builder->opcodes and builder->data
    if (strcmp(mnemonic, "push") == 0) {
      if (*p == '"') {
        // String Literal
        p++;
        const char* start = p;
        while (*p && *p != '"')
          p++;
        char* str_val = strndup(start, p - start);
        p++; // Consume closing quote

        buffer_append_byte(current_bb->opcodes, ROCI_OPCODE_PUSH_STRING);
        value_array_add(current_bb->data, ptr_to_value(str_val));

      } else if (*p == '\'') {
        // Symbol Literal
        p++;
        const char* start = p;
        while (*p && *p != '\'')
          p++;
        char* sym_val = strndup(start, p - start);
        p++; // Consume closing quote

        buffer_append_byte(current_bb->opcodes, ROCI_OPCODE_PUSH_SYMBOL);
        value_array_add(current_bb->data, ptr_to_value(sym_val));

      } else if (strncmp(p, "true", 4) == 0) {
	// This feels a bit wrong since we don't know the character
	// after "true"
        buffer_append_byte(current_bb->opcodes, ROCI_OPCODE_PUSH_TRUE);
        p += 4;
      } else if (strncmp(p, "false", 5) == 0) {
	// This feels a bit wrong since we don't know the character
	// after "false"
        buffer_append_byte(current_bb->opcodes, ROCI_OPCODE_PUSH_FALSE);
        p += 5;
      } else {
        // Number (Integer or Double)
        char* endptr;
        if (strchr(p, '.') != NULL && !isspace(*strchr(p, '.'))) {
          double d_val = strtod(p, &endptr);
          buffer_append_byte(current_bb->opcodes, ROCI_OPCODE_PUSH_DOUBLE);

          // Bitcast float to uint64_t to safely pack into the data array
          uint64_t u_val;
          memcpy(&u_val, &d_val, sizeof(double));
          value_array_add(current_bb->data, u64_to_value(u_val));
          p = endptr;
        } else {
          long long i_val = strtoll(p, &endptr, 0);
          buffer_append_byte(current_bb->opcodes, ROCI_OPCODE_PUSH_INTEGER);
          value_array_add(current_bb->data, u64_to_value(i_val));
          p = endptr;
        }
      }
    } else if (strcmp(mnemonic, "br") == 0) {
      long long target_bb = strtoll(p, cast(char**, &p), 10);
      buffer_append_byte(current_bb->opcodes, ROCI_OPCODE_BR);
      value_array_add(current_bb->data, u64_to_value(target_bb));
    } else if (strcmp(mnemonic, "br_false") == 0) {
      long long target_bb = strtoll(p, cast(char**, &p), 10);
      buffer_append_byte(current_bb->opcodes, ROCI_OPCODE_BR_FALSE);
      value_array_add(current_bb->data, u64_to_value(target_bb));
    } else if (strcmp(mnemonic, "call") == 0) {
      long long num_args = strtoll(p, cast(char**, &p), 10);
      buffer_append_byte(current_bb->opcodes, ROCI_OPCODE_CALL);
      value_array_add(current_bb->data, u64_to_value(num_args));
    } else if (strcmp(mnemonic, "return") == 0) {
      buffer_append_byte(current_bb->opcodes, ROCI_OPCODE_RETURN);
    } else if (strcmp(mnemonic, "drop") == 0) {
      buffer_append_byte(current_bb->opcodes, ROCI_OPCODE_DROP);
    } else {
      log_fatal("Assembler error: Unknown mnemonic.");
    }
  }

  return bblocks;
}
