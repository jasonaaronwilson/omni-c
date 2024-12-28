#line 2 "buffer.c"
#ifndef _BUFFER_H_
#define _BUFFER_H_

/**
 * @file buffer.c
 *
 * A growable array of bytes.
 *
 * Buffers are useful for constructing UTF-8 strings (for example by
 * using buffer_printf) though they can hold any binary data including
 * interior NUL bytes. Buffers automatically grow as data is appended
 * (or inserted into) them reducing large classes of errors.
 */

#include <ctype.h>
#include <stdint.h>
#include <string.h>

struct buffer_S {
  uint32_t length;
  uint32_t capacity;
  uint8_t* elements;
};

/**
 * @struct buffer_t
 *
 * Buffers are non-thread-safe memory regions that generally "grow"
 * over time. Whenever a buffer grows beyound it's capacity, it is
 * moved to accomodates its new capacity (even if there may be other
 * pointers to it's insides, so don't do that unless you have decided
 * the buffer will never grow again).
 *
 * While buffers may seem scary, used properly from a single thread,
 * they are actually quite predictable.
 */
typedef struct buffer_S buffer_t;

extern buffer_t* make_buffer(uint64_t initial_capacity);

extern uint64_t buffer_length(buffer_t* buffer);

extern uint8_t buffer_get(buffer_t* buffer, uint64_t position);

extern char* buffer_c_substring(buffer_t* buffer, uint64_t start, uint64_t end);

extern char* buffer_to_c_string(buffer_t* buffer);

extern void buffer_clear(buffer_t* buffer);

extern buffer_t* buffer_increase_capacity(buffer_t* buffer, uint64_t capacity);

extern buffer_t* buffer_append_byte(buffer_t* buffer, uint8_t byte);

extern buffer_t* buffer_append_bytes(buffer_t* buffer, uint8_t* bytes,
                                     uint64_t n_bytes);

extern buffer_t* buffer_append_buffer(buffer_t* buffer, buffer_t* src_buffer);

extern buffer_t* buffer_append_sub_buffer(buffer_t* buffer,
                                          uint64_t start_position,
                                          uint64_t end_position,
                                          buffer_t* src_buffer);

extern buffer_t* buffer_append_string(buffer_t* buffer, const char* str);

__attribute__((format(printf, 2, 3))) extern buffer_t*
    buffer_printf(buffer_t* buffer, char* format, ...);

extern buffer_t* buffer_append_repeated_byte(buffer_t* buffer, uint8_t byte,
                                             int count);

utf8_decode_result_t buffer_utf8_decode(buffer_t* buffer, uint64_t position);

extern buffer_t* buffer_append_code_point(buffer_t* buffer,
                                          uint32_t code_point);

boolean_t buffer_match_string_at(buffer_t* buffer, uint64_t start_position,
                                 char* str);

buffer_t* buffer_from_string(char* string);

buffer_t* buffer_adjust_region(buffer_t* buffer, uint64_t original_start,
                               uint64_t original_end, uint64_t new_width);

buffer_t* buffer_replace_all(buffer_t* buffer, char* original_text,
                             char* replacement_text);

buffer_t* buffer_replace_matching_byte(buffer_t* buffer, uint8_t original,
                                       uint8_t replacement);

boolean_t buffer_region_contains(buffer_t* buffer, uint64_t start, uint64_t end,
                                 char* text);

uint64_t buffer_beginning_of_line(buffer_t* buffer, uint64_t start);

uint64_t buffer_end_of_line(buffer_t* buffer, uint64_t start);

buffer_t* buffer_to_uppercase(buffer_t* buffer);

buffer_t* buffer_to_lowercase(buffer_t* buffer);

typedef struct line_and_column_S {
  uint64_t line;
  uint64_t column;
} line_and_column_t;

line_and_column_t buffer_position_to_line_and_column(buffer_t* buffer,
                                                     uint64_t position);

#endif /* _BUFFER_H_ */

// ======================================================================

#include <stdlib.h>

/**
 * @function make_buffer
 *
 * Make an empty byte array with the given initial capacity.
 */
buffer_t* make_buffer(uint64_t initial_capacity) {
  buffer_t* result = malloc_struct(buffer_t);
  if (initial_capacity < 16) {
    initial_capacity = 16;
  }
  if (initial_capacity > 0) {
    result->capacity = initial_capacity;
    result->elements = malloc_bytes(initial_capacity);
  }
  return result;
}

/**
 * @function buffer_length
 *
 * Return the number of bytes that have been added to this byte array.
 */
uint64_t buffer_length(buffer_t* array) { return array->length; }

/**
 * @function buffer_length
 *
 * Clear a buffer without resizing it.
 */
void buffer_clear(buffer_t* buffer) {
  for (int i = 0; i < buffer->capacity; i++) {
    buffer->elements[i] = 0;
  }
  buffer->length = 0;
}

/**
 * @function buffer_get
 *
 * Get a single byte from a byte array.
 */
uint8_t buffer_get(buffer_t* buffer, uint64_t position) {
  if (position < buffer->length) {
    return buffer->elements[position];
  } else {
    fatal_error(ERROR_ACCESS_OUT_OF_BOUNDS);
#ifdef __TINYC__
    /* gcc and clang know fatal_error is _Noreturn but tcc doesn't */
    return 0;
#endif
  }
}

/**
 * @function buffer_c_substring
 *
 * Extract a newly allocated string that contains the bytes from start
 * to end (appending a zero byte to make sure it's a legal C string).
 */
char* buffer_c_substring(buffer_t* buffer, uint64_t start, uint64_t end) {
  if (buffer == NULL) {
    fatal_error(ERROR_ILLEGAL_NULL_ARGUMENT);
  }

  if (start > end) {
    fatal_error(ERROR_ILLEGAL_RANGE);
  }

  uint64_t copy_length = (end - start);
  char* result = cast(char*, malloc_bytes(copy_length + 1));
  if (copy_length > 0) {
    memcpy(result, &buffer->elements[start], copy_length);
  }
  result[copy_length] = '\0';
  return result;
}

/**
 * @function buffer_to_c_string
 *
 * Extract a newly allocated string that contain all of the bytes in the byte
 * buffer as a NUL (zero byte) terminated C string.
 */
char* buffer_to_c_string(buffer_t* buffer) {
  return buffer_c_substring(buffer, 0, buffer->length);
}

/**
 * @function buffer_append_byte
 *
 * Append a single byte to the byte array.
 */
buffer_t* buffer_append_byte(buffer_t* buffer, uint8_t element) {
  if (buffer->length < buffer->capacity) {
    buffer->elements[buffer->length] = element;
    buffer->length++;
    return buffer;
  }
  buffer = buffer_increase_capacity(buffer, buffer->capacity * 2);
  return buffer_append_byte(buffer, element);
}

/**
 * @function buffer_append_bytes
 *
 * Append multiple bytes to the byte array.
 */
buffer_t* buffer_append_bytes(buffer_t* buffer, uint8_t* bytes,
                              uint64_t n_bytes) {
  // Obviously this can be optimized...
  for (int i = 0; i < n_bytes; i++) {
    buffer = buffer_append_byte(buffer, bytes[i]);
  }
  return buffer;
}

/**
 * @function buffer_append_string
 *
 * Append all of the bytes from a C string (except the ending NUL
 * char).
 */
buffer_t* buffer_append_string(buffer_t* buffer, const char* str) {
  return buffer_append_bytes(buffer, cast(uint8_t*, str), strlen(str));
}

/**
 * @function buffer_increase_capacity
 *
 * Allow the capacity of a buffer to be increased (or return if the
 * buffer already exceeds the given capacity). Generally this is not
 * necessary to properly use a buffer (and occasionally could actually
 * hurt performance if done incorrectly).
 */
extern buffer_t* buffer_increase_capacity(buffer_t* buffer, uint64_t capacity) {
  if (buffer->capacity < capacity) {
    uint8_t* new_elements = malloc_bytes(capacity);
    memcpy(new_elements, buffer->elements, buffer->length);
    free_bytes(buffer->elements);
    buffer->elements = new_elements;
    buffer->capacity = capacity;
  }
  return buffer;
}

#ifndef BUFFER_PRINTF_INITIAL_BUFFER_SIZE
#define BUFFER_PRINTF_INITIAL_BUFFER_SIZE 1024
#endif

/**
 * @function buffer_printf
 *
 * Format like printf but append the result to the passed in buffer
 * (returning a new buffer in case the buffer exceeded it's capacity).
 */
__attribute__((format(printf, 2, 3))) buffer_t*
    buffer_printf(buffer_t* buffer, char* format, ...) {
  char cbuffer[BUFFER_PRINTF_INITIAL_BUFFER_SIZE];
  int n_bytes = 0;
  do {
    va_list args;
    va_start(args, format);
    n_bytes
        = vsnprintf(cbuffer, BUFFER_PRINTF_INITIAL_BUFFER_SIZE, format, args);
    va_end(args);
  } while (0);

  if (n_bytes < BUFFER_PRINTF_INITIAL_BUFFER_SIZE) {
    return buffer_append_string(buffer, cbuffer);
  } else {
    // Be lazy for now and just copy the code from string_printf for
    // this case but we should be able to do ensure capacity and just
    // put the bytes directly at the end of the buffer...
    char* result = cast(char*, malloc_bytes(n_bytes + 1));
    va_list args;
    va_start(args, format);
    int n_bytes_second = vsnprintf(result, n_bytes + 1, format, args);
    va_end(args);
    if (n_bytes_second != n_bytes) {
      fatal_error(ERROR_INTERNAL_ASSERTION_FAILURE);
    }
    buffer = buffer_append_string(buffer, result);
    free_bytes(result);
    return buffer;
  }
}

/**
 * @function buffer_append_repeated_byte
 *
 * Append 'count' copies of byte to the passed in buffer. This can be
 * used for things like indentation or horizontal rules (composed from
 * say '-', '=', or '*').
 */
extern buffer_t* buffer_append_repeated_byte(buffer_t* buffer, uint8_t byte,
                                             int count) {
  for (int i = 0; i < count; i++) {
    buffer = buffer_append_byte(buffer, byte);
  }
  return buffer;
}

/**
 * @function buffer_utf8_decode
 *
 * Similar to utf8_decode but operating on a buffer_t. (This routine
 * adds bounds checking beyound utf8_decode).
 */
utf8_decode_result_t buffer_utf8_decode(buffer_t* buffer, uint64_t position) {
  if (position >= buffer->length) {
    return compound_literal(utf8_decode_result_t, {.error = true});
  }
  utf8_decode_result_t result = utf8_decode(&buffer->elements[position]);
  if (result.error) {
    return result;
  }
  if ((position + result.num_bytes) > buffer->length) {
    return compound_literal(utf8_decode_result_t, {.error = true});
  }
  return result;
}

/**
 * @function buffer_append_code_point()
 *
 * Append a single code-point according to UTF-8 encoding (so 1 to 4
 * bytes). While you can put anything you want into a buffer_t (not
 * just valid UTF-8 sequences), if you then try to make a C string
 * from the buffer then it might end up with a NUL ('\0') byte in the
 * middle of it if you add code_point == 0 somewhere besides the end
 * of the string.
 *
 * @see utf8_decode(const uint8_t* utf8_bytes).
 */
extern buffer_t* buffer_append_code_point(buffer_t* buffer,
                                          uint32_t code_point) {
  if (code_point < 0x80) {
    // 1-byte sequence for code points in the range 0-127
    buffer = buffer_append_byte(buffer, code_point);
    return buffer;
  } else if (code_point < 0x800) {
    // 2-byte sequence for code points in the range 128-2047
    buffer = buffer_append_byte(buffer, 0xc0 | (code_point >> 6));
    buffer = buffer_append_byte(buffer, 0x80 | (code_point & 0x3f));
    return buffer;
  } else if (code_point < 0x10000) {
    // 3-byte sequence for code points in the range 2048-65535
    buffer = buffer_append_byte(buffer, 0xe0 | (code_point >> 12));
    buffer = buffer_append_byte(buffer, 0x80 | ((code_point >> 6) & 0x3f));
    buffer = buffer_append_byte(buffer, 0x80 | (code_point & 0x3f));
    return buffer;
  } else if (code_point <= 0x10FFFF) {
    // 4-byte sequence for code points in the range 65536-1114111
    buffer = buffer_append_byte(buffer, 0xf0 | (code_point >> 18));
    buffer = buffer_append_byte(buffer, 0x80 | ((code_point >> 12) & 0x3f));
    buffer = buffer_append_byte(buffer, 0x80 | ((code_point >> 6) & 0x3f));
    buffer = buffer_append_byte(buffer, 0x80 | (code_point & 0x3f));
    return buffer;
  } else {
    // Code points beyond the valid UTF-8 range (0-0x10FFFF) are not supported
    fatal_error(ERROR_ILLEGAL_UTF_8_CODE_POINT);
    return 0; // Not Reached.
  }
}
/**
 * @function buffer_match_string_at
 *
 * Determine if the buffer contains "str" at start_position.
 */
boolean_t buffer_match_string_at(buffer_t* buffer, uint64_t start_position,
                                 char* str) {
  for (uint64_t pos = start_position; true; pos++) {
    uint8_t byte_str = cast(uint8_t*, str)[pos - start_position];
    if (byte_str == 0) {
      return true;
    }
    if (pos >= buffer_length(buffer)) {
      return false;
    }
    uint8_t byte_buf = buffer_get(buffer, pos);
    if (byte_str != byte_buf) {
      return false;
    }
  }
  /* NOT REACHED */
  return false;
}

/**
 * @function buffer_from_string
 *
 * Return a buffer initialized with the contents of a particular
 * string. The trailing NUL byte C string terminator byte is not
 * included in the buffer.
 */
buffer_t* buffer_from_string(char* string) {
  buffer_t* result = make_buffer(strlen(string));
  result = buffer_append_string(result, string);
  return result;
}

/**
 * @function buffer_adjust_region
 *
 * This is primarily used as a helper for buffer_replace_all though it
 * can also be used to efficiently delete a region or simply "open up"
 * space within a buffer.
 */
buffer_t* buffer_adjust_region(buffer_t* buffer, uint64_t start, uint64_t end,
                               uint64_t new_width) {
  // TODO(jawilson): more range testing.
  if (start > end) {
    fatal_error(ERROR_ILLEGAL_RANGE);
  }
  uint64_t original_width = end - start;
  if (original_width > new_width) {
    // Copy the tail of the buffer downwards and then decrease the
    // length
    uint64_t difference = original_width - new_width;
    uint64_t tail_length = buffer->length - end;
    memmove(&buffer->elements[end - difference], &buffer->elements[end],
            tail_length);
    buffer->length -= difference;
  } else if (original_width < new_width) {
    // Increase the capacity of the buffer if necessary and then copy
    // the tail of the buffer upwards and finally increase the length
    uint64_t difference = new_width - original_width;
    uint64_t tail_length = buffer->length - end;
    buffer = buffer_increase_capacity(buffer, buffer->length + difference);
    memmove(&buffer->elements[end + difference], &buffer->elements[end],
            tail_length);
    buffer->length += difference;
  }
  return buffer;
}

/**
 * @function buffer_replace_all
 *
 * Find all occurences of original_text and replace them with
 * replacement_text.
 */
buffer_t* buffer_replace_all(buffer_t* buffer, char* original_text,
                             char* replacement_text) {
  int len_original = strlen(original_text);
  int len_replacement = strlen(replacement_text);
  if (len_original < buffer->length) {
    uint64_t pos = 0;
    while (pos <= (buffer->length - len_original)) {
      if (buffer_match_string_at(buffer, pos, original_text)) {
        buffer = buffer_adjust_region(buffer, pos, pos + len_original,
                                      len_replacement);
        memmove(&buffer->elements[pos], replacement_text, len_replacement);
        pos += len_replacement;
      } else {
        pos++;
      }
    }
  }
  return buffer;
}

line_and_column_t buffer_position_to_line_and_column(buffer_t* buffer,
                                                     uint64_t position) {
  uint64_t line = 1;
  uint64_t column = 1;

  // TODO(jawilson): process as code points
  // TODO(jawilson): write unit test
  for (uint64_t pos = 0; pos < position; pos++) {
    uint8_t ch = buffer_get(buffer, pos);
    if (ch == '\n') {
      line++;
      column = 1;
    } else {
      column++;
    }
  }
  return compound_literal(line_and_column_t, {
                                                 .line = line,
                                                 .column = column,
                                             });
}

/**
 * @function buffer_region_contains
 *
 * Determine if a buffer contains the specified text within a region.
 */
boolean_t buffer_region_contains(buffer_t* buffer, uint64_t start, uint64_t end,
                                 char* text) {
  for (int i = start; i < end; i++) {
    if (buffer_match_string_at(buffer, i, text)) {
      return true;
    }
  }
  return false;
}

/**
 * @function buffer_replace_matching_byte
 *
 * Replace all occurences of the original byte value with the
 * replacement value. This is useful for example to turn NUL bytes
 * into something more useful like a space character so the buffer can
 * be turned into a C string without getting truncated.
 */
buffer_t* buffer_replace_matching_byte(buffer_t* buffer, uint8_t original,
                                       uint8_t replacement) {
  for (int i = 0; i < buffer->length; i++) {
    if (buffer->elements[i] == original) {
      buffer->elements[i] = replacement;
    }
  }
  return buffer;
}

/**
 * @function buffer_beginning_of_line
 *
 * Look backwards starting at the start position until we reach a
 * position where the previous character is a newline or the beginning
 * of the buffer.
 */
uint64_t buffer_beginning_of_line(buffer_t* buffer, uint64_t start) {
  uint64_t position = start;
  while (position > 0) {
    position--;
    if (buffer_get(buffer, position) == '\n') {
      return position + 1;
    }
  }
  return position;
}

/**
 * @function buffer_end_of_line
 *
 * Look forwards starting at the start position until we reach the
 * position of the first newline. If we reach the end of the buffer
 * without encountering a newline, simply return the length of the
 * buffer.
 */
uint64_t buffer_end_of_line(buffer_t* buffer, uint64_t start) {
  uint64_t position = start;
  while (position < buffer->length && buffer_get(buffer, position) != '\n') {
    position++;
  }
  return position;
}

/**
 * @function buffer_append_sub_buffer
 *
 * Append all of the bytes of the src_buffer to a buffer.
 *
 * It is currently illegal to append parts of a buffer to itself.
 */
extern buffer_t* buffer_append_buffer(buffer_t* buffer, buffer_t* src_buffer) {
  return buffer_append_sub_buffer(buffer, 0, src_buffer->length, src_buffer);
}

/**
 * @function buffer_append_sub_buffer
 *
 * Append all of the bytes between src_buffer[start_position,
 * end_position) to the dst_buffer. It is illegal to append parts of a
 * buffer to itself because of how buffer's are implemented which we
 * are considering changing for this and other reasons.
 */
extern buffer_t* buffer_append_sub_buffer(buffer_t* buffer,
                                          uint64_t start_position,
                                          uint64_t end_position,
                                          buffer_t* src_buffer) {
  if (buffer == src_buffer) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  for (uint64_t position = start_position; position < end_position;
       position++) {
    buffer = buffer_append_byte(buffer, buffer_get(src_buffer, position));
  }
  return buffer;
}

/**
 * @function buffer_to_uppercase
 *
 * Call toupper on each character in the buffer.
 *
 * TODO(jawilson): make sure this process full unicode even if it can
 * only uppercase ASCII latin characters.
 */

buffer_t* buffer_to_uppercase(buffer_t* buffer) {
  for (uint64_t i = 0; i < buffer->length; i++) {
    buffer->elements[i] = toupper(buffer->elements[i]);
  }
  return buffer;
}

/**
 * @function buffer_to_lowercase
 *
 * Call tolower on each character in the buffer.
 *
 * TODO(jawilson): make sure this process full unicode even if it can
 * only uppercase ASCII latin characters.
 */

buffer_t* buffer_to_lowercase(buffer_t* buffer) {
  for (uint64_t i = 0; i < buffer->length; i++) {
    buffer->elements[i] = tolower(buffer->elements[i]);
  }
  return buffer;
}
