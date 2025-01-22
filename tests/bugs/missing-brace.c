char* gen_enum_1717999267to_string(enum gen_enum_1717999267) {
  switch (value) {
    case ENUM_VALUE_1897738825:
return "ENUM_VALUE_1897738825";
  default:
          return "<<unknown-gen_enum_1717999267%s>>";
  }
// This missing "}" actually hung 

enum gen_enum_1717999267string_to_gen_enum_1717999267(char* value) {
  if (strcmp(value, "ENUM_VALUE_1897738825") == 0) {
return ENUM_VALUE_1897738825;
  }
  return 0;
}

enum_metadata_t* gen_enum_1717999267_metadata() {
    static enum_element_metadata_t var_0 = (enum_element_metadata_t) {
        .next = NULL,
        .name = "ENUM_VALUE_1897738825",
        .value = ENUM_VALUE_1897738825
    };
    static enum_metadata_t enum_metadata_result = (enum_metadata_t) {
        .name = "enum gen_enum_1717999267",
        .elements = &var_0
    };
    return &enum_metadata_result;
}

