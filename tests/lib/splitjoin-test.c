//
// Test split/join operations
//

void test_join(void) {
  value_array_t* array = make_value_array(1);
  value_array_add(array, str_to_value("a"));
  value_array_add(array, str_to_value("b"));
  value_array_add(array, str_to_value("c"));
  buffer_t* joined = join_array_of_strings(array, ", ");

  test_assert(string_equal("a, b, c", buffer_to_c_string(joined)));
}
