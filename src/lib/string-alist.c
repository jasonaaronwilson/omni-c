#line 2 "string-alist.c"
/**
 * @file string-alist.c
 *
 * An association list (a type of map) from a string to a value_t.
 *
 * This simply wraps value-alist.c.
 */

#ifndef _STRING_ALIST_H_
#define _STRING_ALIST_H_

struct string_alist_S {};

typedef struct string_alist_S string_alist_t;

/**
 * @function alist_find
 *
 * Find the value associate with the given key. Use is_ok() or
 * is_not_ok() to see if the value is valid (i.e., if the key was
 * actually found).
 */
static inline value_result_t alist_find(string_alist_t* list, char* key) {
  return value_alist_find(cast(value_alist_t*, list), cmp_string_values,
                          str_to_value(key));
}

/**
 * @function alist_insert
 *
 * Insert a new key and value into an assocation list.
 */
__attribute__((warn_unused_result)) static inline string_alist_t*
    alist_insert(string_alist_t* list, char* key, value_t value) {
  return cast(string_alist_t*,
              value_alist_insert(cast(value_alist_t*, list), cmp_string_values,
                                 str_to_value(key), value));
}

/**
 * @function alist_delete
 *
 * Delete the key and associated value from the given association
 * list. Neither the key nor the value associated are themselves
 * freed.
 */
__attribute__((warn_unused_result)) static inline string_alist_t*
    alist_delete(string_alist_t* list, char* key) {
  return cast(string_alist_t*,
              value_alist_delete(cast(value_alist_t*, list), cmp_string_values,
                                 str_to_value(key)));
}

/**
 * @function alist_length
 *
 * Determine the length of an alist.
 *
 * The alist argument MAY be null.
 */
__attribute__((warn_unused_result)) static inline uint64_t
    alist_length(string_alist_t* list) {
  return value_alist_length(cast(value_alist_t*, list));
}

/**
 * @macro string_alist_foreach
 *
 * Allows iteration over the keys and values in a string association
 * list.
 */
#define string_alist_foreach(alist, key_var, value_var, statements)            \
  do {                                                                         \
    value_alist_foreach(cast(value_alist_t*, alist), key_var##_value,          \
                        value_var, {                                           \
                          char* key_var = (key_var##_value).str;               \
                          statements;                                          \
                        });                                                    \
  } while (0)

#endif /* _STRING_ALIST_H_ */
