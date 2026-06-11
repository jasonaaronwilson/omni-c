/**
 * @file roci-disassembler.c
 *
 * The roci disassembler obviously turns a roci_bb_t* into a textual
 * representation. The address is used to highlight a "current"
 * instruction.
 **/
void bblock_to_buffer(buffer_t* buffer, roci_bb_t* bb, uint8_t* address) {
  buffer_printf(buffer, "%s:\n", uint64_to_string(cast(uint64_t, bb)));
  uint8_t* opcode_ptr = bblock_opcode_pointer(bb);
  uint64_t* data_ptr = bblock_data_pointer(bb);
  for (int i = 0; i < bb->num_opcodes; i++) {
    if (opcode_ptr == address) {
      term_bold(buffer);
    }
    data_ptr += roci_instruction_to_buffer(buffer, opcode_ptr, data_ptr);
    if (opcode_ptr == address) {
      term_reset_formatting(buffer);
    }
    opcode_ptr++;
  }
}

uint32_t roci_instruction_to_buffer(buffer_t* buffer, uint8_t* opcode_ptr,
                                    uint64_t* data_ptr) {
  switch (*opcode_ptr) {
  case ROCI_OPCODE_TRAP:
    buffer_printf(buffer, "    trap\n");
    return 0;
  case ROCI_OPCODE_DROP:
    buffer_printf(buffer, "    drop\n");
    return 0;
  case ROCI_OPCODE_RETURN:
    buffer_printf(buffer, "    return\n");
    return 0;
  case ROCI_OPCODE_PUSH_INTEGER:
    buffer_printf(buffer, "    push %s\n", uint64_to_string(*(data_ptr)));
    return 1;
  case ROCI_OPCODE_PUSH_TRUE:
    buffer_printf(buffer, "    push true\n");
    return 0;
  case ROCI_OPCODE_PUSH_FALSE:
    buffer_printf(buffer, "    push false\n");
    return 0;
  case ROCI_OPCODE_PUSH_DOUBLE:
    buffer_printf(buffer, "    push %lf\n", raw_double_to_double(*(data_ptr)));
    return 1;
  case ROCI_OPCODE_PUSH_STRING:
    buffer_printf(buffer, "    push %s\n",
                  quote_c_string(cast(char*, *(data_ptr))));
    return 1;
  case ROCI_OPCODE_BR:
    buffer_printf(buffer, "    br %s\n", uint64_to_string(*(data_ptr)));
    return 1;
  case ROCI_OPCODE_BR_TRUE:
    buffer_printf(buffer, "    br_true %s\n", uint64_to_string(*(data_ptr)));
    return 1;
  case ROCI_OPCODE_NEW_ENVIRONMENT:
    buffer_printf(buffer, "    new_env\n");
    return 0;
  case ROCI_OPCODE_DROP_ENVIRONMENT:
    buffer_printf(buffer, "    drop_env\n");
    return 0;
  case ROCI_OPCODE_GET_VAR:
    buffer_printf(buffer, "    get_var %s\n", cast(char*, *(data_ptr)));
    return 1;
  case ROCI_OPCODE_SET_VAR:
    buffer_printf(buffer, "    set_var %s\n", cast(char*, *(data_ptr)));
    return 1;
  case ROCI_OPCODE_DEFINE_VAR:
    buffer_printf(buffer, "    define_var %s\n", cast(char*, *(data_ptr)));
    return 1;
  case ROCI_OPCODE_COMMENT:
    buffer_printf(buffer, "    ; %s\n", cast(char*, *(data_ptr)));
    return 1;

  case ROCI_OPCODE_CALL_0:
  case ROCI_OPCODE_CALL_1:
  case ROCI_OPCODE_CALL_2:
  case ROCI_OPCODE_CALL_3:
  case ROCI_OPCODE_CALL_4:
  case ROCI_OPCODE_CALL_5:
  case ROCI_OPCODE_CALL_6:
  case ROCI_OPCODE_CALL_7:
  case ROCI_OPCODE_CALL_8:
  case ROCI_OPCODE_CALL_9:
  case ROCI_OPCODE_CALL_10:
  case ROCI_OPCODE_CALL_11:
  case ROCI_OPCODE_CALL_12:
  case ROCI_OPCODE_CALL_13:
  case ROCI_OPCODE_CALL_14:
  case ROCI_OPCODE_CALL_15:
  case ROCI_OPCODE_CALL_16:
    buffer_printf(buffer, "    call_%d %s\n", *opcode_ptr - ROCI_OPCODE_CALL_0,
                  uint64_to_string(*(data_ptr)));
    return 1;
  case ROCI_OPCODE_MAKE_CLOSURE:
    buffer_printf(buffer, "    make_closure %s\n",
                  uint64_to_string(*(data_ptr)));
    return 1;
  case ROCI_OPCODE_CHECK_ARGS:
    buffer_printf(buffer, "    check_args %s\n", uint64_to_string(*(data_ptr)));
    return 1;
  case ROCI_OPCODE_DEBUG_INFO: {
    roci_src_info_t info = *(data_ptr);
    buffer_printf(buffer, "    debug_info %s, %s\n",
                  uint64_to_string(roci_src_file_number(info)),
                  uint64_to_string(roci_src_line_number(info)));
    return 1;
  }
  default:
    buffer_printf(buffer, "    <unknown-opcode>\n");
    return 0;
  }
  /* Not Reached? */
  return 0;
}

double raw_double_to_double(uint64_t raw_bits) {
  double val = 0.0;
  memcpy(&val, &raw_bits, sizeof(val));
  return val;
}

void disassemble_bblocks(value_array_t* bblocks, buffer_t* buffer) {
  for (int i = 0; i < bblocks->length; i++) {
    bblock_to_buffer(
        buffer, value_array_get_ptr(bblocks, i, typeof(roci_bb_t*)), nullptr);
  }
}
