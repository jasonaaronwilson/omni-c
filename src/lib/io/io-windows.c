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
