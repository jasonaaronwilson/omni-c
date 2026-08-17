/**
 * @function to_platform_path
 *
 * The mental model is that internal_path's are always "/" specified
 * so there is no need to change everything even if they contain '\\'
 * since "unix" loves '/'.
 *
 * This may or may not return a copy in future versions.
 */
char* to_platform_path(char* internal_path) {
  return internal_path;
}

/**
 * @function from_platform_path
 *
 * The mental model is that internal_path's are always "/" specified
 * so we just make sure '/' are converted to backslash ('\\' in C, aka
 * ASCII 92).
 *
 * This may or may not return a copy in future versions.
 */
char* from_platform_path(char* platform_path) {
  return platform_path;
}

/**
 * @file path.c
 *
 * IO Operations on paths (I think we should rename this).
 */

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
