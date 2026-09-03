void test_symbol_table(void) {
  roci_symid_t symid = 0;

  char* interned = roci_intern_symbol("hello", &symid);
  char* interned_hello = interned;
  test_assert_integer_equal(symid, symid_offset());
  test_assert_string_equal("hello", interned);
  test_assert_string_equal("hello", roci_symid_to_string(symid));

  interned = roci_intern_symbol("hello", &symid);
  test_assert_integer_equal(symid, symid_offset());
  if (interned_hello != interned) {
    test_fail("got non == result back");
  }
  test_assert_string_equal("hello", interned);
  test_assert_string_equal("hello", roci_symid_to_string(symid));

  interned = roci_intern_symbol("world", &symid);
  test_assert_integer_equal(symid, symid_offset() + 1);
  test_assert_string_equal("world", interned);
  test_assert_string_equal("world", roci_symid_to_string(symid));

  interned = roci_intern_symbol("world", nullptr);
  test_assert_string_equal("world", interned);

  interned = roci_intern_as_string("world");
  roci_symid_t id = roci_intern_as_symid("world");
  test_assert_integer_equal(symid, symid_offset() + 1);
}
