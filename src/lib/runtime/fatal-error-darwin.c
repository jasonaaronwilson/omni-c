char* get_command_line(void) {
  return "<command-line-unavailable>";
}

char* get_program_path(void) {
  return "<program-path-line-unavailable>";
}


void print_backtrace(void) {
#ifndef NO_VM_BACKTRACE_ON_FATAL_ERROR
  do {
    void* array[10];
    int size = backtrace(array, 10);
    char** strings = backtrace_symbols(array, size);

    // Print the backtrace
    for (int i = 0; i < size; i++) {
      printf("#%d %s\n", i, strings[i]);
    }
  } while (0);
#endif /* NO_VM_BACKTRACE_ON_FATAL_ERROR */
}
