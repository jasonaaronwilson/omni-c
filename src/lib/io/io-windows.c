// Moves these for consistency (omni-c deduplicates so not harmful).

/* ====================================================================== */
/* File Access & Low-Level I/O                                            */
/* ====================================================================== */

int access(const char *path, int mode) {
  return _access(path, mode);
}

int read(int fd, void *buffer, unsigned int count) {
  return _read(fd, buffer, count);
}

int write(int fd, const void *buffer, unsigned int count) {
  return _write(fd, buffer, count);
}

int close(int fd) {
  return _close(fd);
}

int isatty(int fd) {
  return _isatty(fd);
}

/* ====================================================================== */
/* Process ID                                                             */
/* ====================================================================== */

int getpid(void) {
  return _getpid();
}

/* ====================================================================== */
/* Directory Navigation                                                  */
/* ====================================================================== */

char *getcwd(char *buffer, int maxlen) {
  return _getcwd(buffer, maxlen);
}

int chdir(const char *dirname) {
  return _chdir(dirname);
}

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
  WIN32_FILE_ATTRIBUTE_DATA file_info;

  if (!GetFileAttributesExA(filename, GetFileExInfoStandard, &file_info)) {
    // File doesn't exist, is locked, or error occurred
    return -1;
  }

  FILETIME ft = file_info.ftLastWriteTime;

  // Combine high and low 32-bit DWORDs into a 64-bit integer.
  // FILETIME is in 100-nanosecond intervals since Jan 1, 1601.
  uint64_t intervals = (cast(uint64_t, ft.dwHighDateTime) << 32) | ft.dwLowDateTime;

  // Difference in 100-nanosecond intervals between 1601-01-01 and 1970-01-01
  const uint64_t epoch_offset_intervals = 116444736000000000ULL;

  if (intervals < epoch_offset_intervals) {
    return -1;
  }

  // Convert from 100-nanosecond units to microseconds (divide by 10)
  int64_t total_microseconds = cast(int64_t, (intervals - epoch_offset_intervals) / 10ULL);

  return total_microseconds;
}

#define STACK_READ_BUFFER_SIZE 4096

/**
 * @function buffer_read_ready_bytes_handle
 *
 * Reads immediately available bytes from a Windows HANDLE into the buffer.
 * Designed to work with anonymous pipes (from subprocesses), but can be
 * extended for files and sockets.
 */
void buffer_read_ready_bytes_handle(buffer_t* buffer, HANDLE handle, uint32_t max_bytes) {
  if (handle == NULL || buffer == NULL) {
    return;
  }

  DWORD bytes_available = 0;
  DWORD file_type = GetFileType(handle) & ~FILE_TYPE_REMOTE;

  if (file_type == FILE_TYPE_PIPE) {
    // For pipes, ask Windows how many bytes are sitting in the buffer
    BOOL peek_success = PeekNamedPipe(handle, NULL, 0, NULL, &bytes_available, NULL);

    if (!peek_success) {
      DWORD err = GetLastError();
      if (err == ERROR_BROKEN_PIPE) {
        // The child process closed its end of the pipe. Normal exit condition.
        return;
      }
      log_fatal("PeekNamedPipe failed with error %d", err);
      fatal_error(ERROR_ILLEGAL_STATE);
      return;
    }
  } else {
    // Fallback for standard files
    bytes_available = STACK_READ_BUFFER_SIZE;
  }

  // If there's nothing to read, return immediately to avoid blocking
  if (bytes_available == 0) {
    return;
  }

  DWORD bytes_to_read = bytes_available;

  // Cap the read at our stack buffer size
  if (bytes_to_read > STACK_READ_BUFFER_SIZE) {
    bytes_to_read = STACK_READ_BUFFER_SIZE;
  }

  // Cap the read at the requested max limit
  if (bytes_to_read > max_bytes) {
    bytes_to_read = max_bytes;
  }

  uint8_t read_buffer[STACK_READ_BUFFER_SIZE] = {0};
  DWORD bytes_read = 0;

  BOOL read_success = ReadFile(
      handle,
      read_buffer,
      bytes_to_read,
      &bytes_read,
      NULL
  );

  if (read_success && bytes_read > 0) {
    // Safely copy into the opaque buffer abstraction byte-by-byte
    for (DWORD i = 0; i < bytes_read; i++) {
      buffer_append_byte(buffer, cast(uint8_t, read_buffer[i]));
    }
  } else if (!read_success) {
    DWORD err = GetLastError();
    // ERROR_BROKEN_PIPE is expected when the pipe closes cleanly during a read
    if (err != ERROR_BROKEN_PIPE && err != ERROR_MORE_DATA) {
      log_fatal("ReadFile failed with error %d", err);
      fatal_error(ERROR_ILLEGAL_STATE);
    }
  }
}

boolean_t already_changed_timeslice = false;

void shorten_process_timeslice(void) {
  if (already_changed_timeslice) {
    return;
  }
  timeBeginPeriod(1);
  atexit(reset_process_timeslice);
  already_changed_timeslice = true;
}

void reset_process_timeslice(void) {
  timeEndPeriod(1);
}

unsigned int sleep(unsigned int seconds) {
  shorten_process_timeslice();
    Sleep(seconds * 1000);
    return 0;
}

// Technically uint64_t would allow longer sleep times but Gemini
// thinks most POSIX implementations define useconds_t as uint32_t
int usleep(uint32_t usec) {
  shorten_process_timeslice();

  if (usec == 0) {
    Sleep(0); /* Yield remainder of time slice */
    return 0;
  }

  if (usec < 1000) {
    // Just busy wait. There is a better way of course but we are
    // going for simple right now.
    LARGE_INTEGER freq = {0};
    LARGE_INTEGER start = {0};
    LARGE_INTEGER current = {0};
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&start);

    long long ticks = (usec * freq.QuadPart) / 1000000;

    do {
      _mm_pause(); // Yields pipeline execution resources
      QueryPerformanceCounter(&current);
    } while ((current.QuadPart - start.QuadPart) < ticks);

    return 0;
  } else {
    /* Convert microseconds to milliseconds, rounding up so non-zero
       durations less than 1000us still sleep for at least 1ms. */
    DWORD ms = cast(DWORD, (usec + 999) / 1000);
    Sleep(ms);
    return 0;
  }
}

void make_file_read_only(char* file_name) {
    DWORD attributes = GetFileAttributesA(file_name);
    if (attributes == INVALID_FILE_ATTRIBUTES) {
        log_fatal("Failed to get file attributes: %s", file_name);
        fatal_error(ERROR_ILLEGAL_STATE);
    }

    if (!SetFileAttributesA(file_name, attributes | FILE_ATTRIBUTE_READONLY)) {
        log_fatal("Failed to set file read-only attribute: %s", file_name);
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
    DWORD attributes = GetFileAttributesA(file_name);

    /* File doesn't exist */
    if (attributes == INVALID_FILE_ATTRIBUTES) {
        return;
    }

    /* Strip the READONLY attribute if it is set */
    if (attributes & FILE_ATTRIBUTE_READONLY) {
        DWORD new_attributes = attributes & ~cast(DWORD, FILE_ATTRIBUTE_READONLY);
        if (!SetFileAttributesA(file_name, new_attributes)) {
            log_fatal("Error setting permissions for %s\n", file_name);
            fatal_error(ERROR_ILLEGAL_STATE);
        }
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
    uint64_t bytes_remaining = max_bytes - buffer_length(buffer);
    char read_buffer[1024] = {0};

    intptr_t os_handle = _get_osfhandle(file_number);
    if (os_handle == -1) {
        log_fatal("Invalid file descriptor %d", file_number);
        fatal_error(ERROR_ILLEGAL_STATE);
    }

    HANDLE h_file = cast(HANDLE, os_handle);

    while (bytes_remaining > 0) {
        DWORD avail_bytes = 0;
        BOOL peek_ok = PeekNamedPipe(h_file, NULL, 0, NULL, &avail_bytes, NULL);

        if (!peek_ok) {
            DWORD last_err = GetLastError();
            /* ERROR_BROKEN_PIPE indicates the write end closed (EOF) */
            if (last_err == ERROR_BROKEN_PIPE) {
                break;
            }
            log_fatal("Error peeking pipe on descriptor %d (error %lu)", file_number, last_err);
            fatal_error(ERROR_ILLEGAL_STATE);
        }

        /* No bytes ready to read without blocking */
        if (avail_bytes == 0) {
            break;
        }

        /* Determine chunk size to read without blocking */
        uint32_t chunk_size = sizeof(read_buffer);
        if (cast(uint64_t, chunk_size) > bytes_remaining) {
            chunk_size = cast(uint32_t, bytes_remaining);
        }
        if (cast(uint64_t, chunk_size) > cast(uint64_t, avail_bytes)) {
            chunk_size = cast(uint32_t, avail_bytes);
        }

        DWORD bytes_read = 0;
        BOOL read_ok = ReadFile(h_file, read_buffer, chunk_size, &bytes_read, NULL);

        if (!read_ok || bytes_read == 0) {
            break;
        }

        for (uint32_t i = 0; i < bytes_read; i++) {
            buffer = buffer_append_byte(buffer, cast(uint8_t, read_buffer[i]));
            bytes_remaining--;
        }
    }

    return buffer;
}

// Return nullptr when an error occurs and an empty array if no error.
value_array_t* file_glob(const char* pattern) {
    if (pattern == NULL) {
        return nullptr;
    }

    value_array_t* result_array = make_value_array(10);

    WIN32_FIND_DATAA find_data = {0};
    HANDLE h_find = FindFirstFileA(pattern, &find_data);

    if (h_find == INVALID_HANDLE_VALUE) {
        DWORD err = GetLastError();
        if (err == ERROR_FILE_NOT_FOUND || err == ERROR_PATH_NOT_FOUND) {
            return result_array;
        }
        return nullptr;
    }

    /* Extract directory prefix if pattern contained a path (e.g. "dir/*.txt") */
    char dir_prefix[1024] = {0};
    char* last_slash = strrchr(pattern, '/');
    if (last_slash == NULL) {
        last_slash = strrchr(pattern, '\\');
    }

    if (last_slash != NULL) {
        size_t prefix_len = cast(size_t, (last_slash - pattern) + 1);
        if (prefix_len < sizeof(dir_prefix)) {
            strncpy(dir_prefix, pattern, prefix_len);
            dir_prefix[prefix_len] = '\0';
        }
    }

    int keep_searching = 1;
    while (keep_searching) {
        /* Skip '.' and '..' pseudo-directories */
        if (strcmp(find_data.cFileName, ".") != 0 && strcmp(find_data.cFileName, "..") != 0) {
            char full_path[2048] = {0};
            snprintf(full_path, sizeof(full_path), "%s%s", dir_prefix, find_data.cFileName);

            value_array_push(result_array, ptr_to_value(string_duplicate(full_path)));
        }
        keep_searching = FindNextFileA(h_find, &find_data);
    }

    FindClose(h_find);
    return result_array;
}
