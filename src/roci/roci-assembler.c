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
 * C. Line comments begin with a semi-colon an continue until "\n".
 *
 * ; Simple function implementing "return my_func(0x12345, 3.14, "Hello world!")"
 * 0:
 *        push 0x12345
 *        push 3.14
 *        push "Hello world!"
 *        push 'my_func'
 *        push 3
 *        call 1
 * 1:
 *        return
 */
