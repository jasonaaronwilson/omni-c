#line 2 "hashtree.c"
/**
 * @file hashtree.c
 *
 * A hashtree is a binary tree where the order of keys is based on the
 * hash of the key rather than the key itself (hence it appears
 * unsorted to an external observer).
 *
 * Since hashcodes are simply 64bit numbers, they can be very
 * efficiently compared versus complex keys (which obviously can then
 * take more time to compare) providing a constant factor speedup
 * versus a tree sorted by key. Additionally, hashtrees do not require
 * balancing operations like normal binary trees to provide O(lg(n))
 * operations under normal conditions which simplify their
 * implementation.
 *
 * Hashtrees have better space efficieny versus a hashtable but
 * obviously have slower lookup, insert, and delete (and will incur
 * more cache misses). The real reason we implement hashtrees is to
 * allow hashtables a simple but efficient mechanism to handle bucket
 * collisions and thus permit higher load in the hashtable itself
 * while still providing good performance.
 *
 * Like tuples, hashtrees do not carry their key and value types with
 * them so the types must be provided to all operations that operate
 * on them, one reason they be less desireable to use than hashtables.
 */

#ifndef _HASHTREE_H_
#define _HASHTREE_H_

#include <stdint.h>

// Each node in the tree is simply a tuple of hashcode, left (pointer), right
// (pointer), K, and V.
#define hashtree_t(K, V) tuple_t

extern hashtree_t(K, V)
    * make_hashtree_node(type_t* key_type, type_t* value_type);

extern reference_t hashtree_get_reference_to_value(hashtree_t(K, V) * htree,
                                                   type_t* key_type,
                                                   type_t* value_type,
                                                   uint64_t hashcode,
                                                   reference_t key_reference);

extern boolean_t hashtree_insert(pointer_t(hashtree_t(K, V)) htree,
                                 type_t* key_type, type_t* value_type,
                                 uint64_t hashcode, reference_t key_reference,
                                 reference_t value_reference);

extern void hashtree_delete(pointer_t(hashtree_t(K, V)) htree, type_t* key_type,
                            type_t* value_type, reference_t key_reference);

#endif /* _HASHTREE_H_ */

#define HTREE_HASHCODE_POSITION 0
#define HTREE_LEFT_POSITION 1
#define HTREE_RIGHT_POSITION 2
#define HTREE_KEY_POSITION 3
#define HTREE_VALUE_POSITION 4

#define tuple_write tuple_write_element
#define tuple_read tuple_reference_of_element

type_t* intern_hashtree_type(type_t* key_type, type_t* value_type) {
  return intern_tuple_type(5, uint64_type(), self_ptr_type(), self_ptr_type(),
                           key_type, value_type);
}

pointer_t(hashtree_t(K, V))
    make_empty_hashtree_node(type_t* key_type, type_t* value_type) {
  type_t* node_type = intern_hashtree_type(key_type, value_type);
  return (pointer_t(hashtree_t(K, V)))(malloc_bytes(node_type->size));
}

reference_t hashtree_get_reference_to_value(pointer_t(hashtree_t(K, V)) htree,
                                            type_t* key_type,
                                            type_t* value_type,
                                            uint64_t hashcode,
                                            reference_t key_reference) {
  type_t* node_type = intern_hashtree_type(key_type, value_type);
  reference_t node_ref = reference_of(node_type, htree);

  uint64_t node_hashcode = dereference_uint64(
      tuple_reference_of_element(node_ref, HTREE_HASHCODE_POSITION));

  if (node_hashcode == 0) {
    return nil();
  }

  if (hashcode == node_hashcode) {
    // TODO(jawilson): check key!!!
    return tuple_reference_of_element(node_ref, HTREE_VALUE_POSITION);
  } else if (hashcode < node_hashcode) {
    pointer_t(hashtree_t(K, V)) left = *(
        (hashtree_t(K, V)**) tuple_read(node_ref, HTREE_LEFT_POSITION).pointer);
    if (left == NULL) {
      return nil();
    } else {
      return hashtree_get_reference_to_value(left, key_type, value_type,
                                             hashcode, key_reference);
    }

  } else {
    // GO RIGHT
    return nil();
  }
}


/**
 * Insert (or replace) a mapping from K to V in the tree. If the value
 * is not already in the hashtable than this will always add a new
 * node as a leaf node.
 *
 * When the htree tuple has a current hashcode of zero, we assume it
 * is an empty root node and those overwrite an empty node. For this
 * reason, the input hashcode for the key should never itself be zero
 * (simply consistently map zero to any other value like hashtable
 * already does).
 */
boolean_t hashtree_insert(pointer_t(hashtree_t(K, V)) htree, type_t* key_type,
                          type_t* value_type, uint64_t hashcode,
                          reference_t key_reference,
                          reference_t value_reference) {

  if (hashcode == 0) {
    fatal_error(ERROR_ILLEGAL_ZERO_HASHCODE_VALUE);
  }

  type_t* node_type = intern_hashtree_type(key_type, value_type);
  reference_t node_ref = reference_of(node_type, htree);

  // TODO(jawilson): make sure types match
  uint64_t node_hashcode = dereference_uint64(
      tuple_reference_of_element(node_ref, HTREE_HASHCODE_POSITION));
  if (node_hashcode == 0) {
    tuple_write(node_ref, HTREE_HASHCODE_POSITION,
                reference_of_uint64(&hashcode));
    tuple_write(node_ref, HTREE_KEY_POSITION, key_reference);
    tuple_write(node_ref, HTREE_VALUE_POSITION, value_reference);
    return true;
  } else if (hashcode == node_hashcode) {
    // TODO(jawilson): make sure the keys are actually the same!
    tuple_write(node_ref, HTREE_VALUE_POSITION, value_reference);
    return false;
  } else if (hashcode < node_hashcode) {
    hashtree_t(K, V)* left = *(
        (hashtree_t(K, V)**) tuple_read(node_ref, HTREE_LEFT_POSITION).pointer);
    if (left == NULL) {
      hashtree_t(K, V)* new_node
          = make_empty_hashtree_node(key_type, value_type);
      reference_t new_node_ref = reference_of(node_type, new_node);
      reference_t new_node_pointer_ref
          = reference_of(node_type->parameters[HTREE_LEFT_POSITION], &new_node);
      tuple_write(new_node_ref, HTREE_HASHCODE_POSITION,
                  reference_of_uint64(&hashcode));
      tuple_write(new_node_ref, HTREE_KEY_POSITION, key_reference);
      tuple_write(new_node_ref, HTREE_VALUE_POSITION, value_reference);
      tuple_write(node_ref, HTREE_LEFT_POSITION, new_node_pointer_ref);
      return true;
    } else {
      return hashtree_insert(left, key_type, value_type, hashcode,
                             key_reference, value_reference);
    }
  } else {
    hashtree_t(K, V)* right
        = *((hashtree_t(K, V)**) tuple_read(node_ref, HTREE_RIGHT_POSITION)
                .pointer);
    if (right == NULL) {
      hashtree_t(K, V)* new_node
          = make_empty_hashtree_node(key_type, value_type);
      reference_t new_node_ref = reference_of(node_type, new_node);
      reference_t new_node_pointer_ref = reference_of(
          node_type->parameters[HTREE_RIGHT_POSITION], &new_node);
      tuple_write(new_node_ref, HTREE_HASHCODE_POSITION,
                  reference_of_uint64(&hashcode));
      tuple_write(new_node_ref, HTREE_KEY_POSITION, key_reference);
      tuple_write(new_node_ref, HTREE_VALUE_POSITION, value_reference);
      tuple_write_element(node_ref, HTREE_RIGHT_POSITION, new_node_pointer_ref);
      return true;
    } else {
      return hashtree_insert(right, key_type, value_type, hashcode,
                             key_reference, value_reference);
    }
  }
}

void hashtree_delete(pointer_t(hashtree_t(K, V)) htree, type_t* key_type,
                     type_t* value_type, reference_t key_reference) {
  fatal_error(ERROR_UNIMPLEMENTED);
}
