///
/// Make some hashtables and put stuff into them.
///

#include <stdlib.h>

#define C_ARMYKNIFE_LIB_IMPL
#include "../c-armyknife-lib.h"

void test_string_to_int_hashtable() {
  char* mystring = "Hello World!";
  char* mystring2 = "foo bar baz";

  uint64_t mynumber = 1973;
  hashtable_t(char*, uint64_t)* ht
      = make_hashtable(char_ptr_type(), uint64_type(), 32);

  hashtable_set_value(ht, reference_of_char_ptr(&mystring),
                      reference_of_uint64(&mynumber));

  /*
  mynumber += 50;
  hashtable_set_value(ht, reference_of(char_ptr_type(), mystring2),
                      reference_of_uint64(&mynumber));
  */

  uint64_t number = dereference_uint64(
      hashtable_get_reference_to_value(ht, reference_of_char_ptr(&mystring)));
  if (number != 1973) {
    ARMYKNIFE_TEST_FAIL("should read back 1973");
  }
}

int main(int argc, char** argv) {
  test_string_to_int_hashtable();
  exit(0);
}
