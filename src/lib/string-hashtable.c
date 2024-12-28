#line 2 "string-hashtable.c"
/**
 * @file string-hashtable.c
 *
 * A very thread-unsafe hash map of C style zero terminated byte
 * "strings" to a value_t. This is an oqaque reference around a
 * value_hashtable.
 */

#ifndef _STRING_HASHTABLE_H_
#define _STRING_HASHTABLE_H_

struct string_hashtable_S {};

typedef struct string_hashtable_S string_hashtable_t;

static inline value_hashtable_t* to_value_hashtable(string_hashtable_t* ht) {
  return cast(value_hashtable_t*, ht);
}

/**
 * @function make_string_hashtable
 *
 * Create a hashtable with the given number of buckets.
 *
 * The minimum number of buckets is currently 2 to make it less likely
 * we run into some resize loop depending on the values of
 * ARMYKNIFE_HT_LOAD_FACTOR and AK_HT_UPSCALE_MULTIPLIER).
 */
static inline string_hashtable_t* make_string_hashtable(uint64_t n_buckets) {
  return cast(string_hashtable_t*, make_value_hashtable(n_buckets));
}

/**
 * @function string_ht_insert
 *
 * Insert an association into the hashtable.
 */
__attribute__((warn_unused_result)) static inline string_hashtable_t*
    string_ht_insert(string_hashtable_t* ht, char* key, value_t value) {
  return cast(string_hashtable_t*,
              value_ht_insert(to_value_hashtable(ht), hash_string_value,
                              cmp_string_values, str_to_value(key), value));
}

/**
 * @function string_ht_delete
 *
 * Delete an association from the hashtable. It is not an error to
 * delete a key that doesn't exist in the hashtable.
 */
__attribute__((warn_unused_result)) static inline string_hashtable_t*
    string_ht_delete(string_hashtable_t* ht, char* key) {
  return cast(string_hashtable_t*,
              value_ht_delete(to_value_hashtable(ht), hash_string_value,
                              cmp_string_values, str_to_value(key)));
}

/**
 * @function string_ht_find
 *
 * Find an association in the hashtable.
 */
static inline value_result_t string_ht_find(string_hashtable_t* ht, char* key) {
  return value_ht_find(to_value_hashtable(ht), hash_string_value,
                       cmp_string_values, str_to_value(key));
}

/**
 * @function string_ht_num_entries
 *
 * Returns the number of entries in the hashtable.
 */
static inline uint64_t string_ht_num_entries(string_hashtable_t* ht) {
  return value_ht_num_entries(to_value_hashtable(ht));
}

/**
 * @macro string_ht_foreach
 *
 * Allows traversing all elements of a hashtable in an unspecified
 * order.
 */
#define string_ht_foreach(ht, key_var, value_var, statements)                  \
  do {                                                                         \
    value_ht_foreach(to_value_hashtable(ht), key_var##_value, value_var, {     \
      char* key_var = (key_var##_value).str;                                   \
      statements;                                                              \
    });                                                                        \
  } while (0)

#endif /* _STRING_HASHTABLE_H_ */
