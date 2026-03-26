 //
// Explanation of why this file is about this change massively:
//
// I've had a change of heart on the direction for interpretaton.
//
// I fooled around on paper and I'm simply curious about how a stack
// machine would work with a tagged stack, tagged environment, and an
// untagged heap (since we use a conservative collector). Eventually
// we will need a tagged array.
//
// The idea to the tagged stack is to use two stack pointers, one to
// 64bit words, and one 1/8 of the size to hold a convenient 8-bit
// tag. (So two allocations instead of one, they can live anywhere, no
// spatial locality *required* but really small stacks are possible I
// guess). Incrementing or decrementing these pointers can be done in
// parallel and since this is an interpreted virtual machine and real
// machines are very wide now, there is essentially no real code size
// bloat (the size of the VM is a constant, dynamicly compile
// code...). Atomicity is maybe a downside but we have to deal with
// that in many ways on modern processors now (a conservative
// collector doesn't care but worth considering as a more general
// mode). Otherwise squint and pretend we have a 72bit computer! If
// you *trust* what you are loading from the heap, which you can,
// because of our strict code generation protocol. The tag stack only
// puts an extra 1/8 burden on the memory system (aka the TLB).
//
// The interesting part to me is actually the "instruction stream",
// namely what if it was also split that into two as well (we'll call
// them the opcode stream and an immediate stream). In this case the
// opcode stream always increments on every virtual cycle but the
// immediate stream only increments when needed. One goes a byte at a
// time, the other goes 64bits at a time (in the simplest version
// without having to do lot of adds for padding, not that big of a
// deal). If we further virtualize each basic block such that there is
// no assumed fallthrough (so each bblock ends in a two way branch
// (simply consuming two immediates), each basic block can have a
// header.
//
// The simplest header is actually just the offset to the immedidate
// stream (or a pointer to it if we want to waste space and make them
// 64bits). We have enough information to set *pc* and
// *immediates-pc*. A single byte might limit the size of a basic
// block to a few hundred instructions (though always easy to branch
// to a new basic block, just saying...). Like I said earlier, it
// could just be a full pointer, just not necessary. ULEB128 is not
// *super* crazy option.
//
// The simplest scheme laid out in memory looks like: header | opcode
// stream | [?padding (nops!)?] | 64bit aligned immediate stream.
//
// If the opcode and immediate stream pointers have spatial locality,
// that is probably going to be good for performance and this is the
// norm with short basic blocks (loads from nearby locations are are
// optimized by all modern processors).
//
// How dense is this encoding?
//
// My first guess is that this encoding is not very dense especially
// on a 64bit machine because most basic blocks are very short. (Two
// way branches are less crazy on a 32bit version of this machine.) It
// will probably be better in strange ways like debugging.
//
// We are kind of just getting started: I want to do some funky
// dynamic dispatch based on the tag stack to find our next "dynamic"
// target (a hash lookup) and create a sensible but unusual machine
// that will require further insight.

/**
 * @file
 *
 * Roci is an interpreter for a "nano" Lisp/Scheme interpreter with an
 * intentionally minimal runtime (very limited I/O). It's intended to
 * be used as a configuration language for a cross platform build tool.
 *
 * The tokenizer and parser adhere to C like syntax instead of a
 * traditional s-expression syntax.
 */

// registers: arg_stack, env_stack, return_stack

typedef roci_tag_t = enum {
  ROCI_TAG_UNKNOWN,
  ROCI_TAG_BOOLEAN,
  ROCI_TAG_INTEGER,
  ROCI_TAG_DOUBLE,
  ROCI_TAG_STRING,
  ROCI_TAG_SYMBOL,
  ROCI_TAG_CLOSURE,
  ROCI_TAG_C_PRIMITIVE,
  ROCI_TAG_ARRAY,
};

typedef roci_opcode_t = enum {
  ROCI_OPCODE_NOP,
  ROCI_OPCODE_PUSH_BOOLEAN,
  ROCI_OPCODE_PUSH_INTEGER,
  ROCI_OPCODE_PUSH_DOUBLE,
  ROCI_OPCODE_PUSH_STRING,
  ROCI_OPCODE_PUSH_SYMBOL,

  ROCI_OPCODE_NEW_ENVIRONMENT,
  ROCI_OPCODE_DEFINE_VAR,
  ROCI_OPCODE_GET_VAR,
  ROCI_OPCODE_SET_VAR,

  // Unconditional branch with a single target
  ROCI_OPCODE_JUMP,
  // Conditional branch with two branch targets and a conditional
  ROCI_OPCODE_BRANCH,

  ROCI_OPCODE_CALL_0,
  ROCI_OPCODE_CALL_1,
  ROCI_OPCODE_CALL_2,
  ROCI_OPCODE_CALL_3,
  ROCI_OPCODE_CALL_4,
  ROCI_OPCODE_CALL_5,
  ROCI_OPCODE_CALL_6,
  ROCI_OPCODE_CALL_7,
  ROCI_OPCODE_CALL_8,
  ROCI_OPCODE_CALL_9,
  ROCI_OPCODE_CALL_10,
  ROCI_OPCODE_CALL_11,
  ROCI_OPCODE_CALL_12,
  ROCI_OPCODE_CALL_13,
  ROCI_OPCODE_CALL_14,
  ROCI_OPCODE_CALL_15,
  ROCI_OPCODE_RETURN,

  ROCI_OPCODE_TRAP = 255,
};

typedef roci_bb_t = struct {
  uint32_t num_opcodes;
  uint32_t num_data;
  // opcodes + alignment nops
  // data
};

typedef roci_bb_builder_t = struct {
  uint64_t bblock_number;
  roci_bb_t* bblock;
  buffer_t* opcodes;
  value_array_t* data;
  uint64_t bb_next_true;
  uint64_t bb_next_false;
};

typedef roci_bb_builder_array_t = value_array_t;

roci_bb_builder_t* add_bblock(roci_bb_builder_array_t* bblocks) {
  roci_bb_builder_t* result = malloc_struct(roci_bb_builder_t);
  result->opcodes = make_buffer(10);
  result->data = make_value_array(1);
  value_array_add(bblocks, ptr_to_value(result));
  return result;
}

typedef roci_runtime_error_t = enum {
  ROCI_RUNTIME_ERROR_UNKNOWN,
  ROCI_RUNTIME_ERROR_BOOLEAN_REQUIRED,
};

void roci_runtime_error(roci_runtime_error_t runtime_error) {
  log_fatal("A runtime error has occurred evaluating a roci script");
  fatal_error(ERROR_ILLEGAL_STATE);
}
