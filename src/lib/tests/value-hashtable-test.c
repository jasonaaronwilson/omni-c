///
/// Test value-hashtable.c
///

#include <stdlib.h>

#define C_ARMYKNIFE_LIB_IMPL
#include "../c-armyknife-lib.h"

uint64_t hash_value(value_t value1) { return string_hash(value1.str); }

int cmp_values(value_t value1, value_t value2) {
  return strcmp(value1.str, value2.str);
}

void test_value_ht() {
  value_hashtable_t* ht = make_value_hashtable(2);
  value_result_t value;

  value = value_ht_find(ht, hash_values, cmp_values, str_to_value("a"));
  test_assert(is_not_ok(value));
  test_assert(value_ht_num_entries(ht) == 0);

  ht = value_ht_insert(ht, hash_value, cmp_values, str_to_value("a"),
                       str_to_value("A"));
  value = value_ht_find(ht, str_to_value("a"));
  test_assert(is_ok(value) && string_equal("A", value.str));
  test_assert(value_ht_num_entries(ht) == 1);

  ht = value_ht_insert(ht, hash_value, cmp_values, str_to_value("b"),
                       str_to_value("B"));
  test_assert(value_ht_num_entries(ht) == 2);
  value = value_ht_find(ht, hash_value, cmp_values, str_to_value("a"));
  test_assert(is_ok(value) && string_equal("A", value.str));
  value = value_ht_find(ht, hash_value, cmp_values, str_to_value("b"));
  test_assert(is_ok(value) && string_equal("B", value.str));

  // Add a few more elements to the front of the ht.
  ht = value_ht_insert(ht, hash_value, cmp_values, str_to_value("c"),
                       str_to_value("C"));
  ht = value_ht_insert(ht, hash_value, cmp_values, str_to_value("d"),
                       str_to_value("D"));
  test_assert(value_ht_num_entries(ht) == 4);

  // Finally delete a node.
  ht = value_ht_delete(ht, hash_value, cmp_values, str_to_value("b"));
  value = value_ht_find(ht, hash_value, cmp_values, str_to_value("b"));
  test_assert(is_not_ok(value));
  test_assert(value_ht_num_entries(ht) == 3);

  value = value_ht_find(ht, hash_value, cmp_values, str_to_value("a"));
  test_assert(is_ok(value) && string_equal("A", value.str));

  value = value_ht_find(ht, hash_value, cmp_values, str_to_value("c"));
  test_assert(is_ok(value) && string_equal("C", value.str));

  // clang-format off
  value_ht_foreach(ht, key, value, { 
      fprintf(stderr, "key=%s value = %s\n", key.str, value.str);
  });
  // clang-format on
}

void test_ht_random() {
  value_hashtable_t* ht = make_value_hashtable(3);

  int iterations = 1000;
  random_state_t state = random_state_for_test();
  for (int i = 0; i < iterations; i++) {
    uint64_t next = random_next(&state);
    ht = value_ht_insert(ht, hash_value, cmp_values,
                         str_to_value(uint64_to_string(next)),
                         u64_to_value(next));
  }

  // Now make sure everything we just added is found (sort of redudant
  // with the final check but catching errors early already bisects a
  // failing test and nobody hates that!)
  state = random_state_for_test();
  for (int i = 0; i < iterations; i++) {
    uint64_t next = random_next(&state);
    test_assert(is_ok(value_ht_find(ht, hash_value, cmp_values,
                                    str_to_value(uint64_to_string(next)))));
  }

  // Now delete a bunch of things
  state = random_state_for_test();
  for (int i = 0; i < iterations; i++) {
    uint64_t next = random_next(&state);
    if ((next & 3) == 0) {
      ht = value_ht_delete(ht, hash_value, cmp_values,
                           str_to_value(uint64_to_string(next)));
    }
  }

  // Make sure everything inserted is found and everything deleted is
  // not found.
  state = random_state_for_test();
  for (int i = 0; i < iterations; i++) {
    uint64_t next = random_next(&state);
    if ((next & 3) == 0) {
      test_assert(is_not_ok(value_ht_find(ht, hash_value, cmp_values, str_to_value(uint64_to_string(next))));
    } else {
      test_assert(is_ok(value_ht_find(ht, hash_value, cmp_values,
                                      str_to_value(uint64_to_string(next)))));
    }
  }

  log_test("The final bucket count of the hashtable is %lu", ht->n_buckets);
}

int main(int argc, char** argv) {
  test_value_ht();
  test_ht_random();
  exit(0);
}
