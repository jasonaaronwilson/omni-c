///
/// Testing appending references of things to a byte array.
///

#include <stdlib.h>

#define C_ARMYKNIFE_LIB_IMPL
#include "../c-armyknife-lib.h"

void test_write_then_read(char* file_name) {
  buffer_t* hello_buf = make_buffer(1);
  hello_buf = buffer_append_string(hello_buf, "Hello World!");
  buffer_write_file(hello_buf, file_name);

  buffer_t* world_buf = make_buffer(1);
  world_buf = buffer_append_file_contents(world_buf, file_name);
  test_assert(string_equal(buffer_to_c_string(hello_buf),
                           buffer_to_c_string(world_buf)));
}

int main(int argc, char** argv) {
  test_write_then_read(argv[1]);
  exit(0);
}
