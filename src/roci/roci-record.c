typedef roci_record_t = struct {
  char* record_tag;
  uint64_t length;
  uint8_t* value_tags;
  uint64_t* values;
};

roci_value_t roci_make_record(char* record_tag, uint64_t length) {
  roci_value_t result = {0};

  roci_record_t* record = malloc_struct(roci_record_t);
  record->record_tag = record_tag;
  record->length = length;
  record->value_tags = malloc_bytes(length);
  record->values = cast(uint64_t*, malloc_bytes(length * sizeof(uint64_t)));
  for (int i = 0; i < 8; i++) {
    record->value_tags[i] = ROCI_TAG_BOOLEAN;
  }
  result.tag = ROCI_TAG_RECORD;
  result.raw = cast(uint64_t, record);
  return result;
}

roci_value_t roci_record_get(roci_record_t* record, int index) {
  if (index < 0 || index >= 8) {
    log_fatal("index must be between 0 and 7");
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  roci_value_t result = {0};
  result.tag = record->value_tags[index];
  result.raw = record->values[index];

  return result;
}

void roci_record_set(roci_record_t* record, int index, roci_value_t value) {
  if (index < 0 || index >= 8) {
    log_fatal("index must be between 0 and 7");
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  record->value_tags[index] = value.tag;
  record->values[index] = value.raw;
}
