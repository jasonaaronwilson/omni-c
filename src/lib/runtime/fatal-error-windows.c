char* get_command_line(void) {
  return "command line not available under windows right now";
}

char* get_program_path(void) {
  return "<program-path-unknown>";
}

void print_backtrace(void) {
  printf("Stack traces are not available under windows currently.\n");
}
