/**
 * @file flags.c
 *
 * These are (currently) all of the flags that omni-c understands. We
 * are keeping them in a single place since there is a bunch of
 * overlap between commands. (This structure isn't the only way to use
 * the flag's parser...)
 */

value_array_t* FLAG_files = nullptr;
char* FLAG_command = nullptr;
boolean_t FLAG_print_command_line = true;
boolean_t FLAG_include_unnamed_nodes = false;
boolean_t FLAG_print_tokens_show_tokens = false;
boolean_t FLAG_print_tokens_include_whitespace = false;
boolean_t FLAG_print_tokens_include_comments = false;
boolean_t FLAG_print_tokens_parse_and_print = true;
boolean_t FLAG_print_tokens_show_appended_tokens = true;
char* FLAG_c_output_file = nullptr;
char* FLAG_binary_output_file = nullptr;
char* FLAG_archive_output_file = nullptr;
boolean_t FLAG_generate_enum_convertors = true;
char* FLAG_expression = nullptr;
char* FLAG_statement = nullptr;
boolean_t FLAG_dump_symbol_table = false;
boolean_t FLAG_use_statement_parser = true;
boolean_t FLAG_to_c = false;
boolean_t FLAG_omit_c_armyknife_include = false;
char* FLAG_c_compiler = "clang";

void configure_flags(void) {
  flag_program_name("omni-c");
  flag_description(
      "omni-c is a transpiler for the omni-c language as well as a code "
      "generation tool for ISO C.");

  // I don't think these flags are truly global...
  flag_boolean("--print-command-line", &FLAG_print_command_line);
  flag_boolean("--use-statement-parser", &FLAG_use_statement_parser);

  configure_regular_commands();

  configure_print_tokens_command();
  configure_parse_expression();
  configure_parse_statement();
}

void configure_parse_expression(void) {
  flag_command("parse-expression", &FLAG_command);
  flag_description("** UNIT TESTING **");
  flag_string("--expression", &FLAG_expression);
  flag_boolean("--to-c", &FLAG_to_c);
}

void configure_parse_statement(void) {
  flag_command("parse-statement", &FLAG_command);
  flag_description("** UNIT TESTING **");
  flag_string("--statement", &FLAG_statement);
}

void configure_print_tokens_command(void) {
  flag_command("print-tokens", &FLAG_command);
  flag_description("** UNIT TESTING **");
  flag_boolean("--show-tokens", &FLAG_print_tokens_show_tokens);
  flag_boolean("--include-whitespace", &FLAG_print_tokens_include_whitespace);
  flag_boolean("--include-comments", &FLAG_print_tokens_include_comments);
  flag_boolean("--parse-and-print", &FLAG_print_tokens_parse_and_print);
  flag_boolean("--show-appended-tokens",
               &FLAG_print_tokens_show_appended_tokens);
  flag_file_args(&FLAG_files);
}

void configure_regular_commands(void) {
  flag_command("generate-header-file", &FLAG_command);
  flag_description(
      "create a single C file 'library header file'; most users will prefer "
      "'build'");
  flag_string("--c-output-file", &FLAG_c_output_file);
  flag_boolean("--generate-enum-convertors", &FLAG_generate_enum_convertors);
  flag_boolean("--dump-symbol-table", &FLAG_dump_symbol_table);
  flag_boolean("--use-statement-parser", &FLAG_use_statement_parser);
  flag_boolean("--omit-c-armyknife-include", &FLAG_omit_c_armyknife_include);

  flag_file_args(&FLAG_files);

  flag_command("generate-library", &FLAG_command);
  flag_description(
      "create a single C file 'library' of C99 code; most users will prefer "
      "'build'");
  flag_string("--c-output-file", &FLAG_c_output_file);
  flag_boolean("--generate-enum-convertors", &FLAG_generate_enum_convertors);
  flag_boolean("--dump-symbol-table", &FLAG_dump_symbol_table);
  flag_boolean("--use-statement-parser", &FLAG_use_statement_parser);
  flag_boolean("--omit-c-armyknife-include", &FLAG_omit_c_armyknife_include);
  flag_file_args(&FLAG_files);

  flag_command("build", &FLAG_command);
  flag_description(
      "build an executable by generating the C code and invoking the C "
      "compiler");
  flag_string("--c-output-file", &FLAG_c_output_file);
  flag_string("--binary-output-file", &FLAG_binary_output_file);
  flag_boolean("--generate-enum-convertors", &FLAG_generate_enum_convertors);
  flag_boolean("--dump-symbol-table", &FLAG_dump_symbol_table);
  flag_boolean("--use-statement-parser", &FLAG_use_statement_parser);
  flag_boolean("--omit-c-armyknife-include", &FLAG_omit_c_armyknife_include);
  flag_string("--c-compiler", &FLAG_c_compiler);
  flag_file_args(&FLAG_files);

  flag_command("archive", &FLAG_command);
  flag_description("create an archive of unprocessed source files");
  flag_string("--archive-output-file", &FLAG_archive_output_file);
  flag_description("the target path of the output archive");
  flag_file_args(&FLAG_files);

  flag_command("test", &FLAG_command);
  flag_description("compile and run all unit tests in the input files");
  // TODO(jawilson): copy flags from build (should be get rid of
  // use-statement-parser flag?)
  flag_file_args(&FLAG_files);
}
