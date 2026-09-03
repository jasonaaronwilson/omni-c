void test_symbol_table(void) {
  roci_symid_t symid = 0;
  char* interned = roci_intern_symbol("hello", &symid);
  test_assert_integer_equal(symid, symid_offset());
  test_assert_string_equal("hello", interned);
  test_assert_string_equal("hello", roci_symid_to_string(symid));

  interned = roci_intern_symbol("hello", &symid);
  test_assert_integer_equal(symid, symid_offset());
  test_assert_string_equal("hello", interned);
  test_assert_string_equal("hello", roci_symid_to_string(symid));
}
