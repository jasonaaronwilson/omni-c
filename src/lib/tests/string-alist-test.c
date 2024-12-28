//
/// Test string-alist.c
///

#include <stdio.h>
#include <stdlib.h>

#define C_ARMYKNIFE_LIB_IMPL
#include "../c-armyknife-lib.h"

void test_alist() {
  string_alist_t* list = NULL;
  value_result_t value;

  value = alist_find(list, "a");
  test_assert(is_not_ok(value));
  test_assert(alist_length(list) == 0);

  list = alist_insert(list, "a", str_to_value("A"));
  value = alist_find(list, "a");
  test_assert(is_ok(value) && string_equal("A", value.str));
  test_assert(alist_length(list) == 1);

  list = alist_insert(list, "b", str_to_value("B"));
  value = alist_find(list, "a");
  test_assert(is_ok(value) && string_equal("A", value.str));
  value = alist_find(list, "b");
  test_assert(is_ok(value) && string_equal("B", value.str));
  test_assert(alist_length(list) == 2);

  // Add a few more elements to the front of the list.
  list = alist_insert(list, "c", str_to_value("C"));
  list = alist_insert(list, "d", str_to_value("D"));

  // Finally delete a node.
  list = alist_delete(list, "b");
  value = alist_find(list, "b");
  test_assert(is_not_ok(value));

  value = alist_find(list, "a");
  test_assert(is_ok(value) && string_equal("A", value.str));
  value = alist_find(list, "c");
  test_assert(is_ok(value) && string_equal("C", value.str));

  // clang-format off
  string_alist_foreach(list, key, value, { 
      fprintf(stderr, "key=%s value = %s\n", key, value.str);
  });
  // clang-format on
}

void test_alist_random() {
  string_alist_t* list = NULL;

  int iterations = 1000;
  random_state_t state = random_state_for_test();
  for (int i = 0; i < iterations; i++) {
    uint64_t next = random_next(&state);
    list = alist_insert(list, uint64_to_string(next), u64_to_value(next));
  }

  // Now delete a bunch of things
  state = random_state_for_test();
  for (int i = 0; i < iterations; i++) {
    uint64_t next = random_next(&state);
    if ((next & 3) == 0) {
      list = alist_delete(list, uint64_to_string(next));
    }
  }

  // Make sure everything deleted is not found
  state = random_state_for_test();
  for (int i = 0; i < iterations; i++) {
    uint64_t next = random_next(&state);
    if ((next & 3) == 0) {
      test_assert(is_not_ok(alist_find(list, uint64_to_string(next))));
    } else {
      test_assert(is_ok(alist_find(list, uint64_to_string(next))));
    }
  }
}

int main(int argc, char** argv) {
  test_alist();
  test_alist_random();
  exit(0);
}
