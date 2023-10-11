/**
 * This is the main routine for the omni-c transpiler.
 */

#include <stdio.h>

#include "../../c-armyknife-lib/c-armyknife-lib.h"

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
  
}

int main(int argc, char** argv) {

  command_line_parse_result_t args_and_files
    = parse_command_line(argc, argv, true);

  if (args_and_files.command == NULL || string_equal("help", args_and_files.command)) {
    show_help();
    exit(0);
  }

  if (string_equal("build", args_and_files.command)) {
    translate_and_build(args_and_files);
    exit(0);
  }

  exit(0);
}
