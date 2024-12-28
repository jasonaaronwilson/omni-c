#line 2 "tokenizer.c"
/**
 * @file tokenizer.c
 *
 */

#ifndef _TOKENIZER_H_
#define _TOKENIZER_H_

extern value_array_t* string_tokenize(const char* str, const char* delimiters);
extern value_array_t* buffer_tokenize(buffer_t* buffer, const char* delimiters);
extern value_array_t* tokenize_memory_range(uint8_t* start, uint64_t length,
                                            const char* delimiters);

// TODO(jawilson):

#endif /* _TOKENIZER_H_ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void add_duplicate(value_array_t* token_array, const char* data);

/**
 * @function string_tokenize
 *
 * Tokenize a string into a a value_array_t of (non-empty) strings.
 *
 * Delimiters terminate the current token and are thrown away. The
 * delimiters string is treated as a sequence of delimiter characters,
 * it does not mean a delimiter can be multiple characters.
 */
value_array_t* string_tokenize(const char* str, const char* delimiters) {
  return tokenize_memory_range(cast(uint8_t*, str), strlen(str), delimiters);
}

/**
 * @function buffer_tokenize
 *
 * Tokenize the current contents of a buffer into a value_array_t of
 * (non-empty) strings. The input buffer should contain a valid UTF-8
 * encoded string.
 *
 * NUL bytes inside the buffer are automatically treated as an
 * additional delimiter.
 */
value_array_t* buffer_tokenize(buffer_t* buffer, const char* delimiters) {
  return tokenize_memory_range(&(buffer->elements[0]), buffer->length,
                               delimiters);
}

/**
 * @function tokenize_memory_range
 *
 * Tokenize a memory range into a value_array_t of (non-empty) strings. The
 * range should contain a valid UTF-8 encoded string.
 *
 * NUL bytes are automatically treated as an additional delimiter.
 */
value_array_t* tokenize_memory_range(uint8_t* str, uint64_t length,
                                     const char* delimiters) {
  value_array_t* result = make_value_array(1);
  char token_data[1024];
  int cpos = 0;
  for (int i = 0; (i < length); i++) {
    uint8_t ch = str[i];
    if (ch == 0 || string_contains_char(delimiters, ch)) {
      token_data[cpos++] = '\0';
      if (strlen(token_data) > 0) {
        add_duplicate(result, token_data);
      }
      cpos = 0;
    } else {
      token_data[cpos++] = ch;
    }
  }
  token_data[cpos++] = '\0';
  if (strlen(token_data) > 0) {
    add_duplicate(result, token_data);
  }

  return result;
}

// Add a *copy* of the string named data to the token list.
void add_duplicate(value_array_t* token_array, const char* data) {
  value_array_add(token_array, str_to_value(string_duplicate(data)));
}
