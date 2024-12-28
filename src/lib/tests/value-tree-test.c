//
/// Test string-tree.c
///

#include <stdio.h>
#include <stdlib.h>

#define C_ARMYKNIFE_LIB_IMPL
#include "../c-armyknife-lib.h"

void test_tree() {
  value_tree_t* tree = NULL;
  value_result_t value;

  value = value_tree_find(tree, cmp_string_values, str_to_value("a"));
  test_assert(is_not_ok(value));

  tree = value_tree_insert(tree, cmp_string_values, str_to_value("a"),
                           str_to_value("A"));
  value = value_tree_find(tree, cmp_string_values, str_to_value("a"));
  test_assert(is_ok(value) && string_equal("A", value.str));

  tree = value_tree_insert(tree, cmp_string_values, str_to_value("b"),
                           str_to_value("B"));
  value = value_tree_find(tree, cmp_string_values, str_to_value("a"));
  test_assert(is_ok(value) && string_equal("A", value.str));
  value = value_tree_find(tree, cmp_string_values, str_to_value("b"));
  test_assert(is_ok(value) && string_equal("B", value.str));

  // Add a few more elements
  tree = value_tree_insert(tree, cmp_string_values, str_to_value("c"),
                           str_to_value("C"));
  tree = value_tree_insert(tree, cmp_string_values, str_to_value("d"),
                           str_to_value("D"));

  // Finally delete a node.
  tree = value_tree_delete(tree, cmp_string_values, str_to_value("b"));
  value = value_tree_find(tree, cmp_string_values, str_to_value("b"));
  test_assert(is_not_ok(value));

  value = value_tree_find(tree, cmp_string_values, str_to_value("a"));
  test_assert(is_ok(value) && string_equal("A", value.str));

  value = value_tree_find(tree, cmp_string_values, str_to_value("c"));
  test_assert(is_ok(value) && string_equal("C", value.str));

  tree = value_tree_insert(tree, cmp_string_values, str_to_value("e"),
                           str_to_value("E"));
  tree = value_tree_insert(tree, cmp_string_values, str_to_value("f"),
                           str_to_value("F"));
  tree = value_tree_insert(tree, cmp_string_values, str_to_value("g"),
                           str_to_value("G"));
  tree = value_tree_insert(tree, cmp_string_values, str_to_value("h"),
                           str_to_value("H"));
  tree = value_tree_insert(tree, cmp_string_values, str_to_value("i"),
                           str_to_value("i"));
  tree = value_tree_insert(tree, cmp_string_values, str_to_value("j"),
                           str_to_value("j"));

  // clang-format off
  value_tree_foreach(tree, key, value, {
      fprintf(stderr, "key=%s value = %s\n", key.str, value.str);
  });
  // clang-format on
}

void check_values(value_tree_t* t) {
  if (t) {
    char* value_to_key_string = uint64_to_string((t->value).u64);
    test_assert(string_equal(t->key.str, value_to_key_string));
    check_values(t->left);
    check_values(t->right);
  }
}

int main(int argc, char** argv) {
  test_tree();
  exit(0);
}
