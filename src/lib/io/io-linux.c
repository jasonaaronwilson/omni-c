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

/**
 * @function make_writable_if_exists
 *
 * Checks if a file exists and, if it does, adds write permissions for
 * the owner.
 */
void make_writable_if_exists(const char* file_name) {
  // Check if the file exists
  if (access(file_name, F_OK) != 0) {
    // File doesn't exist, so no need to change permissions
    return;
  }

  // Get current permissions
  struct stat file_stat;
  if (stat(file_name, &file_stat) != 0) {
    log_fatal("Error getting file status for %s: %s\n", file_name,
              strerror(errno));
    fatal_error(ERROR_ILLEGAL_STATE);
  }

  // Add write permission for the owner
  mode_t new_mode = file_stat.st_mode | S_IWUSR;

  if (chmod(file_name, new_mode) != 0) {
    log_fatal("Error setting permissions for %s: %s\n", file_name,
              strerror(errno));
    fatal_error(ERROR_ILLEGAL_STATE);
  }
}

/**
 * @function buffer_read_ready_bytes_file_number
 *
 * Read from a file_number until either the end of file is reached,
 * max_bytes has been read, or there are no ready bytes. This function
 * should never block.
 */
extern buffer_t* buffer_read_ready_bytes_file_number(buffer_t* buffer,
                                                     int file_number,
                                                     uint64_t max_bytes) {
  fcntl(file_number, F_SETFL, fcntl(file_number, F_GETFL) | O_NONBLOCK);

  uint64_t bytes_remaining = max_bytes - buffer_length(buffer);
  char read_buffer[1024];

  // Loop until either blocking would occur or max_bytes have been added
  while (bytes_remaining > 0) {
    int bytes_read = read(file_number, read_buffer, sizeof(read_buffer));
    if (bytes_read > 0) {
      for (int i = 0; i < bytes_read; i++) {
        buffer = buffer_append_byte(buffer, cast(uint8_t, read_buffer[i]));
        bytes_remaining--;
      }
    } else if (bytes_read == 0) {
      // End-of-file (write end of pipe closed)
      break;
    } else {
      // bytes_read < 0 (so presumably -1).
      if (errno != EAGAIN && errno != EWOULDBLOCK) {
        // A real error occurred
        log_fatal("Error reading from file descriptor %d: %s", file_number,
                  strerror(errno));
        fatal_error(ERROR_ILLEGAL_STATE);
      }
      // No data available without blocking, so break out of the loop
      break;
    }
  }

  return buffer;
}

// Return nullptr on error.
value_array_t* file_glob(const char* pattern) {
  value_array_t* result_array = make_value_array(10);

    if (result_array == NULL || pattern == NULL) {
      return nullptr;
    }

    glob_t sys_glob = {0};
    int status = glob(pattern, 0, NULL, &sys_glob);

    if (status == GLOB_NOMATCH) {
      return result_array;
    }
    if (status != 0) {
      return nullptr;
    }

    for (size_t i = 0; i < sys_glob.gl_pathc; ++i) {
      value_array_push(result_array, ptr_to_value(string_duplicate(sys_glob.gl_pathv[i])));
    }

    globfree(&sys_glob);
    return result_array;
}
