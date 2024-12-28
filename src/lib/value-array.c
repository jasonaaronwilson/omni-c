#line 2 "value-array.c"

/**
 * @file value-array.c
 *
 * This file contains a growable array of "values".
 *
 * Certain algorithms require that growth occurs based on the current
 * capacity of an array, not a fixed amount. For now we simply double
 * the current capacity when more space in the backing array is
 * required though we may scale this back to something more like 1.5X
 * for "large" arrays to save space.
 */

#ifndef _VALUE_ARRAY_H_
#define _VALUE_ARRAY_H_

struct value_array_S {
  uint32_t length;
  uint32_t capacity;
  value_t* elements;
};

/**
 * @typedef value_array_t
 *
 * A growable array of 64bit "values" (so integers, doubles, and
 * pointers).
 */
typedef struct value_array_S value_array_t;

extern value_array_t* make_value_array(uint64_t initial_capacity);
extern value_t value_array_get(value_array_t* array, uint32_t index);
extern void value_array_replace(value_array_t* array, uint32_t index,
                                value_t element);
extern void value_array_add(value_array_t* array, value_t element);
extern void value_array_push(value_array_t* array, value_t element);
extern value_t value_array_pop(value_array_t* array);
extern void value_array_insert_at(value_array_t* array, uint32_t position,
                                  value_t element);
extern value_t value_array_delete_at(value_array_t* array, uint32_t position);

#define value_array_get_ptr(array, index_expression, cast_type)                \
  (cast(cast_type, value_array_get(array, index_expression).ptr))

#endif /* _VALUE_ARRAY_H_ */

/**
 * @function make_value_array
 *
 * Make a value array with the given initial capacity.
 *
 * An initial capacity of zero is automatically converted to a
 * capacity of at least 1 or more since this makes the "growth" code a
 * bit simpler.
 *
 * When the array runs out of capacity because of calls to add, push,
 * etc., then the backing array is automatically increased *based on
 * the current capacity* which generally leads to better big-O
 * properties.
 *
 * If the initial or later increases in capacity are not fulfillable,
 * then a fatal_error occurs since these are generally not recoverable
 * anyways.
 */
value_array_t* make_value_array(uint64_t initial_capacity) {
  if (initial_capacity == 0) {
    initial_capacity = 1;
  }

  value_array_t* result = malloc_struct(value_array_t);
  result->capacity = initial_capacity;
  result->elements
      = cast(value_t*, malloc_bytes(sizeof(value_t) * initial_capacity));

  return result;
}

void value_array_ensure_capacity(value_array_t* array,
                                 uint32_t required_capacity) {
  if (array->capacity < required_capacity) {
    uint32_t new_capacity = array->capacity * 2;
    if (new_capacity < required_capacity) {
      new_capacity = required_capacity;
    }
    value_t* new_elements
        = cast(value_t*, malloc_bytes(sizeof(value_t) * new_capacity));
    for (int i = 0; i < array->length; i++) {
      new_elements[i] = array->elements[i];
    }
    array->capacity = new_capacity;
    free_bytes(array->elements);
    array->elements = new_elements;
    return;
  }
}

/**
 * @function value_array_get
 *
 * Get the value stored at index `index`. If the index is outside of
 * the range of valid elements, then a fatal_error is signaled.
 */
value_t value_array_get(value_array_t* array, uint32_t index) {
  if (index < array->length) {
    return array->elements[index];
  }
  fatal_error(ERROR_ACCESS_OUT_OF_BOUNDS);
  /* gcc and clang know fatal_error is _Noreturn but tcc doesn't */
  return compound_literal(value_t, {0});
}

/**
 * @function value_array_replace
 *
 * Replace the value at a given `index`. If the index is outside of
 * the range of valid elements, then a `fatal_error` is signaled.
 */
void value_array_replace(value_array_t* array, uint32_t index,
                         value_t element) {
  if (index < array->length) {
    array->elements[index] = element;
    return;
  }
  fatal_error(ERROR_ACCESS_OUT_OF_BOUNDS);
}

/**
 * @function value_array_add
 *
 * Add an element to the end of an array. If more space is required
 * then the backing array is automatically resized. This resizing
 * means that a fatal_error() may occur if malloc() can not satisfy the
 * allocation request.
 */
void value_array_add(value_array_t* array, value_t element) {
  value_array_ensure_capacity(array, array->length + 1);
  array->elements[(array->length)++] = element;
}

/**
 * @function value_array_push
 *
 * This is a synonym for value_array_add which serves to make it more
 * obvious that the array is actually being used like a stack.
 */
void value_array_push(value_array_t* array, value_t element) {
  value_array_add(array, element);
}

/**
 * @function value_array_pop
 *
 * Returns the last element of the array (typically added via push)
 * and modifies the length of the array so that the value isn't
 * accessible any longer. (We also "zero out" the element in case you
 * are using a conservative garbage collector.)
 *
 * If the array is currently empty, then
 * `fatal_error(ERROR_ACCESS_OUT_OF_BOUNDS)` is called.
 */
value_t value_array_pop(value_array_t* array) {
  if (array->length == 0) {
    fatal_error(ERROR_ACCESS_OUT_OF_BOUNDS);
  }
  uint32_t last_index = array->length - 1;
  value_t result = value_array_get(array, last_index);
  array->elements[last_index] = u64_to_value(0);
  array->length--;
  return result;
}

/**
 * @function value_array_insert_at
 *
 * Insert an element into some existing position in the array (or at
 * the end if position == the current array length).
 *
 * This operation is not efficient for large arrays as potentially the
 * entire array must be moved to new locations (there are
 * data-structures like trees that can make this more efficient though
 * such a data-structure isn't in this library yet).
 *
 * If the position is > than the length length, then
 * fatal_error(ERROR_ACCESS_OUT_OF_BOUNDS) is called.
 *
 * If memory allocation is required and malloc() fails, then
 * fatal_error(ERROR_MEMORY_ALLOCATION) is called.
 */
void value_array_insert_at(value_array_t* array, uint32_t position,
                           value_t element) {
  if (position == array->length) {
    value_array_add(array, element);
    return;
  }

  if (position > array->length) {
    fatal_error(ERROR_ACCESS_OUT_OF_BOUNDS);
    return;
  }

  value_array_ensure_capacity(array, array->length + 1);

  // This is the standard loop but we now need to use a signed index
  // because when the position is zero, zero - 1 is 0xffffffff which
  // is still greater than zero (and hence greater than position).
  for (int64_t i = array->length - 1; i >= position; i--) {
    array->elements[i + 1] = array->elements[i];
  }
  array->length++;
  array->elements[position] = element;
}

/**
 * @function value_array_delete_at
 *
 * Deletes the element at the given position (and return it so that it
 * can potentially be freed by the caller).
 *
 * If the position doesn't point to a valid element then
 * fatal_error(ERROR_ACCESS_OUT_OF_BOUNDS) is called.
 */
value_t value_array_delete_at(value_array_t* array, uint32_t position) {
  value_t result = value_array_get(array, position);
  for (int i = position; i < array->length - 1; i++) {
    array->elements[i] = array->elements[i + 1];
  }
  array->length--;
  return result;
}
