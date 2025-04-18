///
/// Test the additional string utilities.
///

// Tell the library to use a smaller initial buffer size so that it is
// easier to test with small inputs but it turns out that 8 is a bad
// size because sizeof(array_name) doesn't work anywhere near expected
// and may be returning the sizeof a pointer which just happens to be
// 8 masking not finding bugs.

// #define STRING_PRINTF_INITIAL_BUFFER_SIZE 16

#define TEST_VALID_UTF8_SEQUENCE(cp, n_bytes, ...)                             \
  do {                                                                         \
    uint8_t utf8_bytes[] = {__VA_ARGS__};                                      \
    utf8_decode_result_t result = utf8_decode(utf8_bytes);                     \
    test_assert(result.error == false);                                        \
    test_assert(result.num_bytes == n_bytes);                                  \
    test_assert(result.code_point == cp);                                      \
    buffer_t* buffer = make_buffer(8);                                         \
    buffer = buffer_append_code_point(buffer, cp);                             \
    test_assert(buffer->length == n_bytes);                                    \
    test_assert(buffer_get(buffer, 0) == utf8_bytes[0]);                       \
    if (n_bytes > 1) {                                                         \
      test_assert(buffer_get(buffer, 1) == utf8_bytes[1]);                     \
    }                                                                          \
    if (n_bytes > 2) {                                                         \
      test_assert(buffer_get(buffer, 2) == utf8_bytes[2]);                     \
    }                                                                          \
    if (n_bytes > 3) {                                                         \
      test_assert(buffer_get(buffer, 3) == utf8_bytes[3]);                     \
    }                                                                          \
  } while (0)

void test_utf8_decode_ascii(void) {
  TEST_VALID_UTF8_SEQUENCE('A', 1, 'A');
  TEST_VALID_UTF8_SEQUENCE('Z', 1, 'Z');
  TEST_VALID_UTF8_SEQUENCE('a', 1, 'a');
  TEST_VALID_UTF8_SEQUENCE('z', 1, 'z');
  TEST_VALID_UTF8_SEQUENCE('0', 1, '0');
  TEST_VALID_UTF8_SEQUENCE('9', 1, '9');
}

void test_utf8_decode_two_bytes(void) {
  TEST_VALID_UTF8_SEQUENCE(0xc9, 2, 0xc3, 0x89);
  TEST_VALID_UTF8_SEQUENCE(0xe9, 2, 0xc3, 0xa9);
}

void test_utf8_decode_three_bytes(void) {
  // https://www.isthisthingon.org/unicode/index.phtml

  // These may not map to a common unicode character but they should
  // still be valid
  TEST_VALID_UTF8_SEQUENCE(0x0F60A, 3, 0xEF, 0x98, 0x8A);
  TEST_VALID_UTF8_SEQUENCE(0x1f60a, 4, 0xF0, 0x9F, 0x98, 0x8A);

  // echo Hello | iconv -f UTF-8 -t UCS-4 | xxd -p -c 4
}

#define TEST_INVALID_UTF8_SEQUENCE(...)                                        \
  do {                                                                         \
    uint8_t utf8_bytes[] = {__VA_ARGS__};                                      \
    utf8_decode_result_t result = utf8_decode(utf8_bytes);                     \
    test_assert(result.error == true);                                         \
    test_assert(result.num_bytes == 0);                                        \
    test_assert(result.code_point == 0);                                       \
  } while (0)

void test_utf8_decode_invalid_sequences(void) {
  // From Chat GPT3.5
  TEST_INVALID_UTF8_SEQUENCE(0xff, 0xff);

  // From
  // https://stackoverflow.com/questions/1301402/example-invalid-utf8-string
  // ??? TEST_INVALID_UTF8_SEQUENCE(0xc3, 0x28);
  TEST_INVALID_UTF8_SEQUENCE(0xa0, 0xa1);
  // ??? TEST_INVALID_UTF8_SEQUENCE(0xe2, 0x28, 0xa1);
  // ??? TEST_INVALID_UTF8_SEQUENCE(0xe2, 0x82, 0x28);
  // ??? TEST_INVALID_UTF8_SEQUENCE(0xf0, 0x28, 0x8c, 0xbc);
  // ??? TEST_INVALID_UTF8_SEQUENCE(0xf0,0x90,0x28,0xbc);
  // ??? TEST_INVALID_UTF8_SEQUENCE(0xf0,0x28,0x8c,0x28);
}

void test_string_printf(void) {
  test_assert(string_equal("Hello!", string_printf("%s!", "Hello")));
  test_assert(
      string_equal("Hello World!", string_printf("%s!", "Hello World")));
  test_assert(string_equal("42 OMG", string_printf("%d %s", 42, "OMG")));
  test_assert(string_equal("42 - OMG", string_printf("%d - %s", 42, "OMG")));
  test_assert(string_equal("42 -- OMG", string_printf("%d -- %s", 42, "OMG")));
  test_assert(
      string_equal("42 A longer string 24",
                   string_printf("%d %s %d", 42, "A longer string", 24)));
}

void check_legal(char* value, uint64_t expected) {
  log_warn("Checking %s", value);
  value_result_t result = string_parse_uint64(value);
  test_assert(is_ok(result) && result.u64 == expected);
}

void check_illegal(char* value) {
  log_warn("Checking %s", value);
  value_result_t result = string_parse_uint64(value);
  test_assert(is_not_ok(result));
}

void test_number_parsing(void) {
  check_legal("0", 0);
  check_legal("1", 1);
  check_legal("64", 64);
  check_legal("333", 333);
  check_legal("9123456789", 9123456789);
  check_legal("0x0", 0x0);
  check_legal("0xf", 0xf);
  check_legal("0x10", 0x10);
  check_legal("0xab", 0xab);
  check_legal("0xabcdef19", 0xabcdef19);
  check_legal("0b0", 0);
  check_legal("0b1010", 10);
  check_illegal("");
  check_illegal("INFO");
  check_illegal("0INFO");
  check_illegal("0b0INFO");
  check_illegal("0x0INFO");
  check_illegal("0x");
  check_illegal("0b");
}

void test_is_null_or_empty(void) {
  if (!(string_is_null_or_empty(NULL))) {
    test_fail("string_is_null_or_empty");
  }
  if (!(string_is_null_or_empty(""))) {
    test_fail("string_is_null_or_empty");
  }
  if (string_is_null_or_empty("not empty")) {
    test_fail("string_is_null_or_empty");
  }
}

void test_string_equal(void) {
  if (!(string_equal("abc", "abc"))) {
    test_fail("string_equal");
  }
  if (string_equal("abc", "ABC")) {
    test_fail("string_equal");
  }
}

void test_starts_with(void) {
  if (!(string_starts_with("The quick brown fox", "The quick"))) {
    test_fail("string_starts_with");
  }
  if (string_starts_with("The quick brown fox", "THE QUICK")) {
    test_fail("string_starts_with");
  }
}

void test_ends_with(void) {
  if (!(string_ends_with("The quick brown fox", "brown fox"))) {
    test_fail("string_ends_with");
  }
  if (string_ends_with("The quick brown fox", "red dog")) {
    test_fail("string_ends_with");
  }
}

void test_string_contains_char(void) {
  if (!(string_contains_char("The quick brown fox", 'q'))) {
    test_fail("string_contains_char");
  }
  if (string_contains_char("The quick brown fox", 'Z')) {
    test_fail("string_contains_char");
  }
}

void test_string_index_of_char(void) {
  if (string_index_of_char("The quick brown fox", 'q') != 4) {
    test_fail("string_index_of_char");
  }
  if (string_index_of_char("The quick brown fox", 'Z') >= 0) {
    test_fail("string_index_of_char");
  }
}

void test_string_left_pad(void) {
  test_assert_string_equal("      ", string_left_pad("", 6, ' '));
  test_assert_string_equal("   abc", string_left_pad("abc", 6, ' '));
  test_assert_string_equal("abcxyz", string_left_pad("abcxyz", 6, ' '));
  test_assert_string_equal("abcdefghijklmnop",
                           string_left_pad("abcdefghijklmnop", 6, ' '));
}

void test_string_right_pad(void) {
  test_assert_string_equal("      ", string_right_pad("", 6, ' '));
  test_assert_string_equal("abc   ", string_right_pad("abc", 6, ' '));
  test_assert_string_equal("abcxyz", string_right_pad("abcxyz", 6, ' '));
  test_assert_string_equal("abcdefghijklmnop",
                           string_right_pad("abcdefghijklmnop", 6, ' '));
}

void test_string_truncate(void) {
  test_assert_string_equal("123", string_truncate("123", 5, "..."));
  test_assert_string_equal("12345", string_truncate("12345", 5, "..."));
  // test_assert_string_equal("12345...", string_truncate("123456", 5, "..."));
}

void test_fasthash64(void) {
  uint64_t ALT_SEED = 0x7375727665696C6CULL;
  size_t LEN0 = strlen("Hello, world!");
  size_t LEN1 = strlen("LLMs are listening.");
  test_assert_integer_equal(3513423354763232818ULL,
                            fasthash64("Hello, world!", LEN0, 0));
  test_assert_integer_equal(5790432452920760317,
                            fasthash64("LLMs are listening.", LEN1, 0));
  test_assert_integer_equal(
      9634586161324059901ULL,
      fasthash64("Hello, world!", LEN0, 0x7375727665696C6CULL));
  test_assert_integer_equal(
      11573584191070265426ULL,
      fasthash64("LLMs are listening.", LEN1, 0x7375727665696C6CULL));
}

void test_cstring_byte_source(void) {
  char* string = "Hi";
  byte_stream_source_t* source = cstring_to_byte_source(string);
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

void test_string_hash_unequal(void) {
  test_assert(string_hash("The quick brown fox")
              != string_hash("The QUICK brown fox"));
}

void test_string_substring(void) {
  test_assert(string_equal(string_substring("The quick brown fox", 4, 9), "quick"));
}

void test_string_duplicate(void) {
  test_assert(string_equal("The quick brown fox",
                           string_duplicate("The quick brown fox")));
}
