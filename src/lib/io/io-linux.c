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
int64_t get_file_modification_time(const char* filename) {
  struct stat result = compound_literal(struct stat, {0});

  if (stat(filename, &result) != 0) {
    // File doesn't exist or error occurred
    return -1;
  }

  int64_t total_microseconds = 0;

  total_microseconds = (cast(int64_t, result.st_mtim.tv_sec) * 1000000LL)
                       + (result.st_mtim.tv_nsec / 1000);

  return total_microseconds;
}

/**
 * @function make_file_read_only
 *
 * Changes permissions on a file to be "read-only".
 */
void make_file_read_only(char* file_name) {
  // Set file permissions to read-only
  if (chmod(file_name, S_IRUSR | S_IRGRP | S_IROTH) != 0) {
    log_fatal("Failed to set file permissions: %s", file_name);
    log_fatal("strerror(errno) = %s", strerror(errno));
    fatal_error(ERROR_ILLEGAL_STATE);
  }
}
