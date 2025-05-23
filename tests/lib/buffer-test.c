//
// Test some operations on byte-arrays
//

void test_append_byte(void) {
  buffer_t* buffer = make_buffer(1);

  test_assert(buffer_length(buffer) == strlen(""));

  buffer = buffer_append_byte(buffer, 'a');
  buffer = buffer_append_byte(buffer, 'b');
  buffer = buffer_append_byte(buffer, 'c');
  buffer = buffer_append_byte(buffer, 'd');
  buffer = buffer_append_byte(buffer, 'e');
  buffer = buffer_append_byte(buffer, 'f');
  buffer = buffer_append_byte(buffer, 'g');

  char* contents = buffer_c_substring(buffer, 0, strlen("abcdefg"));
  test_assert(string_equal("abcdefg", contents));
  test_assert(buffer_length(buffer) == strlen("abcdefg"));
  test_assert(buffer_get(buffer, 2) == 'c');

  free_bytes(contents);
  free_bytes(buffer);
}

void test_append_string(void) {
  buffer_t* buffer = make_buffer(3);
  buffer = buffer_append_string(buffer, "Hello");
  buffer = buffer_append_string(buffer, " ");
  buffer = buffer_append_string(buffer, "");
  buffer = buffer_append_string(buffer, "World!");

  char* contents = buffer_to_c_string(buffer);
  test_assert(string_equal("Hello World!", contents));

  free_bytes(contents);
  free_bytes(buffer);
}

void test_buffer_from_string(void) {
  buffer_t* buffer = buffer_from_string("Hello World.");
  char* contents = buffer_to_c_string(buffer);
  test_assert(string_equal("Hello World.", contents));
  free_bytes(contents);
  free_bytes(buffer);
}

void test_buffer_c_substring(void) {
  buffer_t* buffer = make_buffer(3);
  buffer = buffer_append_string(buffer, "abcdefghijklmnop");
  test_assert(string_equal("", buffer_c_substring(buffer, 0, 0)));
  test_assert(string_equal("a", buffer_c_substring(buffer, 0, 1)));
  test_assert(string_equal("ab", buffer_c_substring(buffer, 0, 2)));
  test_assert(string_equal("abc", buffer_c_substring(buffer, 0, 3)));
  test_assert(string_equal("abcd", buffer_c_substring(buffer, 0, 4)));
  test_assert(string_equal("abcde", buffer_c_substring(buffer, 0, 5)));
}

void test_buffer_small_printf(void) {
  buffer_t* buffer = make_buffer(1);
  buffer = buffer_printf(buffer, "%d %s", 42, "OMG");
  char* contents = buffer_to_c_string(buffer);
  test_assert(string_equal("42 OMG", contents));
  free_bytes(contents);
  free_bytes(buffer);
}

void test_buffer_large_printf(void) {
  buffer_t* buffer = make_buffer(1);
  buffer = buffer_printf(buffer, "%d %s %d", 42, "A longer string", 24);
  char* contents = buffer_to_c_string(buffer);
  test_assert(string_equal("42 A longer string 24", contents));
  free_bytes(contents);
  free_bytes(buffer);
}

void test_buffer_medium_printf(void) {
  buffer_t* buffer = make_buffer(1);
  buffer = buffer_printf(buffer, "%d %s %d", 42, "--", 24);
  char* contents = buffer_to_c_string(buffer);
  test_assert(string_equal("42 -- 24", contents));
  free_bytes(contents);
  free_bytes(buffer);
}

void test_buffer_string_printf(void) {
  buffer_t* buffer = make_buffer(1);
  buffer = buffer_printf(buffer, "* [%s](%s)\n", "foobar.c", "foobar.c");
  char* contents = buffer_to_c_string(buffer);
  test_assert(string_equal("* [foobar.c](foobar.c)\n", contents));
  free_bytes(contents);
  free_bytes(buffer);
}

void test_buffer_utf8_decode(void) {
  buffer_t* buffer = make_buffer(1);
  buffer = buffer_printf(buffer, "AB");

  utf8_decode_result_t decode_result_0 = buffer_utf8_decode(buffer, 0);
  test_assert(decode_result_0.code_point == 'A');
  test_assert(decode_result_0.num_bytes == 1);
  test_assert(!decode_result_0.error);

  utf8_decode_result_t decode_result_1 = buffer_utf8_decode(buffer, 1);
  test_assert(decode_result_1.code_point == 'B');
  test_assert(decode_result_1.num_bytes == 1);
  test_assert(!decode_result_1.error);

  utf8_decode_result_t decode_result_2 = buffer_utf8_decode(buffer, 2);
  test_assert(decode_result_2.error);
}

void test_buffer_match_string_at(void) {
  buffer_t* buffer = make_buffer(1);
  buffer = buffer_printf(buffer, "ABCDEF");
  test_assert(buffer_match_string_at(buffer, 0, "ABC"));
  test_assert(!buffer_match_string_at(buffer, 0, "BC"));
  test_assert(buffer_match_string_at(buffer, 1, "BC"));
  test_assert(!buffer_match_string_at(buffer, 1, "ABCDEF"));
  test_assert(buffer_match_string_at(buffer, 0, "ABCDEF"));
  test_assert(buffer_match_string_at(buffer, 1, ""));
}

void test_buffer_adjust_region(void) {
  buffer_t* buffer = make_buffer(1);

  // No change
  buffer = make_buffer(1);
  buffer = buffer_printf(buffer, "012345");
  buffer = buffer_adjust_region(buffer, 1, 3, 2);
  test_assert_string_equal("012345", buffer_to_c_string(buffer));

  // Full deletion
  buffer = make_buffer(1);
  buffer = buffer_printf(buffer, "012345");
  buffer = buffer_adjust_region(buffer, 1, 3, 0);
  test_assert_string_equal("0345", buffer_to_c_string(buffer));

  // Partial deletion
  buffer = make_buffer(1);
  buffer = buffer_printf(buffer, "012345");
  buffer = buffer_adjust_region(buffer, 1, 3, 1);
  test_assert_string_equal("01345", buffer_to_c_string(buffer));

  // "Insertion"
  buffer = make_buffer(1);
  // POS 012345
  //      **
  //      *****
  // ASC 012345345
  buffer = buffer_printf(buffer, "012345");
  buffer = buffer_adjust_region(buffer, 1, 3, 5);
  test_assert_string_equal("012345345", buffer_to_c_string(buffer));
}

void test_buffer_replace_all(void) {
  buffer_t* buffer = make_buffer(1);

  buffer = make_buffer(1);
  buffer = buffer_printf(buffer, "012345");
  buffer = buffer_replace_all(buffer, "0", "000");
  test_assert_string_equal("00012345", buffer_to_c_string(buffer));

  buffer = make_buffer(1);
  buffer = buffer_printf(buffer, "0123405");
  buffer = buffer_replace_all(buffer, "0", "000");
  test_assert_string_equal("00012340005", buffer_to_c_string(buffer));

  buffer = make_buffer(1);
  buffer = buffer_printf(buffer, "0123450");
  buffer = buffer_replace_all(buffer, "0", "000");
  test_assert_string_equal("00012345000", buffer_to_c_string(buffer));

  buffer = make_buffer(1);
  buffer = buffer_replace_all(buffer, "0", "000");
  test_assert_string_equal("", buffer_to_c_string(buffer));
}

void test_buffer_region_contains(void) {
  buffer_t* buffer = make_buffer(1);

  buffer = make_buffer(1);
  buffer = buffer_printf(buffer, "012345");
  test_assert(buffer_region_contains(buffer, 0, 6, "23"));
  test_assert(!buffer_region_contains(buffer, 0, 6, "89"));
  test_assert(!buffer_region_contains(buffer, 0, 2, "23"));
}

void test_buffer_beginning_of_line(void) {
  buffer_t* buffer = make_buffer(20);
  buffer = buffer_append_string(buffer, "Line 1\nLine 2\n");

  // Test beginning of first line
  test_assert_integer_equal(0, buffer_beginning_of_line(buffer, 0));

  // Test middle of first line
  test_assert_integer_equal(0, buffer_beginning_of_line(buffer, 3));

  // Test start of second line
  test_assert_integer_equal(7, buffer_beginning_of_line(buffer, 7));

  // Test middle of second line
  test_assert_integer_equal(7, buffer_beginning_of_line(buffer, 10));
}

void test_buffer_end_of_line(void) {
  buffer_t* buffer = make_buffer(20);
  buffer = buffer_append_string(buffer, "Line 1\nLine 2\n");

  // Test end of first line
  test_assert(buffer_end_of_line(buffer, 0) == 6);

  // Test middle of first line
  test_assert(buffer_end_of_line(buffer, 3) == 6);

  // Test start of second line
  test_assert(buffer_end_of_line(buffer, 7) == 13);

  // Test end of buffer (no newline)
  test_assert(buffer_end_of_line(buffer, 14) == 14);
}

void test_buffer_append_buffer(void) {
  buffer_t* buffer1 = make_buffer(20);
  buffer1 = buffer_append_string(buffer1, "Line 1\nLine 2\n");

  buffer_t* buffer2 = make_buffer(20);
  buffer2 = buffer_append_string(buffer2, "Line 3\nLine 4\n");

  buffer1 = buffer_append_buffer(buffer1, buffer2);
  test_assert_string_equal("Line 1\nLine 2\nLine 3\nLine 4\n",
                           buffer_to_c_string(buffer1));
}

void test_buffer_append_sub_buffer(void) {
  buffer_t* buffer1 = make_buffer(20);
  buffer1 = buffer_append_string(buffer1, "Line 1\nLine 2\n");

  buffer_t* buffer2 = make_buffer(20);
  buffer2 = buffer_append_string(buffer2, "Line 3\nLine 4\n");

  buffer1 = buffer_append_sub_buffer(buffer1, 0, buffer2->length, buffer2);
  test_assert_string_equal("Line 1\nLine 2\nLine 3\nLine 4\n",
                           buffer_to_c_string(buffer1));
}

void test_buffer_to_uppercase(void) {
  buffer_t* buffer = buffer_append_string(make_buffer(5), "hello");
  buffer_to_uppercase(buffer);
  test_assert_string_equal("HELLO", buffer_to_c_string(buffer));
}

void test_buffer_to_lowercase(void) {
  buffer_t* buffer = buffer_append_string(make_buffer(5), "HELLO");
  buffer_to_lowercase(buffer);
  test_assert_string_equal("hello", buffer_to_c_string(buffer));
}

void test_buffer_replace_matching_byte(void) {
  buffer_t* buffer = buffer_append_string(make_buffer(5), "HELLO");
  buffer = buffer_replace_matching_byte(buffer, 'L', '_');
  test_assert_string_equal("HE__O", buffer_to_c_string(buffer));
}

void test_buffer_byte_source(void) {
  buffer_t* buffer = buffer_append_string(make_buffer(5), "Hi");
  byte_stream_source_t* source = buffer_to_byte_source(buffer);
  boolean_t got_byte = false;
  uint8_t first = source->read_byte(source, &got_byte);
  test_assert(got_byte);
  test_assert_integer_equal(first, 'H');
  uint8_t second = source->read_byte(source, &got_byte);
  test_assert(got_byte);
  test_assert_integer_equal(second, 'i');
  uint8_t third = source->read_byte(source, &got_byte);
  test_assert(got_byte == false);
  test_assert_integer_equal(third, 0);
}

void test_buffer_byte_target(void) {
  buffer_t* buffer = make_buffer(5);
  byte_stream_target_t* target = buffer_to_byte_target(buffer);
  target->write_byte(target, 'H');
  target->write_byte(target, 'i');
  test_assert_string_equal("Hi", buffer_to_c_string(buffer));
}
