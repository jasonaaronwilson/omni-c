#line 2 "tuple.c"
/**
 * @file tuple.c
 *
 * Tuples are a low-level construct which are most useful for
 * implementing containers. The abstraction is like a C structure
 * except that "members" are referenced by position rather than name
 * and that the layout of tuples are determined by the tuple type not
 * the static declaration of the structure.
 */

// ======================================================================
// This section is extraced to tuple.h
// ======================================================================

#ifndef _TUPLE_H_
#define _TUPLE_H_

#include <stdint.h>

// Technically there is no struct that can really represent a tuple
// and the alignment can be as little as one byte. I think this will
// eventually prove to be the wrong thing.
typedef struct {
  __attribute__((aligned(8))) uint8_t data[0];
} tuple_t;

extern type_t* intern_tuple_type(int number_of_parameters, ...);

// Rename all of these since they are in fact painfully long...

extern reference_t tuple_reference_of_element(reference_t tuple,
                                              uint64_t position);

extern reference_t tuple_reference_of_element_from_pointer(
    type_t* type, tuple_t* tuple_pointer, uint64_t position);

extern void tuple_write_element(reference_t tuple_ref, uint64_t position,
                                reference_t value);

extern struct buffer_S* tuple_append_text(struct buffer_S* buffer,
                                          reference_t tuple_ref);

#endif /* _TUPLE_H_ */

// ======================================================================

#include <stdarg.h>
#include <stdlib.h>

#define TUPLE_ALIGN_OFFSET(offset, alignment)                                  \
  ((offset + (alignment - 1)) & ~(alignment - 1))

/**
 * Make a tuple type.
 */
type_t* intern_tuple_type(int number_of_parameters, ...) {
  type_t* result = (malloc_struct(type_t));

  buffer_t* name = make_buffer(32);
  name = buffer_append_string(name, "tuple(");

  int offset = 0;
  int alignment = 1;

  va_list args;
  va_start(args, number_of_parameters);
  for (int i = 0; (i < number_of_parameters); i++) {

    type_t* element_type = va_arg(args, type_t*);
    result->parameters[result->number_of_parameters++] = element_type;
    offset = TUPLE_ALIGN_OFFSET(offset, element_type->alignment);
    if (element_type->size <= 0) {
      fatal_error(ERROR_DYNAMICALLY_SIZED_TYPE_ILLEGAL_IN_CONTAINER);
    }
    if (element_type->alignment > alignment) {
      alignment = element_type->alignment;
    }
    if (i > 0) {
      name = buffer_append_string(name, ",");
    }
    name = buffer_append_string(name, element_type->name);
    offset += element_type->size;
  }
  va_end(args);

  // TODO(jawilson): make sure alignment is a power of two.

  result->size = offset;
  result->alignment = alignment;
  name = buffer_append_string(name, ")");
  result->name = buffer_c_substring(name, 0, buffer_length(name));
  free(name);

  result->append_fn = &tuple_append_text;

  // TODO(jawilson): compare_fn, append_fn, hash_fn
  // TODO(jawilson): actually intern the type!

  return intern_type(*result);
}

/**
 * Get a reference to a "member" element (from a reference to a tuple).
 */
reference_t tuple_reference_of_element(reference_t tuple_ref,
                                       uint64_t position) {
  // Make sure the reference is to a tuple?
  type_t* type = tuple_ref.underlying_type;
  tuple_t* tuple_pointer = tuple_ref.pointer;

  uint64_t offset = 0;
  for (int i = 0; (i < type->number_of_parameters); i++) {
    type_t* element_type = type->parameters[i];
    offset = TUPLE_ALIGN_OFFSET(offset, element_type->alignment);
    if (i == position) {
      return reference_of(element_type, &tuple_pointer->data[offset]);
    }
    offset += element_type->size;
  }
  fatal_error(ERROR_ACCESS_OUT_OF_BOUNDS);
}

void tuple_write_element(reference_t tuple_ref, uint64_t position,
                         reference_t value) {
  reference_t element_reference
      = tuple_reference_of_element(tuple_ref, position);
  if (element_reference.underlying_type != value.underlying_type) {
    fatal_error(ERROR_REFERENCE_NOT_EXPECTED_TYPE);
  }
  /*
  TRACE();
  fprintf(stderr, "Writing size=%d bytes to address %ul\n",
          value.underlying_type->size,
          element_reference.pointer);
  */
  memcpy(element_reference.pointer, value.pointer, value.underlying_type->size);
}

struct buffer_S* tuple_append_text(struct buffer_S* buffer,
                                   reference_t tuple_ref) {
  // Make sure the reference is to a tuple?
  type_t* type = tuple_ref.underlying_type;
  tuple_t* tuple_pointer = tuple_ref.pointer;

  buffer = buffer_append_string(buffer, "tuple(");
  for (int i = 0; (i < tuple_ref.underlying_type->number_of_parameters); i++) {
    if (i > 0) {
      buffer = buffer_append_string(buffer, ", ");
    }
    type_t* element_type = type->parameters[i];
    reference_t element_ref = tuple_reference_of_element(tuple_ref, i);
    buffer = element_type->append_fn(buffer, element_ref);
  }
  buffer = buffer_append_string(buffer, ")");
  return buffer;
}
