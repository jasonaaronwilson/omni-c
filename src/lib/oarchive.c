/**
 * @file oarchive.c
 *
 * This is a small library for reading and writing "omni archives."
 *
 * See: https://github.com/jasonaaronwilson/omni-archive
 */

/**
 * @function oarchive_append_header_and_file_contents
 *
 * Given a filename and a writable file pointer, open the filename for
 * reading, append the correct header including at least the filename
 * and size, and then append the file contents of filename to the
 * writable file.
 */
void oarchive_append_header_and_file_contents(FILE* out, char* filename) {
  buffer_t* contents = make_buffer(1);
  contents = buffer_append_file_contents(contents, filename);
  fprintf(out, "filename=%s", filename);
  fputc(0, out);
  fprintf(out, "size=%d", contents->length);
  fputc(0, out);
  fputc(0, out);
  // Surely by now we have a call for this...
  for (uint64_t i = 0; i < contents->length; i++) {
    fputc(buffer_get(contents, i), out);
  }
}

/**
 * @function oarchive_read_header
 *
 * Read a member header into a string_tree_t. Normally you would use
 * oarchive_stream_members when reading an archive.
 */
string_tree_t* oarchive_read_header(FILE* in) {
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
typedef oarchive_stream_headers_callback_t = 
  fn_t(
       // Return true if stream_members should skip to the next header
       // or false if the entire size number of bytes have been read
       // already.
       boolean_t,
       // input file stream
       FILE*,
       // The parsed metadata
       string_tree_t*,
       // The size in bytes of the member
       int64_t,
       // The user definable callback_data param
       void*);

/**
 * @function oarchive_stream_members
 *
 * Process all of the members of an archive invoking the callback for
 * each member.
 */
void oarchive_stream_members(FILE* in, 
			     oarchive_stream_headers_callback_t callback,
			     void* callback_data) {
  while (!file_eof(in)) {
    string_tree_t* metadata = oarchive_read_header(in);
    // log_metadata(metadata);
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
      log_none("Skipping %lu\n", size);
      // fseek(in, size, SEEK_CUR);
      file_skip_bytes(in, size);
    }
  }
}
