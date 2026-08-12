// Moves these for consistency (omni-c deduplicates so not harmful).

#include <windows.h>
#include <io.h>
#include <direct.h>
#include <process.h>

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
