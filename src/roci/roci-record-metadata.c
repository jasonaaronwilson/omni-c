
int num_record_metadata;
roci_record_fields_t* all_record_metadata[1024] = {};

roci_record_fields_t* get_record_metadata_by_number(int number) {
  if (number < num_record_metadata) {
    return all_record_metadata[number];
  }
  log_fatal("record metadata number %d is too high", number);
  fatal_error(ERROR_ILLEGAL_STATE);
}

int save_record_metadata(roci_record_fields_t* metadata) {
  if (num_record_metadata < 1024) {
    int result = num_record_metadata;
    all_record_metadata[num_record_metadata++] = metadata;
    return result;
  }
  log_fatal("Too many record types have been declared.");
  fatal_error(ERROR_ILLEGAL_STATE);
}

boolean_t match_record_metadata(roci_record_t* record, int number) {
  if (number < num_record_metadata) {
    return all_record_metadata[number] == record->metadata;
  }
  log_fatal("record metadata number %d is too high", number);
  fatal_error(ERROR_ILLEGAL_STATE);
}
