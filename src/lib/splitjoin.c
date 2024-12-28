#line 2 "splitjoin.c"

#ifndef _SPLITJOIN_H_
#define _SPLITJOIN_H_

buffer_t* join_array_of_strings(value_array_t* array_of_strings,
                                char* separator);

#endif /* _SPLITJOIN_H_ */

/**
 * @file splitjoin.c
 *
 * This badly named file handles the very common operations of
 * splitting a string into an array of sub-strings or joining an array
 * of strings into one large string. The Guava Java library has some
 * very configurable routines for doing this though we are just
 * implementing some quick and dirty routines for use in the omni-c
 * compiler.
 */

/**
 * @function join_array_of_strings
 *
 * Join an array of strings placing separator (which may be an empty
 * string but not NULL) between strings.
 */
buffer_t* join_array_of_strings(value_array_t* array_of_strings,
                                char* separator) {
  buffer_t* result = make_buffer(1);
  for (int i = 0; i < array_of_strings->length; i++) {
    if (i > 0) {
      buffer_append_string(result, separator);
    }
    buffer_append_string(result, value_array_get(array_of_strings, i).str);
  }
  return result;
}

// TODO(jawilson): add split_buffer_to_array_of_strings - this is
// similar to string_tokenize except instead of splitting on a
// character set it should split on a string.
