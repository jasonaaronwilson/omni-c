#include <stdint.h>

typedef struct {
  uint64_t number;
  // Negative values mean an error occurred.
  int size;
} signed_decode_result;

/**
 * @function decode_sleb_128
 *
 * Decode a SLEB128 value (up to 64bits)
 */
signed_decode_result decode_sleb_128(const uint8_t* p, const uint8_t* end) {
  const uint8_t* orig_p = p;
  int64_t Value = 0;
  unsigned Shift = 0;
  uint8_t Byte;
  do {
    if (p == end) {
      signed_decode_result result = compound_literal(
          signed_decode_result, {0, ERROR_INSUFFICIENT_INPUT});
      return result;
    }
    Byte = *p;
    uint64_t Slice = Byte & 0x7f;
    // This handles decoding padded numbers, otherwise we might be
    // able to test very easily at the end of the loop.
    if ((Shift >= 64 && Slice != (Value < 0 ? 0x7f : 0x00))
        || (Shift == 63 && Slice != 0 && Slice != 0x7f)) {
      signed_decode_result result
          = compound_literal(signed_decode_result, {0, ERROR_TOO_BIG});
      return result;
    }
    Value |= Slice << Shift;
    Shift += 7;
    ++p;
  } while (Byte >= 128);
  // Sign extend negative numbers if needed.
  if (Shift < 64 && (Byte & 0x40)) {
    Value |= (-1ULL) << Shift;
  }
  signed_decode_result result
      = compound_literal(signed_decode_result, {Value, (p - orig_p)});
  return result;
}
