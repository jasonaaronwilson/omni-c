#line 2 "utf8-decoder.c"
/**
 * @file utf8-decoder.c
 *
 * A very basic UTF-8 decoder.
 */
#ifndef _UTF8_DECODER_H_
#define _UTF8_DECODER_H_

#include <stdint.h>

/**
 * @struct utf8_decode_result_t
 *
 * Holds the result of utf8_decode.
 */
struct utf8_decode_result_S {
  uint32_t code_point;
  uint8_t num_bytes;
  boolean_t error;
};

typedef struct utf8_decode_result_S utf8_decode_result_t;

extern utf8_decode_result_t utf8_decode(const uint8_t* utf8_bytes);

#endif /* _UTF8_DECODER_H_ */

/**
 * @function utf8_decode
 *
 * Decodes the next code-point from a uint8_t* pointer.
 */
utf8_decode_result_t utf8_decode(const uint8_t* array) {
  uint8_t firstByte = array[0];
  if ((firstByte & 0x80) == 0) {
    return compound_literal(utf8_decode_result_t,
                            {.code_point = firstByte, .num_bytes = 1});
  } else if ((firstByte & 0xE0) == 0xC0) {
    return compound_literal(
        utf8_decode_result_t,
        {.code_point = ((firstByte & 0x1F) << 6) | (array[1] & 0x3F),
         .num_bytes = 2});
  } else if ((firstByte & 0xF0) == 0xE0) {
    return compound_literal(utf8_decode_result_t,
                            {.code_point = ((firstByte & 0x0F) << 12)
                                           | ((array[1] & 0x3F) << 6)
                                           | (array[2] & 0x3F),
                             .num_bytes = 3});
  } else if ((firstByte & 0xF8) == 0xF0) {
    return compound_literal(
        utf8_decode_result_t,
        {.code_point = ((firstByte & 0x07) << 18) | ((array[1] & 0x3F) << 12)
                       | ((array[2] & 0x3F) << 6) | (array[3] & 0x3F),
         .num_bytes = 4});
  } else {
    return compound_literal(utf8_decode_result_t, {.error = true});
  }
}
