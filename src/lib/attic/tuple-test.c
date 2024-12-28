///
/// Test some operations on "tuples".
///

#include <stdlib.h>

#define C_ARMYKNIFE_LIB_IMPL
#include "../c-armyknife-lib.h"

void test_writing_in_reverse() {
  type_t* type
      = intern_tuple_type(3, uint64_type(), uint64_type(), uint64_type());
  reference_t tuple_ref = reference_of(type, malloc_bytes(type->size));
  uint64_t big_number = 0xdeadbeafcafebeed;
  tuple_write_element(tuple_ref, 2, reference_of_uint64(&big_number));
  tuple_write_element(tuple_ref, 1, reference_of_uint64(&big_number));
  tuple_write_element(tuple_ref, 0, reference_of_uint64(&big_number));
  reference_t element;
  element = tuple_reference_of_element(tuple_ref, 2);
  if (dereference_uint64(element) != big_number) {
    ARMYKNIFE_TEST_FAIL("element got overwritten?");
  }
  element = tuple_reference_of_element(tuple_ref, 1);
  if (dereference_uint64(element) != big_number) {
    ARMYKNIFE_TEST_FAIL("element got overwritten?");
  }
  element = tuple_reference_of_element(tuple_ref, 0);
  if (dereference_uint64(element) != big_number) {
    ARMYKNIFE_TEST_FAIL("element got overwritten?");
  }
}

void test_sizes() {
  type_t* type = intern_tuple_type(1, uint64_type());
  if (type->size != 8) {
    ARMYKNIFE_TEST_FAIL("tuple_t(uint64_t) size");
  }
  if (type->alignment != 8) {
    ARMYKNIFE_TEST_FAIL("tuple_t(uint64_t) alignment");
  }

  type = intern_tuple_type(1, uint32_type());
  if (type->size != 4) {
    ARMYKNIFE_TEST_FAIL("tuple_t(uint32_t) size");
  }
  if (type->alignment != 4) {
    ARMYKNIFE_TEST_FAIL("tuple_t(uint32_t) alignment");
  }

  // This ordering means we have to "pad" before the uint64 hence the
  // size of the tuple has increased.
  type = intern_tuple_type(2, uint32_type(), uint64_type());
  if (type->size != 16) {
    ARMYKNIFE_TEST_FAIL("tuple_t(uint32_t, uint64_t) size");
  }
  if (type->alignment != 8) {
    ARMYKNIFE_TEST_FAIL("tuple_t(uint64_t) alignment");
  }

  type = intern_tuple_type(3, uint64_type(), uint32_type(), uint64_type());
  if (type->size != 24) {
    ARMYKNIFE_TEST_FAIL("tuple_t(uint64_t, uint32_t, uint64_t) size");
  }
  if (type->alignment != 8) {
    ARMYKNIFE_TEST_FAIL("tuple_t(uint64_t) alignment");
  }
}

void test_offsets() {
  uint64_t stack_tuple_space[16];
  type_t* type;
  reference_t tuple_reference;
  reference_t field0_reference;
  reference_t field1_reference;
  reference_t field2_reference;

  memset(stack_tuple_space, 0, sizeof(stack_tuple_space));
  type = intern_tuple_type(1, uint64_type());
  tuple_reference = reference_of(type, &stack_tuple_space);
  field0_reference = tuple_reference_of_element(tuple_reference, 0);
  uint64_t field_value = dereference_uint64(field0_reference);
  if (field_value != 0) {
    ARMYKNIFE_TEST_FAIL("tuple_t(uint64_t)");
  }
  write_to_uint64_reference(field0_reference, 0xffffffffffffffff);
  field_value = dereference_uint64(field0_reference);
  if (field_value != 0xffffffffffffffff) {
    ARMYKNIFE_TEST_FAIL("tuple_t(uint64_t)");
  }
  if (stack_tuple_space[1] != 0) {
    ARMYKNIFE_TEST_FAIL("tuple_t(uint64_t)");
  }

  memset(stack_tuple_space, 0, sizeof(stack_tuple_space));
  stack_tuple_space[0] = 1000;
  stack_tuple_space[1] = 1001;
  type = intern_tuple_type(2, uint64_type(), uint64_type());
  tuple_reference = reference_of(type, &stack_tuple_space);
  field0_reference = tuple_reference_of_element(tuple_reference, 0);
  field1_reference = tuple_reference_of_element(tuple_reference, 1);
  field_value = dereference_uint64(field0_reference);
  if (field_value != 1000) {
    ARMYKNIFE_TEST_FAIL("tuple_t(uint64_t,uint64_t)");
  }
  field_value = dereference_uint64(field1_reference);
  if (field_value != 1001) {
    ARMYKNIFE_TEST_FAIL("tuple_t(uint64_t,uint64_t)");
  }

  memset(stack_tuple_space, 0, sizeof(stack_tuple_space));
  stack_tuple_space[0] = 1000;
  stack_tuple_space[1] = 0;
  stack_tuple_space[2] = 1002;
  type = intern_tuple_type(3, uint64_type(), uint32_type(), uint64_type());
  tuple_reference = reference_of(type, &stack_tuple_space);
  field0_reference = tuple_reference_of_element(tuple_reference, 0);
  field1_reference = tuple_reference_of_element(tuple_reference, 1);
  field2_reference = tuple_reference_of_element(tuple_reference, 2);
  field_value = dereference_uint64(field0_reference);
  if (field_value != 1000) {
    ARMYKNIFE_TEST_FAIL("tuple_t(uint64_t,uint32_t,uint64_t)");
  }

  write_to_uint32_reference(field1_reference, 0xffffffff);
  uint32_t field_value32 = dereference_uint32(field1_reference);
  if (field_value32 != 0xffffffff) {
    ARMYKNIFE_TEST_FAIL("tuple_t(uint64_t,uint32_t,uint64_t)");
  }
  field_value = dereference_uint64(field2_reference);
  if (field_value != 1002) {
    ARMYKNIFE_TEST_FAIL("tuple_t(uint64_t,uint32_t,uint64_t)");
  }
}

void test_string_tuple() {
  type_t* type = intern_tuple_type(2, char_ptr_type(), char_ptr_type());
  reference_t tuple_ref = reference_of(type, malloc_bytes(type->size));
  tuple_write_element(tuple_ref, 0, reference_of(char_ptr_type(), "Hello"));
  tuple_write_element(tuple_ref, 1, reference_of(char_ptr_type(), "World"));
  reference_t element_0 = tuple_reference_of_element(tuple_ref, 0);
  if (strcmp((char*) element_0.pointer, "Hello") != 0) {
    ARMYKNIFE_TEST_FAIL("first element should be Hello");
  }
  // reference_t element_1 = tuple_reference_of_element(tuple_ref, 0);
}

int main(int argc, char** argv) {
  test_sizes();
  test_offsets();
  test_writing_in_reverse();
  exit(0);
}
