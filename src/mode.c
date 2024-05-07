#line 2 "mode.c"
#ifndef _MODE_H_
#define _MODE_H_

/**
 * @enum input_mode_t
 *
 * Represents what kind of input we are currently lexing/parsing which
 * is generally determined automatically from the source file's
 * extension (".oc" -> INPUT_MODE_OMNI_C, ".c,.h" ->
 * INPUT_MODE_STANDARD_C, ".c++,.cpp,h++,hpp" ->
 * INPUT_MODE_C_PLUS_PLUS).
 *
 * Only when the input is Omni C are all the capabilities of Omni C
 * unlocked but Omni C is meant to be able to seamlessly interact with
 * both C and C++ code and libraries which is why we want to be able
 * to read their header files. Some features such as code generation
 * for enums and structs are available no matter what the input mode
 * is.
 */
typedef enum {
  INPUT_MODE_OMNI_C,
  INPUT_MODE_STANDARD_C,
  INPUT_MODE_C_PLUS_PLUS,
} input_mode_t;

/**
 * @enum output_mode_t
 *
 * Represents what kind of output we are generating when doing a
 * source to source translation for Omni C as well as what the output
 * should look like if only doing code generation for enums and
 * structs. We would like to see this list be pretty long (for example
 * Rust, Go, C#, or Java cross-compilation as well as eventually doing
 * native code generation).
 */
typedef enum {
  OUTPUT_MODE_STANDARD_C,
  OUTPUT_MODE_C_PLUS_PLUS,
} output_mode_t;

#endif /* _MODE_H_ */
