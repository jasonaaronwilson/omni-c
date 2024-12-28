//
// Test Sub Processes
//

#include <stdlib.h>

#define C_ARMYKNIFE_LIB_IMPL
#include "../c-armyknife-lib.h"

void test_sub_process_read(void) {
  value_array_t* argv = make_value_array(1);
  value_array_add(argv, str_to_value("/usr/bin/ls"));

  sub_process_t* sub_process = make_sub_process(argv);
  sub_process_launch(sub_process);
  buffer_t* buffer = make_buffer(1);
  do {
    log_info("(Non-blocking) read from sub_process");
    sub_process_read(sub_process, buffer, NULL);
    log_info("Done reading from sub_process");
    sleep(1);
  } while (is_sub_process_running(sub_process));
  sub_process_read(sub_process, buffer, NULL);

  sub_process_wait(sub_process);
  test_assert(sub_process->exit_code == 0);

  printf("%s\n\n", buffer_to_c_string(buffer));
}

void test_sub_process_write(void) {
  value_array_t* argv = make_value_array(1);
  value_array_add(argv, str_to_value("/usr/bin/cat"));

  sub_process_t* sub_process = make_sub_process(argv);
  sub_process_launch(sub_process);

  buffer_t* buffer_out = make_buffer(1);
  buffer_printf(buffer_out, "Hello World!\n");
  uint64_t written = 0;

  while (written < buffer_out->length) {
    written += sub_process_write(sub_process, buffer_out, written);
    sleep(1);
  }
  sub_process_close_stdin(sub_process);

  buffer_t* buffer_in = make_buffer(1);

  do {
    log_info("(Non-blocking) read from sub_process");
    sub_process_read(sub_process, buffer_in, NULL);
    log_info("Done reading from sub_process");
    sleep(1);
  } while (is_sub_process_running(sub_process));
  sub_process_wait(sub_process);
  test_assert(sub_process->exit_code == 0);

  printf("%s\n\n", buffer_to_c_string(buffer_in));
}

void test_sub_process_launch_and_wait() {
  char* inline_script
      = "echo 'This goes to stdout'\n"
        "echo 'This goes to stderr' >&2\n"
        // "head -c 10\n"
        "cat\n"
        "exit 0";
  value_array_t* argv = make_value_array(1);
  value_array_add(argv, str_to_value("bash"));
  value_array_add(argv, str_to_value("-c"));
  value_array_add(argv, str_to_value(inline_script));
  sub_process_t* sub_process = make_sub_process(argv);

  buffer_t* buffer_stdin
      = buffer_from_string("This is additional stuff sent to stdout.\n");
  buffer_t* buffer_child_stdout = make_buffer(1);
  buffer_t* buffer_child_stderr = make_buffer(1);
  sub_process_launch_and_wait(sub_process, buffer_stdin, buffer_child_stdout,
                              buffer_child_stderr);

  printf("%s\n\n", buffer_to_c_string(buffer_child_stdout));
  printf("%s\n\n", buffer_to_c_string(buffer_child_stderr));
}

void test_sub_process_launch_and_wait_no_stdin() {
  char* inline_script
      = "echo 'This goes to stdout'\n"
        "echo 'This goes to stderr' >&2\n"
        // "head -c 10\n"
        "cat\n"
        "exit 0";
  value_array_t* argv = make_value_array(1);
  value_array_add(argv, str_to_value("bash"));
  value_array_add(argv, str_to_value("-c"));
  value_array_add(argv, str_to_value(inline_script));
  sub_process_t* sub_process = make_sub_process(argv);

  // The only difference with test_sub_process_launch_and_wait
  buffer_t* buffer_stdin = NULL;
  buffer_t* buffer_child_stdout = make_buffer(1);
  buffer_t* buffer_child_stderr = make_buffer(1);
  sub_process_launch_and_wait(sub_process, buffer_stdin, buffer_child_stdout,
                              buffer_child_stderr);

  printf("%s\n\n", buffer_to_c_string(buffer_child_stdout));
  printf("%s\n\n", buffer_to_c_string(buffer_child_stderr));
}


int main(int argc, char** argv) {
  test_sub_process_read();
  test_sub_process_write();
  test_sub_process_launch_and_wait();
  exit(0);
}
