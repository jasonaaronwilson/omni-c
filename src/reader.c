oc_file_t* read_file(char* file_name) {
  buffer_t* buffer = make_buffer(1024 * 8);
  buffer = buffer_append_file_contents(buffer, file_name);
  char* str = buffer_to_c_string(buffer);
  int length = buffer->length;
  oc_file_t* result = malloc_struct(oc_file_t);
  result->tag = STD_C_SOURCE_FILE;
  result->file_name = file_name;
  result->data = buffer;
  return result;
}
