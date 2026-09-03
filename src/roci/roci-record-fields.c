////
//// Dense run-time encoding of record fields plus the name of the
//// record type. Gemini called this the "shape" of the record.
////

typedef roci_record_fields_t = struct {
  roci_symid_t record_symid;
  uint32_t length;
  roci_symid_t symids[0];
};

roci_record_fields_t* make_roci_record_fields(roci_symid_t record_symid, uint32_t length, roci_symid_t* symids) {
  roci_record_fields_t* result = cast(roci_record_fields_t*, 
				      malloc_bytes(sizeof(roci_record_fields_t) + sizeof(roci_symid_t) * length));
  result->record_symid = record_symid;
  result->length = length;
  for (uint32_t i = 0; i < length; i++) {
    result->symids[i] = symids[i];
  }
  return result;
}

roci_record_fields_t* make_roci_record_fields_from_names(char* name,
							 uint32_t length, 
							 char** names) {
  roci_symid_t symids[length];
  for (uint32_t i = 0; i < length; i++) {
    symids[i] = roci_intern_as_symid(names[i]);
  }
  return make_roci_record_fields(roci_intern_as_symid(name), length, symids);
}

static inline uint32_t find_record_field_number(roci_record_fields_t* fields, 
						roci_symid_t field_symid) {
  for (uint32_t i = 0; i < fields->length; i++) {
    if (fields->symids[i] == field_symid) {
      return i;
    }
  }
  fatal_error(ERROR_ILLEGAL_STATE);
}

char* find_record_field_name(roci_record_fields_t* fields, uint32_t field_number) {
  if (field_number < fields->length) {
    return roci_symid_to_string(fields->symids[field_number]);
  }
  fatal_error(ERROR_ILLEGAL_STATE);
}
