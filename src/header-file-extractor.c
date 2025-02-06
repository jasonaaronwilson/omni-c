// This is presumbably dead!

static inline char* remove_type_suffix(char* typename) {
  if (string_ends_with(typename, "_t")) {
    return string_substring(typename, 0, strlen(typename) - 2);
  } else {
    return typename;
  }
}
