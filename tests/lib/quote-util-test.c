///
/// Test C quoting helpers.
///

void test_simple_string_quoting(void) {
  test_assert_string_equal("\"Hello World!\"", quote_c_string("Hello World!"));
  test_assert_string_equal("\"\\n\"", quote_c_string("\n"));
  test_assert_string_equal("\"\\xff\"", quote_c_string("\xff"));
}
