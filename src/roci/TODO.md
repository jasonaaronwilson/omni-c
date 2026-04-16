# Roci TODO

1. properly handle quoted strings and symbols

2. detect bblocks that don't end in an absolute branch

3. implement tagged environments

4. implement these opcodes

  ROCI_OPCODE_NEW_ENVIRONMENT
  ROCI_OPCODE_DEFINE_VAR
  ROCI_OPCODE_GET_VAR
  ROCI_OPCODE_SET_VAR
  ROCI_OPCODE_CALL
  ROCI_OPCODE_RETURN

5. implement a simple roci parser (mostly calls top-level assignments)

6. enqueue additional bblocks for disassembly.
