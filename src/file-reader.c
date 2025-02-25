/**
 * @file file-reader.c
 *
 * Converts an array of file-names into an array of "oc_file"'s
 * (essentially loading each file in memory as a (byte) buffer) so
 * that they can be parsed and processed later.
 *
 * When a filename ends in ".oar", the file is treated as an
 * "omni-archive" and the files inside of the archive are also added
 * to the resulting array. Currently this expansion is not done
 * recursively so you can't have an ".oar" file inside of an
 * ".oar".
 *
 * This design should allow using "memory mapped files" at some point
 * (especially on linux).
 */

/**
 * @function read_files
 *
 * Read the given files using read_file into a single array.
 */
value_array_t* read_files(value_array_t* files) {
  fprintf(stderr, "Parsing %d files...\n", files->length);
  value_array_t* result = make_value_array(files->length);
  for (int i = 0; i < files->length; i++) {
    char* file_name = value_array_get(files, i).str;
    if (string_ends_with(file_name, ".oar")) {
      fprintf(stderr, "Reading archive %s\n", file_name);
      add_all_oarchive_members(result, file_name);
    } else {
      fprintf(stderr, "Reading %s\n", file_name);
      value_array_add(result, ptr_to_value(read_file(file_name)));
    }
  }
  return result;
}

/**
 * @function read_file
 *
 * Read a file from the file-system into memory and put a nice wrapper
 * around the bytes in that file.
 */
file_t* read_file(char* file_name) {
  file_t* result = malloc_struct(file_t);

  buffer_t* buffer = make_buffer(1024 * 8);
  buffer = buffer_append_file_contents(buffer, file_name);

  // TODO(jawilson): deduce from the file extension
  result->tag = STD_C_SOURCE_FILE;
  result->file_name = file_name;
  result->data = buffer;
  return result;
}

void add_all_oarchive_members(value_array_t* result, char* archive_file_name) {
  FILE* in = fopen(archive_file_name, "r");
  oarchive_stream_members(in, &add_orachive_file, result);
  fclose(in);
}

boolean_t add_orachive_file(FILE* input, string_tree_t* metadata, int64_t size,
                            void* callback_data) {
  value_result_t filename_value = string_tree_find(metadata, "filename");
  if (is_ok(filename_value)) {
    char* file_name = filename_value.str;
    log_info("Extracting %s from archive", file_name);
    file_t* read_file = malloc_struct(file_t);
    buffer_t* buffer = make_buffer(size);

    // TODO(jawilson): efficiency? Portability?
    while (size-- > 0) {
      int b = fgetc(input);
      buffer_append_byte(buffer, b);
    }

    // TODO(jawilson): deduce from the file extension
    read_file->tag = STD_C_SOURCE_FILE;
    read_file->file_name = file_name;
    read_file->data = buffer;
    value_array_add(cast(value_array_t*, callback_data),
                    ptr_to_value(read_file));
  } else {
    log_fatal("There is no filename for an omni-archive memember.");
    log_fatal(
        "Perhaps the .oar file isn't properly formed or created by omni-c "
        "archive?");
    fatal_error(ERROR_ILLEGAL_INPUT);
  }
  // We properly read all of the bytes so oarchive_stream_members
  // doesn't need to try to skip. (In fact if we read any byte, skip
  // won't work as currently implemented...)
  return false;
}
