#line 2 "value-hashtable.c"
/**
 * @file value-hashtable.c
 *
 * A very thread-unsafe hash map of value_t to value_t.
 *
 * Please don't expect C++, JVM, or Rust level of performance since we
 * use chaining which is considered slower than open addressing.
 */

#ifndef _VALUE_HASHTABLE_H_
#define _VALUE_HASHTABLE_H_

/**
 * @compiliation_option ARMYKNIFE_HT_LOAD_FACTOR
 *
 * The "load factor" is the ratio of the number of keys in the hash
 * table to the most optimistic capacity for the table if every key
 * happened to be hashed to a different bucket. When the load factor
 * reaches this value, the hash table will be resized to a larger
 * capacity to improve performance. A higher value allows for a denser
 * hash table but can lead to more collisions and slower lookups and
 * insertions. A lower value wastes memory but reduces collisions.
 */
#ifndef ARMYKNIFE_HT_LOAD_FACTOR
#define ARMYKNIFE_HT_LOAD_FACTOR 0.75
#endif /* ARMYKNIFE_HT_LOAD_FACTOR */

/**
 * @compiliation_option AK_HT_UPSCALE_MULTIPLIER
 *
 * In all cases this should be a number > 1.0.
 */
#ifndef AK_HT_UPSCALE_MULTIPLIER
#define AK_HT_UPSCALE_MULTIPLIER 1.75
#endif /* AK_HT_UPSCALE_MULTIPLIER */

struct value_hashtable_S {
  uint64_t n_buckets;
  uint64_t n_entries;
  value_alist_t** buckets;
};

typedef struct value_hashtable_S value_hashtable_t;

extern value_hashtable_t* make_value_hashtable(uint64_t n_buckets);

extern value_hashtable_t* value_ht_insert(value_hashtable_t* ht,
                                          value_hash_fn hash_fn,
                                          value_comparison_fn cmp_fn,
                                          value_t key, value_t value);

extern value_hashtable_t* value_ht_delete(value_hashtable_t* ht,
                                          value_hash_fn hash_fn,
                                          value_comparison_fn cmp_fn,
                                          value_t key);

extern value_result_t value_ht_find(value_hashtable_t* ht,
                                    value_hash_fn hash_fn,
                                    value_comparison_fn cmp_fn, value_t key);

extern void value_hashtable_upsize_internal(value_hashtable_t* ht,
                                            value_hash_fn hash_fn,
                                            value_comparison_fn cmp_fn);

/**
 * @function value_ht_num_entries
 *
 * Returns the number of entries in the hashtable.
 */
static inline uint64_t value_ht_num_entries(value_hashtable_t* ht) {
  return ht->n_entries;
}

/**
 * @macro value_ht_foreach
 *
 * Allows traversing all elements of a hashtable in an unspecified
 * order.
 */
#define value_ht_foreach(ht, key_var, value_var, statements)                   \
  do {                                                                         \
    for (int ht_index = 0; ht_index < ht->n_buckets; ht_index++) {             \
      value_alist_t* alist = ht->buckets[ht_index];                            \
      if (alist != NULL) {                                                     \
        value_alist_foreach(alist, key_var, value_var, statements);            \
      }                                                                        \
    }                                                                          \
  } while (0)

#endif /* _VALUE_HASHTABLE_H_ */

/**
 * @function make_value_hashtable
 *
 * Create a hashtable with the given number of buckets.
 *
 * When the initial number of buckets is less than a small integer
 * (currently 2), then we automatically increase the initial number of
 * buckets to make the fractional growth algorithm work easier to
 * reason about.
 */
value_hashtable_t* make_value_hashtable(uint64_t n_buckets) {
  if (n_buckets < 2) {
    n_buckets = 2;
  }
  value_hashtable_t* result = malloc_struct(value_hashtable_t);
  result->n_buckets = n_buckets;
  result->buckets
      = cast(value_alist_t**,
             malloc_bytes(sizeof(typeof(value_alist_t*)) * n_buckets));
  return result;
}

/**
 * @function value_ht_insert
 *
 * Insert an association into the hashtable.
 */
value_hashtable_t* value_ht_insert(value_hashtable_t* ht, value_hash_fn hash_fn,
                                   value_comparison_fn cmp_fn, value_t key,
                                   value_t value) {
  uint64_t hashcode = hash_fn(key);
  int bucket = hashcode % ht->n_buckets;
  value_alist_t* list = ht->buckets[bucket];
  uint64_t len = value_alist_length(list);
  list = value_alist_insert(list, cmp_fn, key, value);
  ht->buckets[bucket] = list;
  uint64_t len_after = value_alist_length(list);
  if (len_after > len) {
    ht->n_entries++;
    // Without this, a hash table would never grow and thus as the
    // number of entries grows large, the hashtable would only improve
    // performance over an alist by a constant amount (which could
    // still be an impressive speedup...)
    if (ht->n_entries >= (ht->n_buckets * ARMYKNIFE_HT_LOAD_FACTOR)) {
      value_hashtable_upsize_internal(ht, hash_fn, cmp_fn);
    }
  }
  return ht;
}

/**
 * @function value_ht_delete
 *
 * Delete an association from the hashtable. It is not an error to
 * delete a key that doesn't exist in the hashtable.
 */
value_hashtable_t* value_ht_delete(value_hashtable_t* ht, value_hash_fn hash_fn,
                                   value_comparison_fn cmp_fn, value_t key) {
  uint64_t hashcode = hash_fn(key);
  int bucket = hashcode % ht->n_buckets;
  value_alist_t* list = ht->buckets[bucket];
  uint64_t len = value_alist_length(list);
  list = value_alist_delete(list, cmp_fn, key);
  ht->buckets[bucket] = list;
  uint64_t len_after = value_alist_length(list);
  if (len_after < len) {
    ht->n_entries--;
  }
  return ht;
}

/**
 * @function value_ht_find
 *
 * Find an association in the hashtable.
 */
value_result_t value_ht_find(value_hashtable_t* ht, value_hash_fn hash_fn,
                             value_comparison_fn cmp_fn, value_t key) {
  uint64_t hashcode = hash_fn(key);
  int bucket = hashcode % ht->n_buckets;
  value_alist_t* list = ht->buckets[bucket];
  return value_alist_find(list, cmp_fn, key);
}

/**
 * @function value_hashtable_upsize_internal
 *
 * This function is called automatically when an insert brings the
 * number of entries above the number of buckets times
 * ARMYKNIFE_HT_LOAD_FACTOR (defaults to 75%). We don't even check
 * that constraint is valid (hence the _internal suffix).
 *
 * Hopefully based on the name you can infer this function will only
 * ever "grow" a hashtable by deciding on a size of the new larger
 * hash-table and copying all of the entries to the bigger hashtable
 * (using AK_HT_UPSCALE_MULTIPLIER to compute the new number of
 * buckets, currently 1.75).
 */
void value_hashtable_upsize_internal(value_hashtable_t* ht,
                                     value_hash_fn hash_fn,
                                     value_comparison_fn cmp_fn) {
  uint64_t new_num_buckets = ht->n_buckets * AK_HT_UPSCALE_MULTIPLIER;
  value_hashtable_t* new_ht = make_value_hashtable(new_num_buckets);
  // clang-format off
  value_ht_foreach(ht, key, value, block_expr({
      value_hashtable_t* should_be_result = value_ht_insert(new_ht, hash_fn, cmp_fn, key, value);
      // If an insertion into the bigger hashtable results in it's own
      // resize, then the results will be unpredictable (at least
      // without more code). This is likely to only happen when
      // growing a very small hashtable and depends on values choosen
      // for ARMYKNIFE_HT_LOAD_FACTOR and AK_HT_UPSCALE_MULTIPLIER.
      if (new_ht != should_be_result) {
	fatal_error(ERROR_ILLEGAL_STATE);
      }
  }));
  // clang-format on
  value_alist_t** old_buckets = ht->buckets;
  ht->buckets = new_ht->buckets;
  ht->n_buckets = new_ht->n_buckets;
  ht->n_entries = new_ht->n_entries;
  free_bytes(old_buckets);
  free_bytes(new_ht);
}
