/**
 * This is the main routine for the omni-c transpiler.
 */

#include <stdio.h>

#include "common/oc-compiler-state.h"
#include "common/oc-node.h"
#include "parser/parse-files.h"
#include <c-armyknife-lib.h>

value_array_t* FLAG_files = NULL;
char* FLAG_command = NULL;

void translate_and_build() {
  log_info("translate_and_build()");

  oc_compiler_state_t* compiler_state = make_oc_compiler_state();
  value_array_t* parsed_files = parse_files(FLAG_files);
  for (int i = 0; i < FLAG_files->length; i++) {
    oc_file_t* file = (oc_file_t*) value_array_get(parsed_files, i).ptr;
    TSNode root_node = ts_tree_root_node(file->tree);
    oc_node_t* node = ts_node_to_oc_node(root_node, file->data);
    buffer_t* output = make_buffer(1024);
    output = append_oc_node_text(output, node);
    fprintf(stdout, "%s\n", buffer_to_c_string(output));
  }
}

void configure_build_command(void);

void configure_flags() {
  flag_program_name("omni-c");
  flag_description("omni-c is a transpiler for the omni-c language.");

  configure_build_command();
}

void configure_build_command(void) {
  flag_command("build", &FLAG_command);
  flag_file_args(&FLAG_files);
}

int main(int argc, char** argv) {
  configure_fatal_errors((fatal_error_config_t){
      .catch_sigsegv = true,
  });
  logger_init();

  configure_flags();

  char* error = flag_parse_command_line(argc, argv);
  if (error) {
    flag_print_help(stderr, error);
    exit(1);
  }

  if (FLAG_command == NULL) {
    // Technically this should not be reached because once a command
    // is defined, a missing or wrong command should trigger an error
    // and caught above. Note sure why this is still happening but
    // paranoid code can be easier to debug.
    fatal_error(ERROR_BAD_COMMAND_LINE);
  } else if (string_equal("build", FLAG_command)) {
    translate_and_build();
  } else {
    fprintf(stderr, "Uknown command: %s\n", FLAG_command);
  }

  exit(0);
}
