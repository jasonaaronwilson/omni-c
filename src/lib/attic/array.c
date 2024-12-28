#line 2 "array.c"

/**
 * @file array.c
 *
 * This file contains a growable array of values. Any function that
 * changes the length of an array may return a new pointer.
 */

// ======================================================================
// This is block is extraced to array.h
// ======================================================================

#ifndef _ARRAY_H_
#define _ARRAY_H_

#include <stdint.h>

struct array_S {
  type_t* element_type;
  uint32_t length;
  uint32_t capacity;
  __attribute__((aligned(8))) uint8_t data[0];
};

#define array_t(t) struct array_S

extern array_t(|?|)* make_array(type_t* element_type, uint32_t initial_capacity);
extern uint64_t array_length(array_t(|?|)* arr);
extern reference_t array_get_reference(array_t(|?|)* arr, uint64_t position);
__attribute__((warn_unused_result)) extern array_t(|?|)*
    array_add(array_t(|?|)* arr, reference_t element);

#endif /* _ARRAY_H_ */

// ======================================================================

#include <stdlib.h>

static inline void* array_address_of_element(array_t(|?|)* array,
                                             uint64_t position) {
  void* result = &(array->data[0]) + position * array->element_type->size;
  return result;
}

/**
 * Make an array with the given initial_capacity.
 */
array_t(|?|)* make_array(type_t* type, uint32_t initial_capacity) {
  if (initial_capacity == 0) {
    fatal_error(ERROR_ILLEGAL_INITIAL_CAPACITY);
  }

  int element_size = type->size;
  if (element_size <= 0) {
    fatal_error(ERROR_DYNAMICALLY_SIZED_TYPE_ILLEGAL_IN_CONTAINER);
  }
  array_t(|?|)* result = (array_t(|?|)*) (malloc_bytes(
      sizeof(array_t(|?|)) + (element_size * initial_capacity)));
  result->element_type = type;
  result->length = 0;
  result->capacity = initial_capacity;
  return result;
}

/**
 * Return the number of actual entries in an array.
 */
uint64_t array_length(array_t(|?|)* arr) { return arr->length; }

/**
 * Get the nth element from an array.
 */
reference_t array_get_reference(array_t(|?|)* array, uint64_t position) {
  if (position < array->length) {
    return reference_of(array->element_type,
                        array_address_of_element(array, position));
  } else {
    fprintf(stderr, "%s:%d: position is %lu but array length is %d\n", __FILE__,
            __LINE__, position, array->length);
    fatal_error(ERROR_ACCESS_OUT_OF_BOUNDS);
  }
}

/**
 * Add an element to the end of an array.
 */
__attribute__((warn_unused_result)) array_t(|?|)* array_add(array_t(|?|)* array,
                                                            reference_t reference) {
  if (reference.underlying_type != array->element_type) {
    fatal_error(ERROR_REFERENCE_NOT_EXPECTED_TYPE);
  }
  int size = array->element_type->size;
  if (array->length < array->capacity) {
    memcpy(array_address_of_element(array, array->length), reference.pointer,
           size);
    array->length++;
    return array;
  } else {
    array_t(|?|)* result = make_array(array->element_type, array->capacity * 2);
    memcpy(array_address_of_element(result, 0),
           array_address_of_element(array, 0), size * array->length);
    result->length = array->length;
    free_bytes(array);
    array = NULL;
    return array_add(result, reference);
  }
}
