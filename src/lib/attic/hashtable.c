#line 2 "hashtable.c"
/**
 * @file hashtable.c
 *
 * A simple hashtable from keys to values.
 */

#ifndef _HASHTABLE_H_
#define _HASHTABLE_H_

#include <stdint.h>

struct hashtable_S {
  type_t* type;
  array_t(tuple_of_type(uint64_type(), K, V)) * storage;
  uint64_t number_of_keys;
};

#define hashtable_t(K, V) struct hashtable_S

extern hashtable_t(K, V)
    * make_hashtable(type_t* key_type, type_t* value_type,
                     uint32_t initial_capacity);
extern uint64_t hashtable_number_of_keys(hashtable_t(K, V) * hashtable);
extern reference_t hashtable_get_reference_to_value(hashtable_t(K, V)
                                                        * hashtable,
                                                    reference_t key_reference);
extern void hashtable_set_value(hashtable_t(K, V) * ht,
                                reference_t key_reference,
                                reference_t value_reference);
extern int hashtable_compare(hashtable_t(K, V) * a, hashtable_t(K, V) * b);

#endif /* _HASHTABLE_H_ */

// ======================================================================

#include <stdlib.h>

#define HT_ENTRY_HASHCODE_POSITION 0
#define HT_ENTRY_KEY_POSITION 1
#define HT_ENTRY_VALUE_POSITION 2

type_t* intern_hashtable_type(type_t* key_type, type_t* value_type);

/**
 * Make an array with the given initial_capacity.
 */
hashtable_t(K, V)
    * make_hashtable(type_t* key_type, type_t* value_type,
                     uint32_t initial_capacity) {
  if (initial_capacity == 0) {
    fatal_error(ERROR_ILLEGAL_INITIAL_CAPACITY);
  }
  type_t* hashtable_type = intern_hashtable_type(key_type, value_type);
  type_t* storage_type
      = intern_tuple_type(3, uint64_type(), key_type, value_type);
  hashtable_t(K, V)* result = malloc_struct(hashtable_t(K, V));
  result->type = hashtable_type;
  result->storage = make_array(storage_type, initial_capacity);
  result->storage->length = result->storage->capacity;
  return result;
}

/**
 * Return the number of actual entries in an array.
 */
uint64_t hashtable_size(hashtable_t(K, V) * ht) { return ht->number_of_keys; }

uint64_t hashtable_hash_key(hashtable_t(K, V) * ht, reference_t key_reference) {
  uint64_t hash = ht->type->parameters[0]->hash_fn(key_reference);
  // Reserve 0 so we can tell which buckets contain something. Any
  // value except 0 would be fine here but I choose a random looking
  // number that might be a prime.
  if (hash == 0) {
    hash = 113649;
  }
  return hash;
}

reference_t hashtable_get_reference_to_bucket(hashtable_t(K, V) * ht,
                                              uint64_t hashcode,
                                              uint64_t probe_number) {
  uint64_t position = (hashcode + probe_number) % ht->storage->length;
  return array_get_reference(ht->storage, position);
}

/**
 * Lookup a key in a hashtable.
 */
reference_t hashtable_get_reference_to_value(hashtable_t(K, V) * ht,
                                             reference_t key_reference) {
  uint64_t hashcode = hashtable_hash_key(ht, key_reference);
  for (int i = 0; true; i++) {
    reference_t bucket_reference
        = hashtable_get_reference_to_bucket(ht, hashcode, i);
    uint64_t stored_hashcode = dereference_uint64(tuple_reference_of_element(
        bucket_reference, HT_ENTRY_HASHCODE_POSITION));
    if (stored_hashcode == hashcode) {
      reference_t stored_key_reference
          = tuple_reference_of_element(bucket_reference, HT_ENTRY_KEY_POSITION);
      if (compare_references(key_reference, stored_key_reference) == 0) {
        return tuple_reference_of_element(bucket_reference,
                                          HT_ENTRY_VALUE_POSITION);
      }
    } else if (stored_hashcode == 0) {
      break;
    }
  }

  return nil();
}

void hashtable_set_value(hashtable_t(K, V) * ht, reference_t key_reference,
                         reference_t value_reference) {
  uint64_t hashcode = hashtable_hash_key(ht, key_reference);
  for (int i = 0; true; i++) {
    reference_t bucket_reference
        = hashtable_get_reference_to_bucket(ht, hashcode, i);
    uint64_t stored_hashcode = dereference_uint64(tuple_reference_of_element(
        bucket_reference, HT_ENTRY_HASHCODE_POSITION));
    if ((stored_hashcode == 0)
        || ((stored_hashcode == hashcode)
            && ((compare_references(key_reference, tuple_reference_of_element(
                                                       bucket_reference,
                                                       HT_ENTRY_KEY_POSITION)))
                == 0))) {
      tuple_write_element(bucket_reference, HT_ENTRY_HASHCODE_POSITION,
                          reference_of_uint64(&hashcode));
      tuple_write_element(bucket_reference, HT_ENTRY_KEY_POSITION,
                          key_reference);
      tuple_write_element(bucket_reference, HT_ENTRY_VALUE_POSITION,
                          value_reference);
      // TODO(jawilson): keep track of number of entries and possibly
      // grow if we exceed a certain load factor.
      return;
    }
  }
}

// ----------------------------------------------------------------------
// Some construction helper functions
// ----------------------------------------------------------------------

char* construct_hashtable_type_name(type_t* key_type, type_t* value_type) {
  buffer_t* name = make_buffer(32);
  name = buffer_append_string(name, "hashtable_t(");
  name = buffer_append_string(name, key_type->name);
  name = buffer_append_string(name, ",");
  name = buffer_append_string(name, value_type->name);
  name = buffer_append_string(name, ")");
  char* result = buffer_c_substring(name, 0, buffer_length(name));
  free(name);
  return result;
}

type_t* intern_hashtable_type(type_t* key_type, type_t* value_type) {
  type_t hashtable_type;
  hashtable_type.name = construct_hashtable_type_name(key_type, value_type);
  hashtable_type.size = sizeof(struct hashtable_S);
  hashtable_type.alignment = alignof(struct hashtable_S);
  hashtable_type.number_of_parameters = 2;
  hashtable_type.parameters[0] = key_type;
  hashtable_type.parameters[1] = value_type;
  // TODO(jawilson): compare_fn, append_fn, hash_fn which are only
  // needed to print or use the hashtable itself as the key to another
  // hashtable...
  return intern_type(hashtable_type);
}
