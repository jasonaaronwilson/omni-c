void bblock_to_buffer(buffer_t* buffer, roci_bb_t* bb) {
  buffer_printf(buffer, "%s:\n", uint64_to_string(cast(uint64_t, bb)));
  uint8_t* opcode_ptr = bblock_opcode_pointer(bb);
  uint64_t* data_ptr = bblock_data_pointer(bb);
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
    case ROCI_OPCODE_PUSH_INTEGER:
      buffer_printf(buffer, "    push %s\n", uint64_to_string(*(data_ptr++)));
      break;
    case ROCI_OPCODE_PUSH_TRUE:
      buffer_printf(buffer, "    push true\n");
      break;
    case ROCI_OPCODE_PUSH_FALSE:
      buffer_printf(buffer, "    push false\n");
      break;
    case ROCI_OPCODE_PUSH_DOUBLE:
      buffer_printf(buffer, "    push %lf\n",
                    raw_double_to_double(*(data_ptr++)));
      break;
    case ROCI_OPCODE_PUSH_STRING:
      buffer_printf(buffer, "    push \"%s\"\n", cast(char*, *(data_ptr++)));
      break;
    case ROCI_OPCODE_PUSH_SYMBOL:
      buffer_printf(buffer, "    push '%s'\n", cast(char*, *(data_ptr++)));
      break;
    default:
      buffer_printf(buffer, "    <unknown-opcode>\n");
      break;
    }
  }
}

double raw_double_to_double(uint64_t raw_bits) {
  double val = 0.0;
  memcpy(&val, &raw_bits, sizeof(val));
  return val;
}
