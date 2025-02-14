/* ================================================================ */

typedef byte_stream_source_t = struct {
  // Read one byte. If a byte was available, set the second argument
  // to true, otherwise false.
  fn_t(uint8_t, byte_stream_source_t*, boolean_t*) read_byte;
  void* data;
};

typedef byte_stream_target_t = struct {
  fn_t(byte_stream_target_t*, byte_stream_target_t*, uint8_t) write_byte;
  void* data;
};

/* ================================================================ */

byte_stream_source_t* buffer_to_byte_source(buffer_t* buffer) {
  byte_stream_source_t* result = malloc_struct(byte_stream_source_t);
  result->read_byte = &buffer_stream_source_read;
  buffer_byte_stream_source_data_t* data = malloc_struct(buffer_byte_stream_source_data_t);
  data->buffer = buffer;
  data->position = 0;
  result->data = cast(void*, data);
  return result;
}

typedef buffer_byte_stream_source_data_t = struct {
  buffer_t* buffer;
  uint64_t position;
};

uint8_t buffer_stream_source_read(byte_stream_source_t* source, boolean_t* has_byte) {
  buffer_byte_stream_source_data_t* data = cast(buffer_byte_stream_source_data_t*, source->data);
  uint8_t result = 0;
  if (data->position < buffer_length(data->buffer)) {
    result = buffer_get(data->buffer, data->position++);
    *has_byte = true;
  } else {
    *has_byte = false;
  }
  return result;
}

/* ================================================================ */

byte_stream_source_t* cstring_to_byte_source(char* string) {
  byte_stream_source_t* result = malloc_struct(byte_stream_source_t);
  result->read_byte = &cstring_stream_source_read;
  cstring_byte_stream_source_data_t* data = malloc_struct(cstring_byte_stream_source_data_t);
  data->string = string;
  data->length = strlen(string);
  data->position = 0;
  result->data = cast(void*, data);
  return result;
}

typedef cstring_byte_stream_source_data_t = struct {
  char* string;
  uint64_t length;
  uint64_t position;
};

uint8_t cstring_stream_source_read(byte_stream_source_t* source, boolean_t* has_byte) {
  cstring_byte_stream_source_data_t* data = cast(cstring_byte_stream_source_data_t*, source->data);
  uint8_t result = 0;
  if (data->position < data->length) {
    result = data->string[data->position++];
    *has_byte = true;
  } else {
    *has_byte = false;
  }
  return result;
}

/* ================================================================ */

byte_stream_target_t* buffer_to_byte_target(buffer_t* buffer) {
  byte_stream_target_t* result = malloc_struct(byte_stream_target_t);
  result->write_byte = &buffer_stream_target_write;
  result->data = cast(void*, buffer);
  return result;
}

byte_stream_target_t* buffer_stream_target_write(byte_stream_target_t* target, uint8_t byte) {
  buffer_t* buffer = cast(buffer_t*, target->data);
  buffer_append_byte(buffer, byte);
  return target;
}
