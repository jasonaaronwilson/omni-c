///
/// Make an array of uint64 and add 16 elements to it.
///

#include <stdlib.h>

#define C_ARMYKNIFE_LIB_IMPL
#include "../c-armyknife-lib.h"

void test_array_uint64() {
  array_t(uint64_t)* array = make_array(uint64_type(), 1);
  for (int i = 0; i < 16; i++) {
    uint64_t value = i + 100;
    array = array_add(array, reference_of(uint64_type(), &value));
  }

  for (int i = 0; i < 16; i++) {
    uint64_t value = i + 100;
    uint64_t element = dereference_uint64(array_get_reference(array, i));
    if (element != value) {
      fprintf(stderr, "%lu != %lu\n", value, element);
      ARMYKNIFE_TEST_FAIL("array_get_reference");
    }
  }
}

void test_array_uint8() {
  uint64_t start_bytes = number_of_bytes_allocated;

  array_t(uint8_t)* array = make_array(uint8_type(), 16);
  for (int i = 0; i < 16; i++) {
    uint8_t value = i + 100;
    array = array_add(array, reference_of(uint8_type(), &value));
  }

  for (int i = 0; i < 16; i++) {
    uint8_t value = i + 100;
    uint8_t element = dereference_uint8(array_get_reference(array, i));
    if (element != value) {
      fprintf(stderr, "%u != %u\n", value, element);
      ARMYKNIFE_TEST_FAIL("array_get_reference");
    }
  }

  uint64_t end_bytes = number_of_bytes_allocated;
  if ((end_bytes - start_bytes) != 16 + sizeof(struct array_S)) {
    ARMYKNIFE_TEST_FAIL("unexpected reallocation or over allocation ");
  }
}

int main(int argc, char** argv) {
  test_array_uint64();
  test_array_uint8();
  exit(0);
}
