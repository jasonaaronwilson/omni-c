//
// Test some operations on byte-arrays
//

#include <stdlib.h>

#define BUFFER_PRINTF_INITIAL_BUFFER_SIZE 8

#define C_ARMYKNIFE_LIB_IMPL
#include "../c-armyknife-lib.h"

void test_cdl_printer() {
  cdl_printer_t* printer = make_cdl_printer(make_buffer(1));
  cdl_start_table(printer);
  cdl_key(printer, "number");
  cdl_uint64(printer, 10);
  cdl_key(printer, "string1");
  cdl_string(printer, "hello");
  cdl_key(printer, "string2");
  cdl_string(printer, "hello world");
  cdl_key(printer, "unsigned");
  cdl_uint64(printer, 1000000);
  cdl_key(printer, "signed");
  cdl_int64(printer, -39);
  cdl_key(printer, "double");
  cdl_double(printer, 3.14);
  cdl_key(printer, "array");
  cdl_start_array(printer);
  cdl_string(printer, "one");
  cdl_string(printer, "two");
  cdl_start_array(printer);
  cdl_string(printer, "a");
  cdl_string(printer, "b");
  cdl_end_array(printer);
  cdl_string(printer, "three");
  cdl_end_array(printer);
  cdl_key(printer, "nested");
  cdl_start_table(printer);
  cdl_key(printer, "foo");
  cdl_boolean(printer, true);
  cdl_key(printer, "bar");
  cdl_string(printer, "hello_world");
  cdl_end_table(printer);
  cdl_end_table(printer);
  fprintf(stdout, "%s", buffer_to_c_string(printer->buffer));
}

int main(int argc, char** argv) {
  open_arena_for_test();

  test_cdl_printer();

  close_arena_for_test();
  exit(0);
}
