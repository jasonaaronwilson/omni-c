#line 2 "string-util.c"
/**
 * @file string-util.c
 *
 * This contains additional string function to operate on "strings"
 * since the C libary has only basic routines.
 */

// ======================================================================
// This is block is extraced to allocate.h
// ======================================================================

#ifndef _STRING_UTIL_H_
#define _STRING_UTIL_H_

#include <stdint.h>

extern int string_is_null_or_empty(const char* str1);
extern int string_equal(const char* str1, const char* str2);
extern int string_starts_with(const char* str1, const char* str2);
extern int string_ends_with(const char* str1, const char* str2);
extern boolean_t string_contains_char(const char* str, char ch);
extern int string_index_of_char(const char* a, char ch);
extern char* uint64_to_string(uint64_t number);
extern uint64_t string_hash(const char* str);
extern char* string_substring(const char* str, int start, int end);
extern value_result_t string_parse_uint64(const char* string);
extern value_result_t string_parse_uint64_dec(const char* string);
extern value_result_t string_parse_uint64_hex(const char* string);
extern value_result_t string_parse_uint64_bin(const char* string);
extern char* string_duplicate(const char* src);
extern char* string_append(const char* a, const char* b);
extern char* string_left_pad(const char* a, int count, char ch);
extern char* string_right_pad(const char* a, int count, char ch);
__attribute__((format(printf, 1, 2))) extern char* string_printf(char* format,
                                                                 ...);
char* string_truncate(char* str, int limit, char* at_limit_suffix);

#endif /* _STRING_UTIL_H_ */

// ======================================================================

#include <stdlib.h>
#include <string.h>

uint64_t fasthash64(const void* buf, size_t len, uint64_t seed);

/**
 * @function string_is_null_or_empty
 *
 * Return true if the given string is NULL or strlen is zero.
 */
int string_is_null_or_empty(const char* str) {
  return (str == NULL) || (strlen(str) == 0);
}

/**
 * @function string_equal
 *
 * Return true if two strings are equal.
 */
int string_equal(const char* str1, const char* str2) {
  if (string_is_null_or_empty(str1)) {
    return string_is_null_or_empty(str2);
  }
  return strcmp(str1, str2) == 0;
}

/**
 * @function string_starts_with
 *
 * Return true if str1 starts with all of str2.
 */
int string_starts_with(const char* str1, const char* str2) {
  return strncmp(str1, str2, strlen(str2)) == 0;
}

/**
 * @function string_ends_with
 *
 * Return true if str1 ends with all of str2.
 */
int string_ends_with(const char* str1, const char* str2) {
  size_t len1 = strlen(str1);
  size_t len2 = strlen(str2);

  if (len2 > len1) {
    return 0;
  }

  return strcmp(str1 + (len1 - len2), str2) == 0;
}

/**
 * @function string_contains_char
 *
 * Return true if str contains that given character ch.
 */
boolean_t string_contains_char(const char* str, char ch) {
  return string_index_of_char(str, ch) >= 0;
}

// TODO(jawilson): string_contains_code_point

// TODO(jawilson): string_index_of_string

/**
 * @function string_index_of_char
 *
 * Return the index of the given character in a string or a value less
 * than zero if the character isn't inside of the string.
 */
int string_index_of_char(const char* str, char ch) {
  if (string_is_null_or_empty(str)) {
    return -1;
  }
  int str_length = strlen(str);
  for (int i = 0; i < str_length; i++) {
    if (str[i] == ch) {
      return i;
    }
  }
  return -1;
}

/**
 * @function string_hash
 *
 * Return a fast but generally high-quality 64bit hash of an input
 * string.
 */
uint64_t string_hash(const char* str) {
  return fasthash64(str, strlen(str), 0);
}

/**
 * @function string_substring
 *
 * Return a substring of the given string as a newly allocated string.
 */
char* string_substring(const char* str, int start, int end) {
  uint64_t len = strlen(str);
  if (start >= len || start >= end || end < start) {
    fatal_error(ERROR_ILLEGAL_ARGUMENT);
  }
  int result_size = end - start + 1;
  char* result = cast(char*, malloc_bytes(result_size));
  for (int i = start; (i < end); i++) {
    result[i - start] = str[i];
  }
  result[result_size - 1] = '\0';
  return result;
}

value_result_t string_parse_uint64_dec(const char* string) {
  uint64_t len = strlen(string);
  uint64_t integer = 0;

  if (len == 0) {
    return compound_literal(
        value_result_t, {.u64 = 0, .nf_error = NF_ERROR_NOT_PARSED_AS_NUMBER});
  }

  for (int i = 0; i < len; i++) {
    char ch = string[i];
    if (ch < '0' || ch > '9') {
      return compound_literal(
          value_result_t,
          {.u64 = 0, .nf_error = NF_ERROR_NOT_PARSED_AS_NUMBER});
    }
    uint64_t digit = string[i] - '0';
    integer = integer * 10 + digit;
  }

  return compound_literal(value_result_t, {.u64 = integer, .nf_error = NF_OK});
}

/**
 * Parse a sequence of zeros and ones. A prefix like 0b should be
 * stripped before calling this routine and the string must only
 * contain the digits 0 and 1.
 */
value_result_t string_parse_uint64_bin(const char* string) {
  uint64_t len = strlen(string);
  uint64_t integer = 0;

  if (len == 0) {
    return compound_literal(
        value_result_t, {.u64 = 0, .nf_error = NF_ERROR_NOT_PARSED_AS_NUMBER});
  }

  for (int i = 0; i < len; i++) {
    char ch = string[i];
    if (ch < '0' || ch > '1') {
      return compound_literal(
          value_result_t,
          {.u64 = 0, .nf_error = NF_ERROR_NOT_PARSED_AS_NUMBER});
    }
    uint64_t digit = string[i] - '0';
    integer = integer << 1 | digit;
  }

  return compound_literal(value_result_t, {.u64 = integer, .nf_error = NF_OK});
}

static inline boolean_t is_hex_digit(char ch) {
  return (ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f');
}

static inline uint64_t hex_digit_to_value(char ch) {
  if (ch >= '0' && ch <= '9') {
    return ch - '0';
  } else {
    return (ch - 'a') + 10;
  }
}

/**
 * Parse a sequence of characters "0123456789abcdef" to an uint64_t. A
 * prefix like 0x should be striped before calling this routine.
 *
 */
value_result_t string_parse_uint64_hex(const char* string) {
  uint64_t len = strlen(string);
  uint64_t integer = 0;

  if (len == 0) {
    return compound_literal(
        value_result_t, {.u64 = 0, .nf_error = NF_ERROR_NOT_PARSED_AS_NUMBER});
  }

  for (int i = 0; i < len; i++) {
    char ch = string[i];
    if (!is_hex_digit(ch)) {
      return compound_literal(
          value_result_t,
          {.u64 = 0, .nf_error = NF_ERROR_NOT_PARSED_AS_NUMBER});
    }
    uint64_t digit = hex_digit_to_value(ch);
    integer = integer << 4 | digit;
  }

  return compound_literal(value_result_t, {.u64 = integer, .nf_error = NF_OK});
}

/**
 * @function string_parse_uint64
 *
 * Parse a string as a uint64_t.
 *
 * If the string begins with "0x", the the number should be a well
 * formed hexidecimal number (in all lower-case).
 *
 * If the string begins with "0b", the the number should be a well
 * formed binary number.
 *
 * Ortherwise the number should be a well-formed decimal number.
 *
 * While currently overflow is not detected, that is likely to be
 * detected in future versions of the library.
 */
value_result_t string_parse_uint64(const char* string) {
  if (string_starts_with(string, "0x")) {
    return string_parse_uint64_hex(&string[2]);
  } else if (string_starts_with(string, "0b")) {
    return string_parse_uint64_bin(&string[2]);
  } else {
    return string_parse_uint64_dec(string);
  }
}

/**
 * @function string_duplicate
 *
 * Just like strdup except NULL is a valid source argument and we use
 * malloc_bytes which checks the return result from malloc.
 */
char* string_duplicate(const char* src) {
  if (src == NULL) {
    return NULL;
  }
  int len = strlen(src) + 1;
  char* result = cast(char*, malloc_bytes(len));
  memcpy(result, src, len);

  return result;
}

/**
 * @function string_append
 *
 * Return a freshly allocated string that is the concatentation of the
 * two input strings (neither of which should be NULL);
 */
char* string_append(const char* a, const char* b) {
  if (a == NULL || b == NULL) {
    fatal_error(ERROR_ILLEGAL_NULL_ARGUMENT);
  }
  int total_length = strlen(a) + strlen(b) + 1;
  char* result = cast(char*, malloc_bytes(total_length));
  strcat(result, a);
  strcat(result, b);
  return result;
}

/**
 * @function uint64_to_string
 *
 * Convert a uint64_t number to a string.
 */
char* uint64_to_string(uint64_t number) {
  char buffer[32];
  sprintf(buffer, "%lu", number);
  return string_duplicate(buffer);
}

/**
 * @function string_left_pad
 *
 * Prepend left left padding (if necessary) to make it at least N
 * bytes long.
 */
char* string_left_pad(const char* str, int n, char ch) {
  if (n < 0) {
    fatal_error(ERROR_ILLEGAL_RANGE);
  }

  int input_length = strlen(str);

  // Calculate padding needed
  int padding_needed = n - input_length;

  // As usual, since buffer's grow as needed, we are tolerant of a
  // wrong initial computation of the length though getting this wrong
  // is wasteful... In this case we do the wasteful thing knowing that
  // we will free everything shortly. We just want the correct result,
  // not necessarily as fast as possible.

  int len = 1; // max(padding_needed + input_length, input_length) + 1;

  buffer_t* buffer = make_buffer(len);
  for (int i = 0; i < padding_needed; i++) {
    buffer = buffer_append_byte(buffer, ch);
  }
  buffer = buffer_append_string(buffer, str);
  char* result = buffer_to_c_string(buffer);
  free_bytes(buffer);
  return result;
}

/**
 * @function string_right_pad
 *
 * Append right padding to a string (if necessary) to make it at least
 * N bytes long.
 */
char* string_right_pad(const char* str, int n, char ch) {
  if (n < 0) {
    fatal_error(ERROR_ILLEGAL_RANGE);
  }

  int input_length = strlen(str);

  // Calculate padding needed
  int padding_needed = n - input_length;

  // As usual, since buffer's grow as needed, we are tolerant of a
  // wrong initial computation of the length though getting this wrong
  // is wasteful... In this case we do the wasteful thing knowing that
  // we will free everything shortly. We just want the correct result,
  // not necessarily as fast as possible.

  int len = 1; // max(padding_needed + input_length, input_length) + 1;

  buffer_t* buffer = make_buffer(len);
  buffer = buffer_append_string(buffer, str);
  for (int i = 0; i < padding_needed; i++) {
    buffer = buffer_append_byte(buffer, ch);
  }
  char* result = buffer_to_c_string(buffer);
  free_bytes(buffer);
  return result;
}

/**
 * @function string_truncate
 *
 * Return a copy of the string truncated to limit number of *bytes*
 * (excluding the trailing zero). This is currently not unicode safe!
 *
 * When the string is truncated, we also add 'at_limit_suffix' which
 * may make the returned string actually that many characters
 * longer. This behavior is likely to change in a future version.
 */
char* string_truncate(char* str, int limit, char* at_limit_suffix) {
  // limit is just a guess, buffer's always grow as needed.
  buffer_t* buffer = make_buffer(limit);
  for (int i = 0;; i++) {
    char ch = str[i];
    if (ch == '\0') {
      char* result = buffer_to_c_string(buffer);
      free_bytes(buffer);
      return result;
    }
    buffer = buffer_append_byte(buffer, ch);
  }
  if (at_limit_suffix) {
    buffer = buffer_append_string(buffer, at_limit_suffix);
  }
  char* result = buffer_to_c_string(buffer);
  free_bytes(buffer);
  return result;
}

// Allows tests to make the temporary buffer small to more easily test
// the case where vsnprintf is called twice because the first time it
// was called we didn't have a large enough buffer.
#ifndef STRING_PRINTF_INITIAL_BUFFER_SIZE
#define STRING_PRINTF_INITIAL_BUFFER_SIZE 1024
#endif /* STRING_PRINTF_INITIAL_BUFFER_SIZE */

/**
 * @function string_printf
 *
 * Perform printf to a buffer and return the result as a dynamically
 * allocated string. The string is automatically allocated to the
 * appropriate size.
 */
__attribute__((format(printf, 1, 2))) char* string_printf(char* format, ...) {
  char buffer[STRING_PRINTF_INITIAL_BUFFER_SIZE];
  int n_bytes = 0;
  do {
    va_list args;
    va_start(args, format);
    n_bytes
        = vsnprintf(buffer, STRING_PRINTF_INITIAL_BUFFER_SIZE, format, args);
    va_end(args);
  } while (0);

  if (n_bytes < STRING_PRINTF_INITIAL_BUFFER_SIZE) {
    char* result = cast(char*, malloc_bytes(n_bytes + 1));
    strcat(result, buffer);
    return result;
  } else {
    char* result = cast(char*, malloc_bytes(n_bytes + 1));
    va_list args;
    va_start(args, format);
    int n_bytes_second = vsnprintf(result, n_bytes + 1, format, args);
    va_end(args);
    if (n_bytes_second != n_bytes) {
      fatal_error(ERROR_INTERNAL_ASSERTION_FAILURE);
    }
    return result;
  }
}

/* ================================================================ */

/* The MIT License

   Copyright (C) 2012 Zilong Tan (eric.zltan@gmail.com)

   Permission is hereby granted, free of charge, to any person
   obtaining a copy of this software and associated documentation
   files (the "Software"), to deal in the Software without
   restriction, including without limitation the rights to use, copy,
   modify, merge, publish, distribute, sublicense, and/or sell copies
   of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be
   included in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
   BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
   ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
   CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
*/

// #include "fasthash.h"

// Compression function for Merkle-Damgard construction.
// This function is generated using the framework provided.
static inline uint64_t mix(uint64_t h) {
  h ^= h >> 23;
  h *= 0x2127599bf4325c37ULL;
  h ^= h >> 47;
  return h;
}

// security: if the system allows empty keys (len=3) the seed is exposed, the
// reverse of mix. objsize: 0-1fd: 509
uint64_t fasthash64(const void* buf, size_t len, uint64_t seed) {
  const uint64_t m = 0x880355f21e6d1965ULL;
  const uint64_t* pos = cast(const uint64_t*, buf);
  const uint64_t* end = pos + (len / 8);
  const unsigned char* pos2;
  uint64_t h = seed ^ (len * m);
  uint64_t v;

  while (pos != end) {
    v = *pos++;
    h ^= mix(v);
    h *= m;
  }

  pos2 = cast(const unsigned char*, pos);
  v = 0;

  switch (len & 7) {
  case 7:
    v ^= cast(uint64_t, pos2[6]) << 48;
  case 6:
    v ^= cast(uint64_t, pos2[5]) << 40;
  case 5:
    v ^= cast(uint64_t, pos2[4]) << 32;
  case 4:
    v ^= cast(uint64_t, pos2[3]) << 24;
  case 3:
    v ^= cast(uint64_t, pos2[2]) << 16;
  case 2:
    v ^= cast(uint64_t, pos2[1]) << 8;
  case 1:
    v ^= cast(uint64_t, pos2[0]);
    h ^= mix(v);
    h *= m;
  }

  return mix(h);
}
