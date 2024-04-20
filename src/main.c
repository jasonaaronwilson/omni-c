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
boolean_t FLAG_include_unnamed_nodes = false;

void translate_and_build(void) {
  log_info("translate_and_build()");

  oc_compiler_state_t* compiler_state = make_oc_compiler_state();
  value_array_t* parsed_files = parse_files(FLAG_files);
  for (int i = 0; i < FLAG_files->length; i++) {
    oc_file_t* file = (oc_file_t*) value_array_get(parsed_files, i).ptr;
    TSNode root_node = ts_tree_root_node(file->tree);
    oc_node_t* node = ts_node_to_oc_node(root_node, file->data, false);
    buffer_t* output = make_buffer(1024);
    output = append_oc_node_text(output, node, 0);
    fprintf(stdout, "%s\n", buffer_to_c_string(output));
  }
}

void print_parse_trees(void) {
  log_info("translate_and_build()");

  oc_compiler_state_t* compiler_state = make_oc_compiler_state();
  value_array_t* parsed_files = parse_files(FLAG_files);
  for (int i = 0; i < FLAG_files->length; i++) {
    oc_file_t* file = (oc_file_t*) value_array_get(parsed_files, i).ptr;
    TSNode root_node = ts_tree_root_node(file->tree);
    oc_node_t* node
        = ts_node_to_oc_node(root_node, file->data, FLAG_include_unnamed_nodes);
    buffer_t* output = make_buffer(1024);
    output = append_oc_node_tree(output, node);
    fprintf(stdout, "%s\n", buffer_to_c_string(output));
  }
}

/**
 * @function append_function_prototype
 *
 * Given an OC_NODE_FUNCTION_DECLARATOR, format this as a simple
 * prototype (i.e., ignore the OC_NODE_COMPOUND_STATEMENT (aka
 * "block")) at the end of it.
 */
buffer_t* append_function_prototype(buffer_t* buffer, oc_node_t* node) {
  buffer = buffer_append_string(buffer, "\n");
  for (int i = 0; i < node->children->length; i++) {
    oc_node_t* child = (oc_node_t*) value_array_get(node->children, i).ptr;
    if (child->tag != OC_NODE_COMPOUND_STATEMENT) {
      buffer = buffer_append_string(buffer, " ");
      buffer = append_oc_node_text(buffer, child, 1);
    }
  }
  buffer = buffer_append_string(buffer, ";\n");
  return buffer;
}

void extract_header_file(void) {
  log_warn("extract_header_file()");

  oc_compiler_state_t* compiler_state = make_oc_compiler_state();
  value_array_t* parsed_files = parse_files(FLAG_files);

  buffer_t* includes = make_buffer(1024);
#if 0
  buffer_t* defines = make_buffer(1024);
  buffer_t* typedefs = make_buffer(1024);
  buffer_t* enums = make_buffer(1024);
  buffer_t* structure_forward_declarations = make_buffer(1024);
  buffer_t* structures = make_buffer(1024);
#endif /* 0 */
  buffer_t* prototypes = make_buffer(1024);
#if 0
  buffer_t* inlines = make_buffer(1024);
  buffer_t* globals = make_buffer(1024);
#endif /* 0 */

  for (int i = 0; i < FLAG_files->length; i++) {
    oc_file_t* file = (oc_file_t*) value_array_get(parsed_files, i).ptr;
    TSNode root_node = ts_tree_root_node(file->tree);
    oc_node_t* root_oc_node
        = ts_node_to_oc_node(root_node, file->data, FLAG_include_unnamed_nodes);
    for (int i = 0; i < root_oc_node->children->length; i++) {
      oc_node_t* node
          = (oc_node_t*) value_array_get(root_oc_node->children, i).ptr;
      log_warn("Node tag is %s\n", oc_node_tag_to_string(node->tag));
      switch (node->tag) {

      case OC_NODE_PREPROC_INCLUDE:
        includes = append_oc_node_text(includes, node, 0);
        break;

      case OC_NODE_FUNCTION_DECLARATOR:
        prototypes = append_function_prototype(prototypes, node);
        break;

      default:
        break;
      }
    }
    /*
    output = append_prototypes(output, node)
    output = append_oc_node_tree(output, node);
    // fprintf(stdout, "%s\n", buffer_to_c_string(output));
    */
  }

  fprintf(stderr, "/* Includes ...*/%s\n", buffer_to_c_string(includes));

  fprintf(stderr, "/* Function Declarations ...*/%s\n",
          buffer_to_c_string(prototypes));
}

void configure_build_command(void);
void configure_print_parse_tree_command(void);
void configure_extract_header_file_command(void);

void configure_flags() {
  flag_program_name("omni-c");
  flag_description("omni-c is a transpiler for the omni-c language.");

  configure_build_command();
  configure_print_parse_tree_command();
  configure_extract_header_file_command();
}

void configure_build_command(void) {
  flag_command("build", &FLAG_command);
  flag_file_args(&FLAG_files);
}

void configure_print_parse_tree_command(void) {
  flag_command("print-parse-trees", &FLAG_command);
  flag_boolean("--show-unnamed-nodes", &FLAG_include_unnamed_nodes);
  flag_file_args(&FLAG_files);
}

void configure_extract_header_file_command(void) {
  flag_command("extract-header-file", &FLAG_command);
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
  } else if (string_equal("extract-header-file", FLAG_command)) {
    extract_header_file();
  } else if (string_equal("print-parse-trees", FLAG_command)) {
    print_parse_trees();
  } else {
    fprintf(stderr, "Uknown command: %s\n", FLAG_command);
  }

  exit(0);
}
