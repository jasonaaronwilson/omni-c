/**
 * @file roci-buffer-registry
 *
 * For source level debugging with the roci debugger, we may need to
 * keep around temporary buffers. While we don't necessarily need to
 * keep around the contents forever (except for repl buffers), we do
 * need to assign each conceptual "file" a unique 32-bit integer to
 * keep buffer for the debug information that is embedded in the byte
 * code.
 */

typedef roci_buffer_info_t = struct {
  buffer_t* buffer;
  char* filename;
  uint32_t buffer_number;
};

value_array_t* roci_buffers = nullptr;

int32_t roci_register_buffer(buffer_t* buffer, char* filename) {
  if (roci_buffers == nullptr) {
    roci_buffers = make_value_array(10);
  }
  roci_buffer_info_t* info = get_buffer_info_by_filename(filename);
  if (info != nullptr) {
    log_fatal("Trying to register a duplicate filename %s", filename);
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  info = malloc_struct(roci_buffer_info_t);
  info->buffer = buffer;
  info->filename = filename;
  info->buffer_number = roci_buffers->length;
  value_array_add(roci_buffers, ptr_to_value(info));
  return info->buffer_number;
}

roci_buffer_info_t* get_buffer_info_by_filename(char* filename) {
  for (int i = 0; i < roci_buffers->length; i++) {
    roci_buffer_info_t* info
        = value_array_get_ptr(roci_buffers, i, typeof(roci_buffer_info_t*));
    if (string_equal(filename, info->filename)) {
      return info;
    }
  }
  return nullptr;
}

roci_buffer_info_t* get_buffer_info_by_number(uint32_t buffer_number) {
  for (int i = 0; i < roci_buffers->length; i++) {
    roci_buffer_info_t* info
        = value_array_get_ptr(roci_buffers, i, typeof(roci_buffer_info_t*));
    if (buffer_number == info->buffer_number) {
      return info;
    }
  }
  return nullptr;
}
