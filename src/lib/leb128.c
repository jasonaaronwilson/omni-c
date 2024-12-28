#line 2 "leb128.c"

#ifndef _LEB128_H_
#define _LEB128_H_

/**
 * @file leb128.c
 *
 * ULEB-128 (unsigned) and SLEB-128 (signed, aka, possibly negative
 * integers) are variable length encodings for possibly "very large"
 * numbers. I first came across LEB while working with "DWARF2" in in
 * the 1990s. Google uses the same encoding for unsigned integers but
 * uses "zig-zag" for signed numbers (the sign bit is stored in the
 * lowest bit instead of the more complicated technique SLEB-128
 * uses).
 *
 * Essentially the 8th bit of each byte says whether to continue
 * adding bits from the next byte or if the number is now "complete"
 * (so 7-bits per byte are the real data and 1-bit is
 * "overhead"). Since many numbers we deal with are actually small,
 * LEB encoding tend to be an efficient encoding of integers and grow
 * "gracefully" to accomodate larger numbers. In fact the "128" in
 * ULEB-128 is a misnomer -- you could in theory encode more than 128
 * bits. This implementation only reads 64bit numbers, not the full
 * 128 bits.
 *
 * This implementation originally comes from LLVM although I have
 * changed it to a C file, renamed the functions, got rid of camel
 * case, removed the C++ namespace, removed the inline directive,
 * removed the pad to argument and changed how results and errors are
 * returned.
 */

#include <stdint.h>

#define ERROR_INSUFFICIENT_INPUT -1
#define ERROR_TOO_BIG -2

typedef struct {
  uint64_t number;
  // Negative values mean an error occurred.
  int size;
} unsigned_decode_result;

typedef struct {
  uint64_t number;
  // Negative values mean an error occurred.
  int size;
} signed_decode_result;

extern unsigned encode_sleb_128(int64_t Value, uint8_t* p);
extern unsigned encode_uleb_128(uint64_t Value, uint8_t* p);
extern unsigned_decode_result decode_uleb_128(const uint8_t* p,
                                              const uint8_t* end);
extern signed_decode_result decode_sleb_128(const uint8_t* p,
                                            const uint8_t* end);

#endif /* _LEB128_H_ */

//===- llvm/Support/LEB128.h - [SU]LEB128 utility functions -----*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file declares some utility functions for encoding SLEB128 and
// ULEB128 values.
//
//===----------------------------------------------------------------------===//

/**
 * @function encode_sleb_128
 *
 * Utility function to encode a SLEB128 value to a raw byte
 * buffer. Returns the length in bytes of the encoded value. 10 bytes
 * should be enough to hold the 64bit number after encoding.
 */
unsigned encode_sleb_128(int64_t Value, uint8_t* p) {
  uint8_t* orig_p = p;
  int More;
  do {
    uint8_t Byte = Value & 0x7f;
    // NOTE: this assumes that this signed shift is an arithmetic right shift.
    Value >>= 7;
    More = !((((Value == 0) && ((Byte & 0x40) == 0))
              || ((Value == -1) && ((Byte & 0x40) != 0))));
    if (More)
      Byte |= 0x80; // Mark this byte to show that more bytes will follow.
    *p++ = Byte;
  } while (More);

  return cast(unsigned, p - orig_p);
}

/**
 * @function
 *
 * Utility function to encode a ULEB128 value to a raw byte
 * buffer. Returns the length in bytes of the encoded value. 10 bytes
 * should be enough to hold the 64bit number after encoding.
 */
unsigned encode_uleb_128(uint64_t Value, uint8_t* p) {
  uint8_t* orig_p = p;
  do {
    uint8_t Byte = Value & 0x7f;
    Value >>= 7;
    if (Value != 0)
      Byte |= 0x80; // Mark this byte to show that more bytes will follow.
    *p++ = Byte;
  } while (Value != 0);

  return cast(unsigned, (p - orig_p));
}

/**
 * @function decode_uleb_128
 *
 * Decode a ULEB-128 value (up to 64bits).
 */
unsigned_decode_result decode_uleb_128(const uint8_t* p, const uint8_t* end) {
  const uint8_t* orig_p = p;
  uint64_t Value = 0;
  unsigned Shift = 0;
  do {
    if (p == end) {
      unsigned_decode_result result = compound_literal(
          unsigned_decode_result, {0, ERROR_INSUFFICIENT_INPUT});
      return result;
    }
    uint64_t Slice = *p & 0x7f;
    if ((Shift >= 64 && Slice != 0) || Slice << Shift >> Shift != Slice) {
      unsigned_decode_result result
          = compound_literal(unsigned_decode_result, {0, ERROR_TOO_BIG});
      return result;
    }
    Value += Slice << Shift;
    Shift += 7;
  } while (*p++ >= 128);
  unsigned_decode_result result = compound_literal(
      unsigned_decode_result, {Value, cast(unsigned, p - orig_p)});
  return result;
}

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
  if (Shift < 64 && (Byte & 0x40))
    Value |= (-1ULL) << Shift;
  signed_decode_result result
      = compound_literal(signed_decode_result, {Value, (p - orig_p)});
  return result;
}
