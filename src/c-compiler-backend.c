int invoke_c_compiler(char* input_file, char* output_file) {
  value_array_t* argv = c_compiler_command_line(input_file, output_file);

  log_warn("Invoking C compiler with these arguments: %s",
           buffer_to_c_string(join_array_of_strings(argv, " ")));
  sub_process_t* sub_process = make_sub_process(argv);
  sub_process_launch(sub_process);

  buffer_t* buffer = make_buffer(1);
  do {
    sub_process_read(sub_process, buffer, buffer);
    usleep(5);
  } while (is_sub_process_running(sub_process));
  sub_process_read(sub_process, buffer, buffer);
  sub_process_wait(sub_process);

  log_warn(">>> Exit Status %d <<<\n%s", sub_process->exit_code,
           buffer_to_c_string(buffer));

  return sub_process->exit_code;
}

value_array_t* c_compiler_command_line(char* input_file, char* output_file) {
  if (string_equal("clang", FLAG_c_compiler)
      || string_equal("gcc", FLAG_c_compiler)
      || string_equal("tcc", FLAG_c_compiler)
      || string_equal("zig", FLAG_c_compiler)) {
    value_array_t* argv = make_value_array(2);
    value_array_add(argv, str_to_value(FLAG_c_compiler));
    if (string_equal("zig", FLAG_c_compiler)) {
      value_array_add(argv, str_to_value("cc"));
    }
    value_array_add(argv, str_to_value("-g"));
    value_array_add(argv, str_to_value("-rdynamic"));
    value_array_add(argv, str_to_value("-O3"));
    value_array_add(argv, str_to_value("-std=gnu99"));
    value_array_add(argv, str_to_value("-o"));
    value_array_add(argv, str_to_value(output_file));
    value_array_add(argv, str_to_value(input_file));
    value_array_add(argv, str_to_value("-lgc"));
    return argv;
  } else {
    log_fatal("Unknown C compiler %s\n", FLAG_c_compiler);
    fatal_error(ERROR_ILLEGAL_INPUT);
  }
}
