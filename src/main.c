/**
 * This is the main routine for the omni-c transpiler.
 */

#include <stdio.h>

#include "debug-printer.h"
#include "file-reader.h"
#include "header-file-printer.h"
#include "lexer.h"
#include "parser.h"
#include "token-transformer.h"
#include <c-armyknife-lib.h>

#include "main.c.generated.h"

value_array_t* FLAG_files = NULL;
char* FLAG_command = NULL;
boolean_t FLAG_include_unnamed_nodes = false;

boolean_t FLAG_print_tokens_show_tokens = false;
boolean_t FLAG_print_tokens_include_whitespace = false;
boolean_t FLAG_print_tokens_include_comments = false;
boolean_t FLAG_print_tokens_parse_and_print = true;

char* FLAG_ouput_file = NULL;

void do_print_tokens(value_array_t* tokens, char* message) {
  if (FLAG_print_tokens_show_tokens) {
    // Experimentally, tokens when debug printed are pretty big no
    // matter the size of the actual token and some tokens like
    // comments can be quite large.
    buffer_t* buffer = make_buffer(tokens->length * 60);
    for (int j = 0; j < tokens->length; j++) {
      oc_token_t* token = token_at(tokens, j);
      buffer = append_token_debug_string(buffer, *token);
      buffer = buffer_append_string(buffer, "\n");
    }
    fprintf(stdout, "** %s **\n%s\n", message, buffer_to_c_string(buffer));
  }
}

void print_tokens(void) {
  log_info("print_tokens()");

  value_array_t* files = read_files(FLAG_files);
  for (int i = 0; i < FLAG_files->length; i++) {
    oc_file_t* file = (oc_file_t*) value_array_get(files, i).ptr;

    fprintf(stdout, "====================================================\n");
    fprintf(stdout, "====> Processing %s\n", file->file_name);
    fprintf(stdout, "====================================================\n");

    oc_tokenizer_result_t tokenizer_result = tokenize(file->data);

    if (tokenizer_result.tokenizer_error_code) {
      log_warn("Tokenizer error: \"%s\"::%d -- %d",
               value_array_get(FLAG_files, i).str,
               tokenizer_result.tokenizer_error_position,
               tokenizer_result.tokenizer_error_code);
      continue;
    }

    value_array_t* tokens = tokenizer_result.tokens;
    do_print_tokens(tokens, "before xform tokens");

    tokens = transform_tokens(
        tokens, (token_transformer_options_t){
                    .keep_whitespace = FLAG_print_tokens_include_whitespace,
                    .keep_comments = FLAG_print_tokens_include_comments,
                    .keep_javadoc_comments = FLAG_print_tokens_include_comments,
                    .keep_c_preprocessor_lines = false,
                });

    do_print_tokens(tokens, "after xform tokens");

    if (FLAG_print_tokens_parse_and_print) {
      parse_result_t declarations = parse_declarations(tokens, 0);
      if (is_error_result(declarations)) {
        declarations.parse_error.file_name = file->file_name;
        buffer_t* buffer = make_buffer(1);
        buffer = buffer_append_human_readable_error(
            buffer, &(declarations.parse_error));
        log_fatal("%s", buffer_to_c_string(buffer));
        fatal_error(ERROR_ILLEGAL_STATE);
      } else {
        buffer_t* buffer = make_buffer(1024);
        buffer = buffer_append_dbg_parse_node(buffer, declarations.node, 0);
        fprintf(stdout, "** Parse Nodes %s **\n%s\n", file->file_name,
                buffer_to_c_string(buffer));
      }
    }
  }
}

//// TODO(jawilson):
//// boolean_t FLAG_prototype_empty_bodies = true;

boolean_t FLAG_unique_prototype_header_files = false;

__attribute__((warn_unused_result)) buffer_t*
    extract_enums_process_declarations(buffer_t* output,
                                       declarations_node_t* root) {
  uint64_t length = node_list_length(root->declarations);

  for (uint64_t i = 0; i < length; i++) {
    parse_node_t* node = node_list_get(root->declarations, i);
    if (node->tag == PARSE_NODE_TYPEDEF) {
      /*
      output = buffer_append_c_function_node_prototype(output, fn_node);
      output = buffer_printf(output, "\n");
      */
    } else if (node->tag == PARSE_NODE_ENUM) {
    }
  }

  return output;
}

__attribute__((warn_unused_result)) buffer_t*
    extract_prototypes_process_declarations(buffer_t* output,
                                            declarations_node_t* root) {
  uint64_t length = node_list_length(root->declarations);

  for (uint64_t i = 0; i < length; i++) {
    parse_node_t* node = node_list_get(root->declarations, i);
    if (node->tag == PARSE_NODE_FUNCTION) {
      function_node_t* fn_node = to_function_node(node);

      // Skip existing prototypes. We assume multiple prototypes are
      // allowed by ISO C is long as they are the same. This means
      // that we can extract prototypes and include them before
      // existing prototypes and the compiler will help make sure we
      // did this correctly before actually removing any
      // prototypes. We could also thus probably emit a prototype
      // for each prototype and besides having a duplicate (or more)
      // prototype, everything should technically be OK. Just
      // ignoring prototypes is certainly useful for debugging.
      if (fn_node->body == NULL) {
        continue;
      }

      // Normally prototypes aren't used for inline functions (and
      // normally they are declared static...) since the inline
      // functions often appears instead of their prototypes in a
      // header file. Honestly I haven't completely figured this out
      // yet. Skipping these prototypes for now if probably fine for
      // the demo especially since the wisely used ones will be be
      // static which we are skipping below anyways (for now).
      if (fn_node->function_specifier != NULL
          && token_matches(fn_node->function_specifier, "inline")) {
        continue;
      }
      // "static" for a "top-level" declaration means that the
      // declaration is not supposed to be visible outside the
      // compilation unit which sort of corresponds to a single "C"
      // file if the C pre-processor didn't exist -- or at least
      // that is where I eventually want to have Omni-C to
      // behave. At least for now, the simplest thing is to not try
      // to put them into any header files which we can revisit
      // later (along with static inline...)
      if (fn_node->storage_class_specifier != NULL
          && token_matches(fn_node->function_specifier, "static")) {
        continue;
      }

      output = buffer_append_c_function_node_prototype(output, fn_node);
      output = buffer_printf(output, "\n");
    }
  }

  return output;
}

void extract_command(char* command) {
  log_info("extract_prototypes(%s)", command);

  buffer_t* prototype_outputs = make_buffer(16 * 1024);

  value_array_t* files = read_files(FLAG_files);
  for (int i = 0; i < FLAG_files->length; i++) {
    if (FLAG_unique_prototype_header_files) {
      buffer_clear(prototype_outputs);
    }

    oc_file_t* file = (oc_file_t*) value_array_get(files, i).ptr;
    prototype_outputs
        = buffer_printf(prototype_outputs,
                        "/* Automatically extracted prototypes from %s */\n\n",
                        file->file_name);

    /*
    prototype_outputs = buffer_printf(prototype_outputs,
                           "#ifdef OMNI_C_INCLUDE_GENERATED_HEADER_FILES\n");
    */

    fprintf(stdout, "====================================================\n");
    fprintf(stdout, "====> Processing %s\n", file->file_name);
    fprintf(stdout, "====================================================\n");

    oc_tokenizer_result_t tokenizer_result = tokenize(file->data);

    if (tokenizer_result.tokenizer_error_code) {
      log_warn("Tokenizer error: \"%s\"::%d -- %d",
               value_array_get(FLAG_files, i).str,
               tokenizer_result.tokenizer_error_position,
               tokenizer_result.tokenizer_error_code);
      fatal_error(ERROR_ILLEGAL_INPUT);
    }

    value_array_t* tokens = tokenizer_result.tokens;
    tokens = transform_tokens(tokens, (token_transformer_options_t){
                                          .keep_whitespace = false,
                                          .keep_comments = false,
                                          .keep_javadoc_comments = false,
                                          .keep_c_preprocessor_lines = false,
                                      });

    parse_result_t declarations_result = parse_declarations(tokens, 0);
    if (is_error_result(declarations_result)) {
      declarations_result.parse_error.file_name = file->file_name;
      buffer_t* buffer = make_buffer(1);
      buffer = buffer_append_human_readable_error(
          buffer, &(declarations_result.parse_error));
      log_fatal("%s", buffer_to_c_string(buffer));
      fatal_error(ERROR_ILLEGAL_INPUT);
    }

    declarations_node_t* root = to_declarations_node(declarations_result.node);
    prototype_outputs
        = extract_prototypes_process_declarations(prototype_outputs, root);

    // prototype_outputs = buffer_printf(
    // prototype_outputs, "#endif /* OMNI_C_INCLUDE_GENERATED_HEADER_FILES
    // */\n");

    if (FLAG_unique_prototype_header_files) {
      char* prototype_outputs_file_name
          = string_printf("%s.generated.h", file->file_name);
      buffer_write_file(prototype_outputs, prototype_outputs_file_name);
      free_bytes(prototype_outputs_file_name);
    }
  }

  if (!FLAG_unique_prototype_header_files) {
    // TODO(jawilson): prototype_outputs to --prototype_outputs...
    fprintf(stdout, "%s", buffer_to_c_string(prototype_outputs));
  }
}

//// /**
////  * @function append_function_prototype
////  *
////  * Given an OC_NODE_FUNCTION_DECLARATOR, format this as a simple
////  * prototype (i.e., ignore the OC_NODE_COMPOUND_STATEMENT (aka
////  * "block")) at the end of it.
////  */
//// buffer_t* append_function_prototype(buffer_t* buffer, oc_node_t* node) {
////   buffer = buffer_append_string(buffer, "\n");
////   for (int i = 0; i < node->children->length; i++) {
////     oc_node_t* child = (oc_node_t*) value_array_get(node->children, i).ptr;
////     if (child->tag != OC_NODE_COMPOUND_STATEMENT) {
////       buffer = buffer_append_string(buffer, " ");
////       buffer = append_oc_node_text(buffer, child, 0);
////     }
////   }
////   buffer = buffer_append_string(buffer, ";\n");
////   return buffer;
//// }
////
//// char* struct_node_to_struct_name(oc_node_t* node) {
////   for (int i = 0; i < node->children->length; i++) {
////     oc_node_t* child = (oc_node_t*) value_array_get(node->children, i).ptr;
////     if (child->tag == OC_NODE_TYPE_IDENTIFIER) {
////       return child->text;
////     }
////   }
////   fatal_error(ERROR_ILLEGAL_STATE);
//// }
////
//// buffer_t* append_structure_forward_declarations(buffer_t* buffer,
////                                                 oc_node_t* node) {
////   buffer = buffer_append_string(buffer, "\nstruct ");
////   buffer = buffer_append_string(buffer, struct_node_to_struct_name(node));
////   buffer = buffer_append_string(buffer, "_S;");
////   return buffer;
//// }
////
//// #define structure_for_each_field(node, field_type_node_var, \
////                                  field_name_node_var, statements) \
////   do { \
////     for (int i = 0; i < node->children->length; i++) { \
////       oc_node_t* child = (oc_node_t*) value_array_get(node->children,
/// i).ptr;  \
////       if (child->tag == OC_NODE_FIELD_DECLARATION_LIST) { \
////         for (int j = 0; j < child->children->length; j++) { \
////           oc_node_t* field_node \
////               = (oc_node_t*) value_array_get(child->children, j).ptr; \
////           if (field_node->tag == OC_NODE_FIELD_DECLARATION) { \
////             do { \
////               oc_node_t* field_type_node_var \
////                   = (oc_node_t*) value_array_get(field_node->children,
/// 0).ptr; \
////               oc_node_t* field_name_node_var \
////                   = (oc_node_t*) value_array_get(field_node->children,
/// 1).ptr; \
////               statements; \
////             } while (0); \
////           } \
////         } \
////         break; \
////       } \
////     } \ /   } while (0)
////
////
//// char* type_node_to_string(oc_node_t* field_type_node) {
////   // OC_NODE_PRIMITIVE_TYPE
////   return field_type_node->text;
//// }
////
//// buffer_t* structure_dfs(oc_node_t* node, string_hashtable_t* struct_nodes,
////                         string_hashtable_t* struct_visit_status,
////                         buffer_t* structures) {
////
////   char* struct_name = struct_node_to_struct_name(node);
////
////   if (is_ok(string_ht_find(struct_visit_status, struct_name))) {
////     return structures;
////   }
////
////   struct_visit_status
////       = string_ht_insert(struct_visit_status, struct_name,
/// u64_to_value(1));
////
////   // clang-format off
////   structure_for_each_field(node, field_type_node, field_name_node, {
////       char* name = type_node_to_string(field_type_node);
////       value_result_t result = string_ht_find(struct_nodes, name);
////       if (is_ok(result)) {
//// 	structures = structure_dfs(cast(oc_node_t*, result.ptr), struct_nodes,
/// struct_visit_status, structures); /       } /     }); /   // clang-format on
////
////   structures = append_oc_node_text(structures, node, 0);
////
////   return structures;
//// }
////
//// buffer_t* structures_do_dfs(string_hashtable_t* struct_nodes,
////                             string_hashtable_t* struct_visit_status,
////                             buffer_t* structures) {
////   string_ht_foreach(struct_nodes, node_name, node, {
////     structures = structure_dfs(cast(oc_node_t*, node.ptr), struct_nodes,
////                                struct_visit_status, structures);
////   });
////   return structures;
//// }
////
//// void extract_header_file(void) {
////   log_warn("extract_header_file()");
////
////   if (1) {
////     print_parse_trees();
////   }
////
////   oc_compiler_state_t* compiler_state = make_oc_compiler_state();
////   value_array_t* parsed_files = parse_files(FLAG_files);
////
////   // TODO(jawilson): add a manifest at the begining
////
////   buffer_t* includes = make_buffer(1024);
////
//// #if 0
////   buffer_t* defines = make_buffer(1024);
////   buffer_t* typedefs = make_buffer(1024);
////   buffer_t* enums = make_buffer(1024);
//// #endif /* 0 */
////
////   buffer_t* structure_forward_declarations = make_buffer(1024);
////   buffer_t* structures = make_buffer(1024);
////   buffer_t* prototypes = make_buffer(1024);
////
//// #if 0
////   buffer_t* inlines = make_buffer(1024);
////   buffer_t* globals = make_buffer(1024);
//// #endif /* 0 */
////
////   // struct-name to node. This will allow us to do a DFS so that we
////   // output structures in the proper order.
////   string_hashtable_t* struct_nodes = make_string_hashtable(32);
////
////   for (int i = 0; i < FLAG_files->length; i++) {
////     oc_file_t* file = (oc_file_t*) value_array_get(parsed_files, i).ptr;
////     TSNode root_node = ts_tree_root_node(file->tree);
////     oc_node_t* root_oc_node
////         = ts_node_to_oc_node(root_node, file->data,
/// FLAG_include_unnamed_nodes); /     for (int i = 0; i <
/// root_oc_node->children->length; i++) { /       oc_node_t* node /           =
///(oc_node_t*) value_array_get(root_oc_node->children, i).ptr; / log_warn("Node
/// tag is %s\n", oc_node_tag_to_string(node->tag)); /       switch (node->tag)
/// {
////
////       case OC_NODE_PREPROC_INCLUDE:
////         includes = append_oc_node_text(includes, node, 0);
////         break;
////
////       case OC_NODE_FUNCTION_DECLARATOR:
////         prototypes = append_function_prototype(prototypes, node);
////         break;
////
////       case OC_NODE_STRUCT_SPECIFIER:
////         struct_nodes = string_ht_insert(
////             struct_nodes, struct_node_to_struct_name(node),
/// ptr_to_value(node)); /         structure_forward_declarations =
/// append_structure_forward_declarations( / structure_forward_declarations,
/// node); /         break;
////
////       default:
////         break;
////       }
////     }
////     /*
////     output = append_prototypes(output, node)
////     output = append_oc_node_tree(output, node);
////     // fprintf(stdout, "%s\n", buffer_to_c_string(output));
////     */
////   }
////
////   string_hashtable_t* struct_visit_status = make_string_hashtable(32);
////   structures = structures_do_dfs(struct_nodes, struct_visit_status,
/// structures);
////
////   fprintf(stderr, "\n/* Includes ... */\n%s\n",
/// buffer_to_c_string(includes));
////
////   fprintf(stderr, "\n/* Structure Forward Declarations ... */\n%s\n",
////           buffer_to_c_string(structure_forward_declarations));
////
////   fprintf(stderr, "\n/* Structure Declarations ... */\n%s\n",
////           buffer_to_c_string(structures));
////
////   fprintf(stderr, "\n/* Function Declarations ... */\n%s\n",
////           buffer_to_c_string(prototypes));
//// }
////
//// void configure_build_command(void);
//// void configure_print_parse_tree_command(void);
//// void configure_extract_header_file_command(void);
////

void configure_flags(void) {
  flag_program_name("omni-c");
  flag_description(
      "omni-c is a transpiler for the omni-c language as well as a code "
      "generation tool for ISO C.");

  // configure_build_command();
  // configure_print_parse_tree_command();
  // configure_extract_header_file_command();
  configure_print_tokens_command();
  configure_extract_prototypes_command();
}

//// void configure_build_command(void) {
////   flag_command("build", &FLAG_command);
////   flag_file_args(&FLAG_files);
//// }
////
//// void configure_print_parse_tree_command(void) {
////   flag_command("print-parse-trees", &FLAG_command);
////   flag_boolean("--show-unnamed-nodes", &FLAG_include_unnamed_nodes);
////   flag_file_args(&FLAG_files);
//// }
////
//// void configure_extract_header_file_command(void) {
////   flag_command("extract-header-file", &FLAG_command);
////   flag_file_args(&FLAG_files);
//// }

void configure_print_tokens_command(void) {
  flag_command("print-tokens", &FLAG_command);
  flag_boolean("--show-tokens", &FLAG_print_tokens_show_tokens);
  flag_boolean("--include-whitespace", &FLAG_print_tokens_include_whitespace);
  flag_boolean("--include-comments", &FLAG_print_tokens_include_comments);
  flag_boolean("--parse-and-print", &FLAG_print_tokens_parse_and_print);
  flag_file_args(&FLAG_files);
}

void configure_extract_prototypes_command(void) {
  flag_command("extract-prototypes", &FLAG_command);
  flag_string("--output-file", &FLAG_ouput_file);
  flag_boolean("--unique-prototype-header-files",
               &FLAG_unique_prototype_header_files);
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
    ////   } else if (string_equal("build", FLAG_command)) {
    ////     translate_and_build();
    ////   } else if (string_equal("extract-header-file", FLAG_command)) {
    ////     extract_header_file();
    ////   } else if (string_equal("print-parse-trees", FLAG_command)) {
    ////     print_parse_trees();
  } else if (string_equal("print-tokens", FLAG_command)) {
    print_tokens();
  } else if (string_equal("extract-prototypes", FLAG_command)) {
    extract_command("extract-prototypes");
  } else {
    fprintf(stderr, "Unknown command: %s\n", FLAG_command);
  }

  exit(0);
}
