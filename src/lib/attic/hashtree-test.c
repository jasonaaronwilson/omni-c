///
/// Test out some hashtree operations.
///

#include <stdlib.h>

#define C_ARMYKNIFE_LIB_IMPL
#include "../c-armyknife-lib.h"

void test_string_to_string_htree() {
  boolean_t was_added = false;
  char* never_in_tree_key = "never-in-tree-key";

  pointer_t(hashtree_t(char*, char*)) htree
      = make_empty_hashtree_node(char_ptr_type(), char_ptr_type());


  reference_t value_ref;

  char* key_1000 = "key1000";
  char* value_1000 = "value1000";

  // Check lookup in an empty tree
  value_ref = hashtree_get_reference_to_value(
      htree, char_ptr_type(), char_ptr_type(), 666,
      reference_of_char_ptr(&never_in_tree_key));
  if (value_ref.underlying_type != nil_type()) {
    ARMYKNIFE_TEST_FAIL("shouldn't be able to find our key in an empty tree");
  }

  // Now actually add an element to the tree
  was_added = hashtree_insert(htree, char_ptr_type(), char_ptr_type(), 1000,
                              reference_of_char_ptr(&key_1000),
                              reference_of_char_ptr(&value_1000));
  if (!was_added) {
    ARMYKNIFE_TEST_FAIL("hashtree_insert should have returned true");
  }

  // Check lookup for a key in the tree now that it is in there.
  value_ref
      = hashtree_get_reference_to_value(htree, char_ptr_type(), char_ptr_type(),
                                        1000, reference_of_char_ptr(&key_1000));
  if (value_ref.underlying_type == nil_type()) {
    ARMYKNIFE_TEST_FAIL("we should have found key_1000 in the tree");
  }
  if (dereference_char_ptr(value_ref) != value_1000) {
    // HERE: dereference_char_ptr(value_ref) is junky (but apprently
    // is the correct type)
    fprintf(stderr, "%s != %s", dereference_char_ptr(value_ref), value_1000);
    ARMYKNIFE_TEST_FAIL("the value for key_1000 should be value_1000");
  }

  // We still better not find the never_in_tree_key
  value_ref = hashtree_get_reference_to_value(
      htree, char_ptr_type(), char_ptr_type(), 666,
      reference_of_char_ptr(&never_in_tree_key));
  if (value_ref.underlying_type != nil_type()) {
    ARMYKNIFE_TEST_FAIL("shouldn't be able to find our key in an empty tree");
  }

  // Add something to the left of key1000
  char* key_500 = "key500";
  char* value_500 = "value500";
  was_added = hashtree_insert(htree, char_ptr_type(), char_ptr_type(), 500,
                              reference_of_char_ptr(&key_500),
                              reference_of_char_ptr(&value_500));

  if (!was_added) {
    ARMYKNIFE_TEST_FAIL("hashtree_insert should have returned true");
  }

  // Add something to the right of key1000
  char* key_1500 = "key1500";
  char* value_1500 = "value1500";
  was_added = hashtree_insert(htree, char_ptr_type(), char_ptr_type(), 1500,
                              reference_of_char_ptr(&key_1500),
                              reference_of_char_ptr(&value_1500));

  if (!was_added) {
    ARMYKNIFE_TEST_FAIL("hashtree_insert should have returned true");
  }
}

int main(int argc, char** argv) {
  test_string_to_string_htree();
  exit(0);
}
