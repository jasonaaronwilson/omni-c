//
// Test some operations on byte-arrays
//

void leaf(void) {
  error_t* error = make_error(ERROR_ILLEGAL_STATE);
  throw_error(error);
}

void leaf_caller(void) {
  leaf();
}

void test_throws(void) {
  error_t* error = nullptr;
  try_call(leaf_caller(), error);
  if (error == nullptr) {
    test_fail("Didn't get an error as expected");
  }
}

void nop(void) {
  current_time_millis();
}

void test_doesnt_throw(void) {
  error_t* error = nullptr;
  try_call(nop(), error);
  if (error != nullptr) {
    test_fail("Got an error when we shouldn't have");
  }
}

char* even_odd_caller(int index) {
  if ((index & 1) == 0) {
    throw_error(make_error(ERROR_ILLEGAL_STATE));
  }
  return "foo";
}

void test_throw_loop(void) {
  int num_errors = 0;
  for (int i = 0; i < 10; i++) {
    error_t* error = nullptr;
    try_call(even_odd_caller(i), error);
    if (error == nullptr) {
      num_errors++;
    }
  }
  test_assert_integer_equal(5, num_errors);
}

void first_caller(void) {
  error_t* error = nullptr;
  try_call(leaf_caller(), error);
}

void test_multiple_catches(void) {
  error_t* error = nullptr;
  try_call(first_caller(), error);
  if (error != nullptr) {
    test_fail("Shouldn't have caught the error here.");
  }
}

void rethrower(void) {
  error_t* error = nullptr;
  try_call(leaf_caller(), error);
  throw_error(error);
}

void test_rethrow(void) {
  error_t* error = nullptr;
  try_call(rethrower(), error);
  if (error == nullptr) {
    test_fail("Should have caught rethrow error");
  }
}

void full_details_thrower(void) {
  error_t* error = make_error(ERROR_ILLEGAL_STATE);
  error->message = "Full message";
  throw_error(error);
}

void test_error_details(void) {
  error_t* error = nullptr;
  try_call(full_details_thrower(), error);
  if (error == nullptr) {
    test_fail("Should have caught rethrow error");
  }
  test_assert_string_equal("Full message", error->message);
  test_assert_string_equal("tests/lib/error-system-test.c", error->file_name);
  test_assert_string_equal("full_details_thrower", error->function_name);
  if (error->line < 75 || error->line > 85) {
    log_warn("Line number is incorrect %d", error->line);
    test_fail("Line number doesn't seem to be in range.");
  }
}

volatile void* crash_ptr = 0;

void sigsegv(void) {
  // Making the pointer variable itself 'volatile' prevents the compiler 
  // from statically analyzing it or optimizing it away, suppressing 
  // warnings about the obvious null dereference.
  // volatile int* crash_ptr = (int*)0;
  *(cast(int*, crash_ptr)) = 0xDEAD;
}

void test_sigsegv(void) {
  install_signal_handlers();
  error_t* error = nullptr;
  try_call(sigsegv(), error);
  if (error == nullptr) {
    test_fail("Should have caught rethrow error");
  } else {
    log_warn("We did get an error!");
  }
}
