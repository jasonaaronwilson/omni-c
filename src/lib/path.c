boolean_t path_is_directory(const char* path) {
  struct stat st;
  if (!path || stat(path, &st) != 0) {
    return false;
  }
  return S_ISDIR(st.st_mode);
}

boolean_t path_is_file(const char* path) {
  struct stat st;
  if (!path || stat(path, &st) != 0) {
    return false;
  }
  return S_ISREG(st.st_mode);
}
