/*
 * A mini-library for emitting roci VM instructions.
 */
void roci_emit_opcode(roci_compiler_state_t* state, roci_opcode_t opcode) {
  buffer_append_byte(state->current_bb->opcodes, opcode);
}

void roci_emit_token_string_datum(roci_compiler_state_t* state, char* str) {
  value_array_add(state->current_bb->data, str_to_value(str));
}

void roci_emit_int_datum(roci_compiler_state_t* state, uint64_t val) {
  value_array_add(state->current_bb->data, u64_to_value(val));
}

void roci_emit_new_environment(roci_compiler_state_t* state) {
  state->env_depth++;
  roci_emit_opcode(state, ROCI_OPCODE_NEW_ENVIRONMENT);
}

void roci_emit_drop_environment(roci_compiler_state_t* state) {
  state->env_depth--;
  roci_emit_opcode(state, ROCI_OPCODE_DROP_ENVIRONMENT);
}

void roci_emit_return(roci_compiler_state_t* state) {
  roci_emit_opcode(state, ROCI_OPCODE_RETURN);
}

void roci_emit_get_var(roci_bb_builder_t* bb, char* fn_name) {
  buffer_append_byte(bb->opcodes, ROCI_OPCODE_GET_VAR);
  value_array_add(bb->data, ptr_to_value(fn_name));
}

void roci_emit_comment(roci_bb_builder_t* bb, char* str) {
  buffer_append_byte(bb->opcodes, ROCI_OPCODE_COMMENT);
  value_array_add(bb->data, ptr_to_value(str));
}

void roci_emit_debug_info(roci_compiler_state_t* state, token_t* token) {
  roci_emit_opcode(state, ROCI_OPCODE_DEBUG_INFO);
  roci_emit_int_datum(state,
                      token_to_roci_src_info(state->buffer_number, token));
}

/**
 * @function roci_new_bblock
 *
 * Return a new roci_bb_builder_t*.
 *
 * The order of bblocks is technically not relevant but effort has
 * been made above, for example compile_if, so that the disassembly
 * looks like "real" assembly language will confuse us humans less. The
 * person writing this is definitely not AI.
 */
roci_bb_builder_t* roci_new_bblock(roci_compiler_state_t* state,
                                   char* label_prefix) {
  if (label_prefix == nullptr) {
    label_prefix = "bb_";
  }
  roci_bb_builder_t* result = add_bblock(state->bblocks);
  result->bblock_label
      = string_printf("%s%d", label_prefix, state->bb_label_count++);
  roci_emit_comment(result, result->bblock_label);
  return result;
}

void roci_emit_branch(roci_bb_builder_t* src_bblock,
                      roci_bb_builder_t* tgt_bblock) {
  buffer_append_byte(src_bblock->opcodes, ROCI_OPCODE_BR);
  value_array_add(src_bblock->data, ptr_to_value(tgt_bblock->bblock_label));
}

void roci_emit_br_true(roci_bb_builder_t* src_bblock,
                       roci_bb_builder_t* tgt_bblock) {
  buffer_append_byte(src_bblock->opcodes, ROCI_OPCODE_BR_TRUE);
  value_array_add(src_bblock->data, ptr_to_value(tgt_bblock->bblock_label));
}

void roci_verify_identifier(roci_compiler_state_t* state, token_t* token) {
  if (token->type != TOKEN_TYPE_IDENTIFIER) {
    roci_compiler_error(state, ROCI_COMPILE_TIME_ERROR);
  }
  // Make sure our unique keywords don't match either.
}
