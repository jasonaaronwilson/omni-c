/**
 * @file roci-bb-builder.c
 *
 * This code is used by roci-assembler.c and in the future
 * roci-parser.c to build all of the low-level bblocks and eventually
 * "link" them via their real addresses in memory rather than labels.
 */

typedef roci_bb_builder_t = struct {
  char* bblock_label;
  roci_bb_t* bblock;
  value_array_t* data;
  buffer_t* opcodes;
};

typedef roci_bb_builder_array_t = value_array_t;

/**
 * @function add_bblock
 *
 */
roci_bb_builder_t* add_bblock(roci_bb_builder_array_t* bblocks) {
  roci_bb_builder_t* result = malloc_struct(roci_bb_builder_t);
  result->opcodes = make_buffer(10);
  result->data = make_value_array(1);
  value_array_add(bblocks, ptr_to_value(result));
  return result;
}

/**
 * @function
 *
 * Build and link all of the bblocks into the interpreter efficient
 * format and return the address of the first bblock.
 *
 * If there are no bblocks, then NULL is returned.
 *
 * This is slightly tricky to use and will evolve over time at which
 * time hopefully I will update the documentation.
 */
roci_bb_t* build_bblocks(roci_bb_builder_array_t* bblocks) {

  log_info("roci -- build_bblocks\n");

  // First allocate all of the bblocks before building the final
  // contents so we can "link" everything properly.
  for (int i = 0; i < bblocks->length; i++) {
    roci_bb_builder_t* builder
        = value_array_get_ptr(bblocks, i, typeof(roci_bb_builder_t*));

    log_info("roci -- builder %d, %d\n", builder->data->length,
             builder->opcodes->length);

    roci_bb_t* bblock
        = malloc((1 + builder->data->length) * 8 + builder->opcodes->length);
    bblock->num_data = builder->data->length;
    bblock->num_opcodes = builder->opcodes->length;
    builder->bblock = bblock;
  }

  // Now copy over opcodes and data converting bblock numbers to
  // bblock addresses
  for (int i = 0; i < bblocks->length; i++) {
    copy_opcodes_and_link(
        bblocks, value_array_get_ptr(bblocks, i, typeof(roci_bb_builder_t*)));
  }

  return value_array_get_ptr(bblocks, 0, typeof(roci_bb_builder_t*))->bblock;
}

void copy_opcodes_and_link(roci_bb_builder_array_t* bblocks,
                           roci_bb_builder_t* builder) {
  roci_bb_t* bb = builder->bblock;
  uint8_t* opcode_ptr = bblock_opcode_pointer(bb);
  uint64_t* data_ptr = bblock_data_pointer(bb);
  int dindex = 0;

  for (int i = 0; i < builder->opcodes->length; i++) {
    roci_opcode_t opcode = buffer_get(builder->opcodes, i);
    *(opcode_ptr++) = opcode;
    switch (opcode) {
    case ROCI_OPCODE_PUSH_INTEGER:
    case ROCI_OPCODE_PUSH_DOUBLE:
    case ROCI_OPCODE_PUSH_STRING:
    case ROCI_OPCODE_GET_VAR:
    case ROCI_OPCODE_SET_VAR:
    case ROCI_OPCODE_DEFINE_VAR:
      *(data_ptr++) = value_array_get(builder->data, dindex++).u64;
      break;

    case ROCI_OPCODE_BR_FALSE:
    case ROCI_OPCODE_BR:
      char* label = value_array_get_ptr(builder->data, dindex++, typeof(char*));
      *(data_ptr++) = bb_label_to_address(bblocks, label);
      break;

    default:
      break;
    }
  }
}

uint64_t bb_label_to_address(roci_bb_builder_array_t* bblocks, char* label) {
  for (int i = 0; i < bblocks->length; i++) {
    roci_bb_builder_t* builder
        = value_array_get_ptr(bblocks, i, typeof(roci_bb_builder_t*));
    if (string_equal(builder->bblock_label, label)) {
      return cast(uint64_t, builder->bblock);
    }
  }
  log_fatal("bblock with label %s not found", label);
  fatal_error(ERROR_ILLEGAL_STATE);
  return 0;
}
