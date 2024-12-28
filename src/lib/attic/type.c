#line 2 "type.c"
/**
 * @file type.c
 *
 * This is a runtime type so that containers can do dynamic type
 * checking since the C compiler isn't capable of type checking
 * automatically.
 */

#ifndef _TYPE_H_
#define _TYPE_H_

#include <stdalign.h>
#include <stdint.h>
#include <stdlib.h>

#define MAX_TYPE_PARAMETERS 8

struct reference_S;
struct type_S;

typedef int (*compare_references_fn_t)(struct reference_S a,
                                       struct reference_S b);

typedef uint64_t (*hash_reference_fn_t)(struct reference_S object);

typedef buffer_t* (*append_text_representation_fn_t)(buffer_t* buffer,
                                                     struct reference_S object);

struct type_S {
  char* name;
  int size;
  int alignment;
  uint64_t number_of_parameters;
  struct type_S* parameters[MAX_TYPE_PARAMETERS];
  compare_references_fn_t compare_fn;
  hash_reference_fn_t hash_fn;
  append_text_representation_fn_t append_fn;
};
typedef struct type_S type_t;

extern type_t* intern_type(type_t type);

extern type_t char_ptr_type_constant;
extern type_t nil_type_constant;
extern type_t self_ptr_type_constant;

extern type_t uint64_type_constant;
extern type_t uint32_type_constant;
extern type_t uint16_type_constant;
extern type_t uint8_type_constant;

extern type_t int64_type_constant;
extern type_t int32_type_constant;
extern type_t int16_type_constant;
extern type_t int8_type_constant;

static inline type_t* nil_type() { return &nil_type_constant; }
static inline type_t* char_ptr_type() { return &char_ptr_type_constant; }
static inline type_t* self_ptr_type() { return &self_ptr_type_constant; }

static inline type_t* uint64_type() { return &uint64_type_constant; }
static inline type_t* uint32_type() { return &uint32_type_constant; }
static inline type_t* uint16_type() { return &uint16_type_constant; }
static inline type_t* uint8_type() { return &uint8_type_constant; }

static inline type_t* int64_type() { return &int64_type_constant; }
static inline type_t* int32_type() { return &int32_type_constant; }
static inline type_t* int16_type() { return &int16_type_constant; }
static inline type_t* int8_type() { return &int8_type_constant; }

#define pointer_t(t) t*

#endif /* _TYPE_H_ */

#include <stdalign.h>

type_t* intern_type(type_t type) {
  WARN("intern_type is not actually doing interning");
  type_t* result = (type_t*) malloc_copy_of((uint8_t*) &type, sizeof(type));
  return result;
}

int compare_string_references(reference_t ref_a, reference_t ref_b) {
  return strcmp(dereference_char_ptr(ref_a), dereference_char_ptr(ref_b));
}

int compare_uint64_t(struct reference_S a, struct reference_S b) {
  return dereference_uint64(a) - dereference_uint64(b);
}

// etc.

uint64_t hash_reference_bytes(reference_t reference) {
  // Actually call fasthash64!
  return 12;
}

uint64_t hash_string_reference(reference_t reference) {
  // Hash the underlying string (we won't know it's size like above).
  return 12;
}

buffer_t* append_string_text(buffer_t* buffer, struct reference_S object) {
  char* str = dereference_char_ptr(object);
  buffer = buffer_append_byte(buffer, '"');
  // TODO(jawilson): quote "
  buffer = buffer_append_string(buffer, str);
  buffer = buffer_append_byte(buffer, '"');
  return buffer;
}

struct buffer_S* append_uint64_text(struct buffer_S* buffer,
                                    struct reference_S object) {
  char buf[64];
  uint64_t number = dereference_uint64(object);
  sprintf(buf, "%lu", number);
  return buffer_append_string(buffer, buf);
}

struct buffer_S* append_uint32_text(struct buffer_S* buffer,
                                    struct reference_S object) {
  char buf[64];
  uint64_t number = dereference_uint32(object);
  sprintf(buf, "%lu", number);
  return buffer_append_string(buffer, buf);
}

struct buffer_S* append_uint16_text(struct buffer_S* buffer,
                                    struct reference_S object) {
  char buf[64];
  uint64_t number = dereference_uint16(object);
  sprintf(buf, "%lu", number);
  return buffer_append_string(buffer, buf);
}

struct buffer_S* append_uint8_text(struct buffer_S* buffer,
                                   struct reference_S object) {
  char buf[64];
  uint64_t number = dereference_uint8(object);
  sprintf(buf, "%lu", number);
  return buffer_append_string(buffer, buf);
}

struct buffer_S* append_int64_text(struct buffer_S* buffer,
                                   struct reference_S object) {
  char buf[64];
  int64_t number = dereference_int64(object);
  sprintf(buf, "%ld", number);
  return buffer_append_string(buffer, buf);
}

struct buffer_S* append_int32_text(struct buffer_S* buffer,
                                   struct reference_S object) {
  char buf[64];
  int32_t number = dereference_int32(object);
  sprintf(buf, "%d", number);
  return buffer_append_string(buffer, buf);
}

struct buffer_S* append_int16_text(struct buffer_S* buffer,
                                   struct reference_S object) {
  char buf[64];
  int16_t number = dereference_int16(object);
  sprintf(buf, "%d", number);
  return buffer_append_string(buffer, buf);
}

struct buffer_S* append_int8_text(struct buffer_S* buffer,
                                  struct reference_S object) {
  char buf[64];
  int8_t number = dereference_int8(object);
  sprintf(buf, "%d", number);
  return buffer_append_string(buffer, buf);
}

type_t uint64_type_constant = {
    .name = "uint64_t",
    .size = sizeof(uint64_t),
    .alignment = alignof(uint64_t),
    .hash_fn = &hash_reference_bytes,
    .append_fn = &append_uint64_text,
};

type_t uint32_type_constant = {
    .name = "uint32_t",
    .size = sizeof(uint32_t),
    .alignment = alignof(uint32_t),
    .hash_fn = &hash_reference_bytes,
    .append_fn = &append_uint32_text,
};

type_t uint16_type_constant = {
    .name = "uint16_t",
    .size = sizeof(uint16_t),
    .alignment = alignof(uint16_t),
    .hash_fn = &hash_reference_bytes,
    .append_fn = &append_uint16_text,
};

type_t uint8_type_constant = {
    .name = "uint8_t",
    .size = sizeof(uint8_t),
    .alignment = alignof(uint8_t),
    .hash_fn = &hash_reference_bytes,
    .append_fn = &append_uint8_text,
};

type_t int64_type_constant = {
    .name = "int64_t",
    .size = sizeof(int64_t),
    .alignment = alignof(int64_t),
    .hash_fn = &hash_reference_bytes,
    .append_fn = &append_int64_text,
};

type_t int32_type_constant = {
    .name = "int32_t",
    .size = sizeof(int32_t),
    .alignment = alignof(int32_t),
    .hash_fn = &hash_reference_bytes,
    .append_fn = &append_int32_text,
};

type_t int16_type_constant = {
    .name = "int16_t",
    .size = sizeof(int16_t),
    .alignment = alignof(int16_t),
    .hash_fn = &hash_reference_bytes,
    .append_fn = &append_int16_text,
};

type_t int8_type_constant = {
    .name = "int8_t",
    .size = sizeof(int8_t),
    .alignment = alignof(int8_t),
    .hash_fn = &hash_reference_bytes,
    .append_fn = &append_int8_text,
};

type_t char_type_constant = {
    .name = "char",
    .size = sizeof(char),
    .alignment = alignof(char),
    .hash_fn = &hash_reference_bytes,
};

type_t double_type_constant = {
    .name = "double",
    .size = sizeof(double),
    .alignment = alignof(double),
    .hash_fn = &hash_reference_bytes,
};

type_t float_type_constant = {
    .name = "float",
    .size = sizeof(float),
    .alignment = alignof(float),
    .hash_fn = &hash_reference_bytes,
};

type_t char_ptr_type_constant = {
    .name = "char*",
    .size = sizeof(char*),
    .alignment = alignof(char*),
    .hash_fn = &hash_string_reference,
    .compare_fn = &compare_string_references,
    .append_fn = &append_string_text,
};

type_t nil_type_constant = {
    .name = "nil",
    .size = 0,
    .alignment = 0,
    .hash_fn = &hash_reference_bytes,
};

type_t self_ptr_type_constant = {
    .name = "self*",
    .size = sizeof(uint64_t*),
    .alignment = alignof(uint64_t*),
};

// TODO(jawilson): more pointer types for the built in C types.
