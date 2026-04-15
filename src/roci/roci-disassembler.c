void bblock_to_buffer(buffer_t* buffer, roci_bb_t* bb) {
  buffer_printf(buffer, "%s:\n", uint64_to_string(cast(uint64_t, bb)));
  uint8_t* opcode_ptr = bblock_opcode_pointer(bb);
  // uint64_t* data_ptr = bblock_data_pointer(bb);
  for (int i = 0; i < bb->num_opcodes; i++) {
    switch (*opcode_ptr++) {
    case ROCI_OPCODE_TRAP:
      buffer_printf(buffer, "    trap\n");
      break;
    case ROCI_OPCODE_DROP:
      buffer_printf(buffer, "    drop\n");
      break;
    case ROCI_OPCODE_RETURN:
      buffer_printf(buffer, "    return\n");
      break;
    default:
      buffer_printf(buffer, "    <unknown-opcode>\n");
      break;
    }
  }
}
