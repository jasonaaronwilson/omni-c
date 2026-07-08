/**
 * @function get_file_modification_time
 *
 * Gets the last modification time of a file in microseconds.
 *
 * @param filename The path to the file.
 *
 * @return The modification time in microseconds since the epoch,
 * or -1 if the file does not exist or cannot be read.
 */
int64_t get_file_modification_time(const char *filename) {
  struct stat result = compound_literal(struct stat, {0});

  if (stat(filename, &result) != 0) {
    // File doesn't exist or error occurred
    return -1;
  }

  int64_t total_microseconds = 0;

  total_microseconds = (cast(int64_t, result.st_mtim.tv_sec) * 1000000LL) +
    (result.st_mtim.tv_nsec / 1000);

  return total_microseconds;
}
