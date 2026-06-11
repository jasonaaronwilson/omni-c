///
/// Test C quoting helpers.
///

void test_simple_string_quoting(void) {
  test_assert_string_equal("\"Hello World!\"", quote_c_string("Hello World!"));
  test_assert_string_equal("\"\\n\"", quote_c_string("\n"));
  test_assert_string_equal("\"\\xff\"", quote_c_string("\xff"));
}

void test_simple_string_unquoting(void) {
  test_assert_string_equal("Hello World!", string_unquote_c_string("\"Hello World!\""));
  test_assert_string_equal("\n", string_unquote_c_string("\"\\n\""));
  test_assert_string_equal("\t", string_unquote_c_string("\"\\t\""));
  test_assert_string_equal("\r", string_unquote_c_string("\"\\r\""));
  test_assert_string_equal("\xfe", string_unquote_c_string("\"\\xfe\""));
}


