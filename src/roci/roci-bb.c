/**
 * @file roci-bb.c
 *
 * This abstraction is meant to be simple enough that real world
 * silicon (or more likely an FPGA) could implement it efficently.
 *
 * What we want is a stream of 8 bit opcodes and a stream of 64bit
 * aligned datums. For very small basic blocks, which I think I'm
 * going to be producing, we might want to be clever for example a
 * single 24bit opcode length follow by opcodes followed by aligned
 * datums.
 */

typedef roci_bb_t = struct {
  uint32_t num_data;
  uint32_t num_opcodes;
  // data
  // opcodes + alignment nops
};

/**
 * @function bblock_opcode_pointer
 *
 * Compute the initial opcode pointer from a fullly linked
 * bblock. Desipite num_opcodes, you should just stop looking after an
 * unconditional branch.
 */
inline uint8_t* bblock_opcode_pointer(roci_bb_t* bb) {
  return cast(uint8_t*, bb) + 8 + bb->num_data * 8;
}

/**
 * @function bblock_data_pointer
 *
 * Compute the initial data pointer from a fullly linked bblock.
 */
inline uint64_t* bblock_data_pointer(roci_bb_t* bb) {
  return cast(uint64_t*, bb) + 1;
}
