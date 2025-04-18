//
/// Test value-alist.c
///

void test_value_alist(void) {
  value_alist_t* list = NULL;
  value_result_t value = {0};

  value = value_alist_find(list, cmp_string_values, str_to_value("a"));
  test_assert(is_not_ok(value));
  test_assert(value_alist_length(list) == 0);

  list = value_alist_insert(list, cmp_string_values, str_to_value("a"),
                            str_to_value("A"));
  value = value_alist_find(list, cmp_string_values, str_to_value("a"));
  test_assert(is_ok(value) && string_equal("A", value.str));
  test_assert(value_alist_length(list) == 1);

  list = value_alist_insert(list, cmp_string_values, str_to_value("b"),
                            str_to_value("B"));
  value = value_alist_find(list, cmp_string_values, str_to_value("a"));
  test_assert(is_ok(value) && string_equal("A", value.str));
  value = value_alist_find(list, cmp_string_values, str_to_value("b"));
  test_assert(is_ok(value) && string_equal("B", value.str));
  test_assert(value_alist_length(list) == 2);

  // Add a few more elements to the front of the list.
  list = value_alist_insert(list, cmp_string_values, str_to_value("c"),
                            str_to_value("C"));
  list = value_alist_insert(list, cmp_string_values, str_to_value("d"),
                            str_to_value("D"));

  // Finally delete a node.
  list = value_alist_delete(list, cmp_string_values, str_to_value("b"));
  value = value_alist_find(list, cmp_string_values, str_to_value("b"));
  test_assert(is_not_ok(value));

  value = value_alist_find(list, cmp_string_values, str_to_value("a"));
  test_assert(is_ok(value) && string_equal("A", value.str));
  value = value_alist_find(list, cmp_string_values, str_to_value("c"));
  test_assert(is_ok(value) && string_equal("C", value.str));

  // clang-format off
  value_alist_foreach(list, key, value, block_expr({ 
      fprintf(stderr, "key=%s value = %s\n", key.str, value.str);
      }));
  // clang-format on
}

void test_value_alist_random(void) {
  value_alist_t* list = NULL;

  int iterations = 1000;
  random_state_t state = random_state_for_test();
  for (int i = 0; i < iterations; i++) {
    uint64_t next = random_next(&state);
    list = value_alist_insert(list, cmp_string_values,
                              str_to_value(uint64_to_string(next)),
                              u64_to_value(next));
  }

  // Now delete a bunch of things
  state = random_state_for_test();
  for (int i = 0; i < iterations; i++) {
    uint64_t next = random_next(&state);
    if ((next & 3) == 0) {
      list = value_alist_delete(list, cmp_string_values,
                                str_to_value(uint64_to_string(next)));
    }
  }

  // Make sure everything deleted is not found
  state = random_state_for_test();
  for (int i = 0; i < iterations; i++) {
    uint64_t next = random_next(&state);
    if ((next & 3) == 0) {
      test_assert(is_not_ok(value_alist_find(
          list, cmp_string_values, str_to_value(uint64_to_string(next)))));
    } else {
      test_assert(is_ok(value_alist_find(
          list, cmp_string_values, str_to_value(uint64_to_string(next)))));
    }
  }
}
