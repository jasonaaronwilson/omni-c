char* get_command_line(void) {
  buffer_t* buffer
      = buffer_append_file_contents(make_buffer(1), "/proc/self/cmdline");
  buffer_replace_matching_byte(buffer, 0, ' ');
  return buffer_to_c_string(buffer);
}

char* get_program_path(void) {
  char buf[4096];
  int n = readlink("/proc/self/exe", buf, sizeof(buf));
  if (n > 0) {
    return string_duplicate(buf);
  } else {
    return "<program-path-unknown>";
  }
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
