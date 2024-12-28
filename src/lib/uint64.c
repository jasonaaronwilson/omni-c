#line 2 "uint64.c"
/**
 * @file uint64.c
 *
 * Implement a couple of useful operations on uint64_t (which can
 * often be used for smaller types).
 */

#ifndef _UINT64_H_
#define _UINT64_H_

#include <stdint.h>

extern int uint64_highest_bit_set(uint64_t n);

#endif /* _UINT64_H_ */

/**
 * @function uint64_highest_bit_set
 *
 */
int uint64_highest_bit_set(uint64_t n) {
  if (n >= 1ULL << 32) {
    return uint64_highest_bit_set(n >> 32) + 32;
  } else if (n >= 1ULL << 16) {
    return uint64_highest_bit_set(n >> 16) + 16;
  } else if (n >= 1ULL << 8) {
    return uint64_highest_bit_set(n >> 8) + 8;
  } else if (n >= 1ULL << 4) {
    return uint64_highest_bit_set(n >> 4) + 4;
  } else if (n >= 1ULL << 2) {
    return uint64_highest_bit_set(n >> 2) + 2;
  } else if (n >= 1ULL << 1) {
    return uint64_highest_bit_set(n >> 1) + 1;
  } else {
    return 0;
  }
}
