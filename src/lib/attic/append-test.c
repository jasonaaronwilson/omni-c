///
/// Testing appending references of things to a byte array.
///

#include <stdlib.h>

#define C_ARMYKNIFE_LIB_IMPL
#include "../c-armyknife-lib.h"

void test_append_uint64() {
  buffer_t* buffer = make_buffer(16);

  uint64_t number = 123456789;
  reference_t number_ref = reference_of_uint64(&number);
  buffer = buffer_append_reference(buffer, number_ref);
  char* str = buffer_to_c_string(buffer);
  free(buffer);
  if (strcmp(str, "123456789") != 0) {
    ARMYKNIFE_TEST_FAIL("expected 123456789");
  }
}

void test_append_uint32() {
  buffer_t* buffer = make_buffer(16);

  uint32_t number = 123456789;
  reference_t number_ref = reference_of_uint32(&number);
  buffer = buffer_append_reference(buffer, number_ref);
  char* str = buffer_to_c_string(buffer);
  free(buffer);
  if (strcmp(str, "123456789") != 0) {
    ARMYKNIFE_TEST_FAIL("expected 123456789");
  }
}

void test_append_uint16() {
  buffer_t* buffer = make_buffer(16);

  uint16_t number = 50000;
  reference_t number_ref = reference_of_uint16(&number);
  buffer = buffer_append_reference(buffer, number_ref);
  char* str = buffer_to_c_string(buffer);
  free(buffer);
  if (strcmp(str, "50000") != 0) {
    ARMYKNIFE_TEST_FAIL("expected 50000");
  }
}

void test_append_uint8() {
  buffer_t* buffer = make_buffer(16);

  uint8_t number = 200;
  reference_t number_ref = reference_of_uint8(&number);
  buffer = buffer_append_reference(buffer, number_ref);
  char* str = buffer_to_c_string(buffer);
  free(buffer);
  if (strcmp(str, "200") != 0) {
    ARMYKNIFE_TEST_FAIL("expected 200");
  }
}

void test_append_int64() {
  buffer_t* buffer = make_buffer(16);

  int64_t number = -123456789;
  reference_t number_ref = reference_of_int64(&number);
  buffer = buffer_append_reference(buffer, number_ref);
  char* str = buffer_to_c_string(buffer);
  free(buffer);
  if (strcmp(str, "-123456789") != 0) {
    ARMYKNIFE_TEST_FAIL("expected -123456789");
  }
}

void test_append_int32() {
  buffer_t* buffer = make_buffer(16);

  int32_t number = -123456789;
  reference_t number_ref = reference_of_int32(&number);
  buffer = buffer_append_reference(buffer, number_ref);
  char* str = buffer_to_c_string(buffer);
  free(buffer);
  if (strcmp(str, "-123456789") != 0) {
    ARMYKNIFE_TEST_FAIL("expected -123456789");
  }
}

void test_append_int16() {
  buffer_t* buffer = make_buffer(16);

  int32_t number = -30000;
  reference_t number_ref = reference_of_int32(&number);
  buffer = buffer_append_reference(buffer, number_ref);
  char* str = buffer_to_c_string(buffer);
  free(buffer);
  if (strcmp(str, "-30000") != 0) {
    ARMYKNIFE_TEST_FAIL("expected -30000");
  }
}

void test_append_int8() {
  buffer_t* buffer = make_buffer(16);

  int8_t number = -100;
  reference_t number_ref = reference_of_int8(&number);
  buffer = buffer_append_reference(buffer, number_ref);
  char* str = buffer_to_c_string(buffer);
  free(buffer);
  if (strcmp(str, "-100") != 0) {
    fprintf(stderr, "%s\n", str);
    ARMYKNIFE_TEST_FAIL("expected -100");
  }
}

void test_append_string() {
  buffer_t* buffer = make_buffer(16);

  char* input_str = "Hello World!";
  char* expected_str = "\"Hello World!\"";

  reference_t str_ref = reference_of_char_ptr(&input_str);
  buffer = buffer_append_reference(buffer, str_ref);
  char* str = buffer_to_c_string(buffer);
  free(buffer);

  if (strcmp(expected_str, str) != 0) {
    fprintf(stderr, "%s", str);
    ARMYKNIFE_TEST_FAIL("expected Hello World!");
  }
}

int main(int argc, char** argv) {

  test_append_uint64();
  test_append_uint32();
  test_append_uint16();
  test_append_uint8();

  test_append_int64();
  test_append_int32();
  test_append_int16();
  test_append_int8();

  test_append_string();

  exit(0);
}
