///
/// Test the additional string utilities.
///

void test_string_tokenizer(void) {
  value_array_t* tokens = NULL;

  tokens = string_tokenize("The quick brown fox", " ");
  test_assert(tokens->length == 4);

  tokens = string_tokenize("The#quick#brown fox", " #");
  test_assert(tokens->length == 4);

  tokens = string_tokenize("The#quick#brown fox", " #");
  test_assert((tokens->length == 4)
              && string_equal("quick", value_array_get(tokens, 1).str));
}
