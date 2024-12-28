///
/// Test command line parsing.
///

#include <stdlib.h>

#define C_ARMYKNIFE_LIB_IMPL
#include "../c-armyknife-lib.h"

void test_boolean(void) {
  char* args[3];
  args[0] = "boolean-test";
  args[1] = "--true-boolean=true";
  args[2] = "--false-boolean=false";

  boolean_t FLAG_true_boolean = false;
  boolean_t FLAG_false_boolean = true;
  value_array_t* FLAG_files = NULL;

  flag_program_name("boolean-test");
  flag_boolean("--true-boolean", &FLAG_true_boolean);
  flag_boolean("--false-boolean", &FLAG_false_boolean);
  flag_file_args(&FLAG_files);

  char* error = flag_parse_command_line(3, args);
  test_assert(!error);
  test_assert(FLAG_true_boolean);
  test_assert(!FLAG_false_boolean);

  // ----------------------------------------------------------------------

  FLAG_true_boolean = false;
  FLAG_false_boolean = true;
  args[1] = "--true-boolean=1";
  args[2] = "--false-boolean=0";
  error = flag_parse_command_line(3, args);
  test_assert(!error);
  test_assert(FLAG_true_boolean);
  test_assert(!FLAG_false_boolean);

  // ----------------------------------------------------------------------

  FLAG_true_boolean = false;
  FLAG_false_boolean = true;
  args[1] = "--true-boolean=t";
  args[2] = "--false-boolean=f";
  error = flag_parse_command_line(3, args);
  test_assert(!error);
  test_assert(FLAG_true_boolean);
  test_assert(!FLAG_false_boolean);

  // ----------------------------------------------------------------------

  args[1] = "--true-boolean=BAD";
  error = flag_parse_command_line(3, args);
  test_assert(error);
}

void test_uint64(void) {
  char* args[2];
  args[0] = "uint64-test";
  args[1] = "--number=0";

  uint64_t FLAG_number = 0;
  value_array_t* FLAG_files = NULL;

  flag_program_name("uint64-test");
  flag_uint64("--number", &FLAG_number);
  flag_file_args(&FLAG_files);

  char* error = flag_parse_command_line(2, args);
  test_assert(!error);
  test_assert(FLAG_number == 0);

  // ----------------------------------------------------------------------

  args[1] = "--number=0xf";
  error = flag_parse_command_line(2, args);
  test_assert(!error);
  test_assert(FLAG_number == 0xf);

  // ----------------------------------------------------------------------

  args[1] = "--number=0b110";
  error = flag_parse_command_line(2, args);
  test_assert(!error);
  test_assert(FLAG_number == 6);
}

void test_string(void) {
  char* args[2];
  args[0] = "string-test";
  args[1] = "--string=0";

  char* FLAG_string = NULL;
  value_array_t* FLAG_files = NULL;

  flag_program_name("string-test");
  flag_string("--string", &FLAG_string);
  flag_file_args(&FLAG_files);

  char* error = flag_parse_command_line(2, args);
  test_assert(!error);
  test_assert(string_equal("0", FLAG_string));

  // ----------------------------------------------------------------------

  args[1] = "--string=hello world";
  error = flag_parse_command_line(2, args);
  test_assert(!error);
  test_assert(string_equal("hello world", FLAG_string));
}

void test_leftovers(void) {
  char* args[4];
  args[0] = "leftovers-test";
  args[1] = "--string=0";
  args[2] = "file1";
  args[3] = "dir/file2";

  char* FLAG_string = NULL;
  value_array_t* FLAG_files = NULL;

  flag_program_name("leftovers-test");
  flag_string("--string", &FLAG_string);
  flag_file_args(&FLAG_files);

  char* error = flag_parse_command_line(4, args);
  test_assert(!error);
  test_assert(string_equal("0", FLAG_string));
  test_assert(FLAG_files);
  test_assert(string_equal("file1", value_array_get(FLAG_files, 0).str));
  test_assert(string_equal("dir/file2", value_array_get(FLAG_files, 1).str));
}

typedef enum {
  test_enum_unknown,
  test_enum_a,
  test_enum_b,
  test_enum_c
} test_enum_t;

void test_enum_64(void) {
  char* args[2];
  args[0] = "enum-64-test";
  args[1] = "--enum=a";

  uint64_t FLAG_enum = test_enum_unknown;
  value_array_t* FLAG_files = NULL;

  flag_program_name("enum-64-test");
  flag_enum_64("--enum", &FLAG_enum);
  flag_enum_value("a", test_enum_a);
  flag_enum_value("A", test_enum_a);
  flag_enum_value("b", test_enum_b);
  flag_enum_value("c", test_enum_c);

  flag_file_args(&FLAG_files);

  char* error = flag_parse_command_line(2, args);
  test_assert(!error);
  test_assert(FLAG_enum == test_enum_a);

  // ----------------------------------------------------------------------

  args[1] = "--enum=b";
  error = flag_parse_command_line(2, args);
  test_assert(!error);
  test_assert(FLAG_enum == test_enum_b);

  // ----------------------------------------------------------------------

  args[1] = "--enum=BAD";
  error = flag_parse_command_line(2, args);
  test_assert(error);
}

void test_enum(void) {
  char* args[2];
  args[0] = "enum-test";
  args[1] = "--enum=a";

  test_enum_t FLAG_enum = test_enum_unknown;
  value_array_t* FLAG_files = NULL;

  flag_program_name("enum-64-test");
  flag_enum("--enum", (int*) &FLAG_enum);
  flag_enum_value("a", test_enum_a);
  flag_enum_value("A", test_enum_a);
  flag_enum_value("b", test_enum_b);
  flag_enum_value("c", test_enum_c);

  flag_file_args(&FLAG_files);

  char* error = flag_parse_command_line(2, args);
  test_assert(!error);
  test_assert(FLAG_enum == test_enum_a);

  // ----------------------------------------------------------------------

  args[1] = "--enum=b";
  error = flag_parse_command_line(2, args);
  test_assert(!error);
  test_assert(FLAG_enum == test_enum_b);

  // ----------------------------------------------------------------------

  args[1] = "--enum=BAD";
  error = flag_parse_command_line(2, args);
  test_assert(error);
}

void test_command_1(void) {

  boolean_t FLAG_true_boolean = false;
  value_array_t* FLAG_files = NULL;
  char* FLAG_sub_command;

  char* args[3];
  args[0] = "foo";
  args[1] = "sub_command_zero";
  args[2] = "--true-boolean=true";

  flag_program_name("foo");
  flag_boolean("--true-boolean", &FLAG_true_boolean);
  flag_command("sub_command_zero", &FLAG_sub_command);
  flag_file_args(&FLAG_files);

  char* error = flag_parse_command_line(3, args);
  test_assert(!error);
  test_assert(FLAG_true_boolean);

  test_assert_string_equal("sub_command_zero", FLAG_sub_command);
}

void test_command_2(void) {

  boolean_t FLAG_true_boolean = false;
  value_array_t* FLAG_files = NULL;
  char* FLAG_sub_command;

  char* args[3];
  args[0] = "foo";
  args[1] = "sub_command_zero";
  args[2] = "--true-boolean=true";

  flag_program_name("foo");
  flag_command("sub_command_zero", &FLAG_sub_command);
  // This is the only line that has changed since test_command_1
  flag_boolean("--true-boolean", &FLAG_true_boolean);
  flag_file_args(&FLAG_files);

  char* error = flag_parse_command_line(3, args);
  test_assert(!error);
  test_assert(FLAG_true_boolean);

  test_assert_string_equal("sub_command_zero", FLAG_sub_command);
}

void test_bad_command(void) {
  value_array_t* FLAG_files = NULL;
  char* FLAG_sub_command;

  char* args[2];
  args[0] = "foo";
  args[1] = "bad_command";

  flag_program_name("foo");
  flag_command("good_command", &FLAG_sub_command);
  flag_command("good_command_two", &FLAG_sub_command);
  flag_file_args(&FLAG_files);

  char* error = flag_parse_command_line(2, args);
  test_assert(error);
  test_assert_string_equal(
      "The first command line argument is not a known command: bad_command",
      error);
}

void test_bad_empty_command(void) {
  value_array_t* FLAG_files = NULL;
  char* FLAG_sub_command;

  char* args[1];
  args[0] = "foo";

  flag_program_name("foo");
  flag_command("good_command", &FLAG_sub_command);
  flag_command("good_command_two", &FLAG_sub_command);
  flag_file_args(&FLAG_files);

  char* error = flag_parse_command_line(1, args);
  test_assert(error);
  test_assert_string_equal(
      "This program requires a command but not enough arguments were given",
      error);
}

void test_alias(void) {
  value_array_t* FLAG_files = NULL;
  boolean_t FLAG_true_boolean = false;

  char* args[2];
  args[0] = "foo";
  args[1] = "--true-boolean=true";

  flag_program_name("foo");
  flag_boolean("--true-boolean", &FLAG_true_boolean);
  flag_alias("--true");

  flag_file_args(&FLAG_files);

  char* error = flag_parse_command_line(2, args);
  test_assert(!error && FLAG_true_boolean);

  FLAG_true_boolean = false;
  args[1] = "--true=true";

  error = flag_parse_command_line(2, args);
  test_assert(!error && FLAG_true_boolean);
}


int main(int argc, char** argv) {
  configure_fatal_errors((fatal_error_config_t){
      .catch_sigsegv = true,
  });
  test_boolean();
  test_uint64();
  test_string();
  test_enum();
  test_enum_64();
  test_leftovers();
  test_command_1();
  test_command_2();
  test_bad_command();
  test_bad_empty_command();
  test_alias();
  exit(0);
}
