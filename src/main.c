/**
 * This is the main routine for the omni-c transpiler.
 */

#include <stdio.h>

#include "../../c-armyknife-lib/c-armyknife-lib.h"

void show_help() {
  fprintf(stdout, "HELP GOES HERE\n");
}

int main(int argc, char** argv) {

  command_line_parse_result_t args_and_files
    = parse_command_line(argc, argv, true);

  if (string_equal("help", args_and_files.command)) {
    show_help();
    exit(0);
  }

  exit(0);
}
