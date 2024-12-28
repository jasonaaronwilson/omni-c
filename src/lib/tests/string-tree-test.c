//
/// Test string-tree.c
///

#include <stdio.h>
#include <stdlib.h>

#define C_ARMYKNIFE_LIB_IMPL
#include "../c-armyknife-lib.h"

void test_tree() {
  string_tree_t* tree = NULL;
  value_result_t value;

  value = string_tree_find(tree, "a");
  test_assert(is_not_ok(value));

  tree = string_tree_insert(tree, "a", str_to_value("A"));
  value = string_tree_find(tree, "a");
  test_assert(is_ok(value) && string_equal("A", value.str));

  tree = string_tree_insert(tree, "b", str_to_value("B"));
  value = string_tree_find(tree, "a");
  test_assert(is_ok(value) && string_equal("A", value.str));
  value = string_tree_find(tree, "b");
  test_assert(is_ok(value) && string_equal("B", value.str));

  // Add a few more elements
  tree = string_tree_insert(tree, "c", str_to_value("C"));
  tree = string_tree_insert(tree, "d", str_to_value("D"));

  // Finally delete a node.
  tree = string_tree_delete(tree, "b");
  value = string_tree_find(tree, "b");
  test_assert(is_not_ok(value));

  value = string_tree_find(tree, "a");
  test_assert(is_ok(value) && string_equal("A", value.str));

  value = string_tree_find(tree, "c");
  test_assert(is_ok(value) && string_equal("C", value.str));

  tree = string_tree_insert(tree, "e", str_to_value("E"));
  tree = string_tree_insert(tree, "f", str_to_value("F"));
  tree = string_tree_insert(tree, "g", str_to_value("G"));
  tree = string_tree_insert(tree, "h", str_to_value("H"));
  tree = string_tree_insert(tree, "i", str_to_value("i"));
  tree = string_tree_insert(tree, "j", str_to_value("j"));

  // clang-format off
  string_tree_foreach(tree, key, value, {
      fprintf(stderr, "key=%s value = %s\n", key, value.str);
  });
  // clang-format on
}

void check_values(value_tree_t* t) {
  if (t) {
    test_assert_string_equal(t->key.str, uint64_to_string(t->value.u64));
    check_values(t->left);
    check_values(t->right);
  }
}

void check_sequence(char* file, int line, string_tree_t* tree,
                    string_hashtable_t* ht, int index) {

  random_state_t state = random_state_for_test();
  for (int i = 0; i < index; i++) {
    uint64_t next = random_next(&state);
    char* key_string = uint64_to_string(next);

    value_result_t lookup_result = string_tree_find(tree, key_string);
    value_result_t lookup_result_reference = string_ht_find(ht, key_string);

    if (lookup_result.nf_error != lookup_result_reference.nf_error) {
      fprintf(stdout,
              "CHECK SEQUENCE %s:%d: tree and hashtable reference are not in "
              "agreement!",
              file, line);
      test_fail("check sequence failure");
    }
  }
}

void test_random_insertion_and_deletion() {
  int iterations = 10000;

  string_tree_t* tree = NULL;
  string_hashtable_t* ht = make_string_hashtable(100);

  random_state_t state = random_state_for_test();
  for (int i = 0; i < iterations; i++) {
    uint64_t next = random_next(&state);
    char* key_string = uint64_to_string(next);
    tree = string_tree_insert(tree, key_string, u64_to_value(next));
    ht = string_ht_insert(ht, key_string, u64_to_value(next));
    if (i < 1000) {
      check_sequence(__FILE__, __LINE__, tree, ht, i);
    }
  }

  check_sequence(__FILE__, __LINE__, tree, ht, iterations);
  check_values(cast(value_tree_t*, tree));

  // TODO(jawilson): delete 1/4 of the keys and repeat above checks.
  state = random_state_for_test();
  for (int i = 0; i < iterations; i++) {
    uint64_t next = random_next(&state);
    if ((next & 3) == 0) {
      tree = string_tree_delete(tree, uint64_to_string(next));
      ht = string_ht_delete(ht, uint64_to_string(next));
      if (i < 1000) {
        check_sequence(__FILE__, __LINE__, tree, ht, i);
      }
    } else {
      if (i < 1000) {
        check_sequence(__FILE__, __LINE__, tree, ht, i);
      }
    }
  }

  check_sequence(__FILE__, __LINE__, tree, ht, iterations);

  // Check we have just the correct keys by comparing to the reference
  // implementation
  state = random_state_for_test();
  for (int i = 0; i < iterations; i++) {
    uint64_t next = random_next(&state);
    char* key = uint64_to_string(next);
    value_result_t lookup_result = string_tree_find(tree, key);
    value_result_t lookup_result_reference = string_ht_find(ht, key);

    if (lookup_result.nf_error != lookup_result_reference.nf_error) {
      test_fail("tree and hashtable reference are not in agreement!");
    }

    if ((next & 3) == 0) {
      test_assert(is_not_ok(lookup_result));
    } else {
      test_assert(is_ok(lookup_result));
    }
  }
}

int main(int argc, char** argv) {
  test_tree();
  test_random_insertion_and_deletion();
  exit(0);
}
