/**
 * This is the main routine for the omni-c transpiler.
 */

#include <stdio.h>

#include "../../c-armyknife-lib/c-armyknife-lib.h"
#include "common/oc-compiler-state.h"
#include "common/oc-node.h"
#include "parser/parse-files.h"

void show_help() {
  fprintf(stdout, "Usage: omni-c <command> <flags*> <files*>\n");
  fprintf(stdout, "  help\n");
  fprintf(stdout, "  build\n");
  fprintf(stdout, "  install\n");
  fprintf(stdout, "  translate\n");
  fprintf(stdout, "  archive\n");
  fprintf(stdout, "  debug\n");
  fprintf(stdout, "  xreference\n");
  fprintf(stdout, "  version\n");
  fprintf(stdout, "\n");
  fprintf(stdout, "See c-transpiler-manual.md\n");
}

void translate_and_build(command_line_parse_result_t args_and_files) {
  oc_compiler_state_t* compiler_state = make_oc_compiler_state();
  ptr_array_t* files = parse_files(args_and_files.files);
  for (int i = 0; i < files->length; i++) {
    oc_file_t* file = ptr_array_get(files, i);
    TSNode root_node = ts_tree_root_node(file->tree);
    oc_node_t* node = ts_node_to_oc_node(root_node, file->data);
    buffer_t* output = make_buffer(1024);
    output = append_oc_node_text(output, node);
    fprintf(stdout, "%s\n", buffer_to_c_string(output));
  }
}

int main(int argc, char** argv) {

  command_line_parse_result_t args_and_files
      = parse_command_line(argc, argv, true);

  if (args_and_files.command == NULL
      || string_equal("help", args_and_files.command)) {
    show_help();
    exit(0);
  }

  if (string_equal("build", args_and_files.command)) {
    translate_and_build(args_and_files);
    exit(0);
  } else {
    fprintf(stderr, "Uknown command: %s\n", args_and_files.command);
  }

  exit(0);
}
