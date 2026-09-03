void test_record_fields(void) {
  char*[] fields = { "a", "b", "c" };
  roci_record_fields_t* record_fields =
    make_roci_record_fields_from_names("myrecord", 3, fields);

  uint32_t slot = find_record_field_number(record_fields, roci_intern_as_symid("b"));
  test_assert_integer_equal(slot, 1);

  slot = find_record_field_number(record_fields, roci_intern_as_symid("c"));
  test_assert_integer_equal(slot, 2);
}
