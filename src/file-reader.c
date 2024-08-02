#line 2 "file-reader.c"

/**
 * @file file-reader.c
 *
 * Handles reading "oc_file"'s (into memory - eventually memory
 * mapping might work...)
 */

#ifndef _FILE_READER_H_
#define _FILE_READER_H_

#include "oc-file.h"

#include "file-reader.c.generated.h"

#endif /* _FILE_READER_H_ */

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
    fprintf(stderr, "Reading %s\n", file_name);
    value_array_add(result, ptr_to_value(read_file(file_name)));
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
