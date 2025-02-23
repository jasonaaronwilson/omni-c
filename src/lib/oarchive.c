/**
 * @file oarchive.c
 *
 * This program implements a tool for creating, listing, appending,
 * and extracting archives in the Omni Archive File Format.
 */

#include <stdlib.h>

#define ARMYKNIFE_LIB_DEFAULT_LOG_LEVEL LOGGER_DEBUG
#define C_ARMYKNIFE_LIB_IMPL
#include "c-armyknife-lib-no-lines.h"

typedef enum { OVERWRITE_NO, OVERWRITE_YES, OVERWRITE_ASK } over_write_t;

/*
enum is_pipeline_t {
  PIPELINE_NO,
  PIPELINE_YES,
  PIPELINE_AUTO,
}
*/

boolean_t FLAG_verbose = 1;
over_write_t FLAG_overwrite = OVERWRITE_NO;
char* FLAG_output_file = NULL;
char* FLAG_input_file = NULL;
value_array_t* FLAG_files = NULL;
char* FLAG_command = NULL;

void configure_create_command(void);
void configure_list_command(void);
void configure_extract_command(void);

/*
is_pipeline_t FLAG_is_pipeline = PIPELINE_AUTO;
*/

void configure_flags() {
  flag_program_name("oarchive");

  flag_description(
      "oarchive can create, list, extract, append to, or join archives in the "
      "omni archive format (`.oar`).");

  // Global Flags
  flag_boolean("--verbose", &FLAG_verbose);
  flag_description("When true, output more information useful for debugging");

  flag_enum("--overwrite", (int*) &FLAG_overwrite);
  flag_description("Controls whether files can be over written or not.");
  flag_enum_value("no", OVERWRITE_NO);
  flag_enum_value("yes", OVERWRITE_YES);
  flag_enum_value("ask", OVERWRITE_ASK);

  /*
  flag_enum("--is-pipeline", &FLAG_is_pipline);
  flag_description("...");
  flag_enum_boolean_values(PIPELINE_NO, PIPELINE_YES);
  flag_enum_value("auto", PIPELINE_AUTO);
  */

  configure_create_command();
  configure_list_command();
  configure_extract_command();
}

void configure_create_command(void) {
  flag_command("create", &FLAG_command);
  flag_alias("c");

  flag_description("create an archive from the given files");

  flag_file_args(&FLAG_files);

  flag_string("--output-file", &FLAG_output_file);
  flag_alias("--output");
  flag_alias("-o");
  flag_description(
      "The destination for the created archive (stdout is used if "
      "--output-file is not specified)");
}

void configure_list_command(void) {
  flag_command("list", &FLAG_command);
  flag_alias("l");

  flag_description("list the files in the given archive");

  flag_file_args(&FLAG_files);

  flag_string("--input-file", &FLAG_input_file);
  flag_alias("--input");
  flag_alias("-i");
  flag_description(
      "The input archive file path (stdin is used if --input-file is not "
      "specified)");
}

void configure_extract_command(void) {
  flag_command("extract", &FLAG_command);
  flag_alias("x");

  flag_description("extract the file in the given archive");

  flag_file_args(&FLAG_files);

  flag_string("--input-file", &FLAG_input_file);
  flag_alias("--input");
  flag_alias("-i");
  flag_description(
      "The input archive file path (stdin is used if --input-file is not "
      "specified)");
}

void append_header_and_file_contents(FILE* out, char* filename) {
  buffer_t* contents = make_buffer(1);
  contents = buffer_append_file_contents(contents, filename);
  fprintf(out, "filename=%s", filename);
  fputc(0, out);
  fprintf(out, "size=%d", contents->length);
  fputc(0, out);
  fputc(0, out);
  for (uint64_t i = 0; i < contents->length; i++) {
    fputc(buffer_get(contents, i), out);
  }
}

string_tree_t* read_header(FILE* in) {
  string_tree_t* metadata = NULL;
  while (!feof(in)) {
    if (file_peek_byte(in) == '\0') {
      fgetc(in);
      break;
    }
    // TODO(jawilson): If there is an illegal header line without an
    // =, this won't work very well.
    buffer_t* key = make_buffer(8);
    key = buffer_read_until(key, in, '=');
    buffer_t* value = make_buffer(8);
    value = buffer_read_until(value, in, '\0');
    if (key->length == 0 && value->length == 0) {
      return metadata;
    }
    metadata = string_tree_insert(metadata, buffer_to_c_string(key),
                                  str_to_value(buffer_to_c_string(value)));
  }
  return metadata;
}

/**
 * @typedef
 *
 * Defines the callback type signature for stream_members (which is
 * used for processing an archive while streaming it).
 */
typedef boolean_t (*stream_headers_callback_t)(FILE* input,
                                               string_tree_t* metadata,
                                               int64_t size,
                                               void* callback_data);

void log_metadata(string_tree_t* metadata) {
  log_info("Logging metadata");
  if (should_log_info()) {
    string_tree_foreach(metadata, key, value,
                        { log_info("'%s' = '%s'", key, value.str); });
  }
}

/**
 * A callback based interface for processing an archive while streaming
 * it.
 */
void stream_members(FILE* in, stream_headers_callback_t callback,
                    void* callback_data) {
  while (!file_eof(in)) {
    string_tree_t* metadata = read_header(in);
    log_metadata(metadata);
    int64_t size = 0;
    value_result_t size_value = string_tree_find(metadata, "size");
    if (!is_ok(size_value)) {
      log_warn("Encounterd a header without an explicit size.");
    } else {
      value_result_t data_size = string_parse_uint64_dec(size_value.str);
      if (!is_ok(data_size)) {
        log_fatal("Encounterd a header with an unparseable size %s",
                  size_value.str);
        fatal_error(ERROR_FATAL);
      } else {
        size = data_size.u64;
      }
    }

    // ---------------------------------------------------------------
    boolean_t skip_data = callback(in, metadata, size, callback_data);
    // ---------------------------------------------------------------

    if (skip_data && size > 0) {
      log_info("Skipping %lu\n", size);
      // fseek(in, size, SEEK_CUR);
      file_skip_bytes(in, size);
    }
  }
}

void create_command(void) {
  log_info("create_command");

  FILE* out = stdout;
  if (FLAG_output_file != NULL) {
    out = fopen(FLAG_output_file, "w");
  }

  for (int i = 0; i < FLAG_files->length; i++) {
    append_header_and_file_contents(out, value_array_get(FLAG_files, i).str);
  }

  if (FLAG_output_file != NULL) {
    fclose(out);
  }
}

boolean_t list_command_callback(FILE* input, string_tree_t* metadata,
                                int64_t size, void* callback_data) {
  value_result_t filename_value = string_tree_find(metadata, "filename");
  if (is_ok(filename_value)) {
    fprintf(stdout, "%s\n", filename_value.str);
  } else {
    fprintf(stdout, "%s\n", "<<member lacks filename>>");
  }
  return true;
}

void list_command(void) {
  log_info("list_command");
  FILE* in = stdin;

  if (FLAG_input_file != NULL) {
    log_info("opening %s", FLAG_input_file);
    // TODO(jawilson): safe file_open instead.
    in = fopen(FLAG_input_file, "r");
  }

  stream_members(in, &list_command_callback, NULL);

  if (FLAG_input_file != NULL) {
    fclose(in);
  }
}

boolean_t extract_command_callback(FILE* input, string_tree_t* metadata,
                                   int64_t size, void* callback_data) {
  value_result_t filename_value = string_tree_find(metadata, "filename");
  if (is_ok(filename_value)) {
    log_info("Extracting %s", filename_value.str);
    FILE* output = fopen(filename_value.str, "w");
    file_copy_stream(input, output, false, size);
    fclose(output);
  }
  return false;
}

void extract_command(void) {
  log_info("list_command");
  FILE* in = stdin;
  if (FLAG_input_file != NULL) {
    log_info("opening %s", FLAG_input_file);
    // TODO(jawilson): safe file_open instead.
    in = fopen(FLAG_input_file, "r");
  }

  stream_members(in, &extract_command_callback, NULL);

  if (FLAG_input_file != NULL) {
    fclose(in);
  }
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
  } else if (string_equal("create", FLAG_command)) {
    create_command();
  } else if (string_equal("list", FLAG_command)) {
    list_command();
  } else if (string_equal("extract", FLAG_command)) {
    extract_command();
  } else {
    fatal_error(ERROR_BAD_COMMAND_LINE);
  }

  exit(0);
}
