#line 2 "io.c"
/**
 * @file io.c
 *
 * This contains routines to read the contents of a file or write a
 * new file.
 */

// ======================================================================
// This is block is extraced to allocate.h
// ======================================================================

#ifndef _IO_H_
#define _IO_H_

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>

__attribute__((warn_unused_result)) extern buffer_t*
    buffer_append_file_contents(buffer_t* bytes, char* file_name);

__attribute__((warn_unused_result)) extern buffer_t*
    buffer_append_all(buffer_t* buffer, FILE* input);

extern void buffer_write_file(buffer_t* bytes, char* file_name);

__attribute__((warn_unused_result)) extern buffer_t*
    buffer_read_until(buffer_t* buffer, FILE* input, char end_of_line);

extern buffer_t* buffer_read_ready_bytes(buffer_t* buffer, FILE* input,
                                         uint64_t max_bytes);

extern buffer_t* buffer_read_ready_bytes_file_number(buffer_t* buffer,
                                                     int file_number,
                                                     uint64_t max_bytes);

int file_peek_byte(FILE* input);

boolean_t file_eof(FILE* input);

void file_copy_stream(FILE* input, FILE* output, boolean_t until_eof,
                      uint64_t size);

void file_skip_bytes(FILE* input, uint64_t n_bytes);

#endif /* _IO_H_ */

// ======================================================================

#include <stdio.h>

// This is optional...
#include <sys/stat.h>

/**
 * @function buffer_append_file_contents
 *
 * Completely reads a file and appends the contents to the passed in
 * buffer. This is often much more convenient than streaming a file.
 */
__attribute__((warn_unused_result)) buffer_t*
    buffer_append_file_contents(buffer_t* bytes, char* file_name) {

  uint64_t capacity = bytes->capacity;

  // This is optional
  {
    struct stat st;
    if (stat(file_name, &st) < 0) {
      log_fatal("file does not exist: %s", file_name);
      fatal_error(ERROR_ILLEGAL_STATE);
    }
    capacity = st.st_size;
  }

  bytes = buffer_increase_capacity(bytes, capacity);

  FILE* file = fopen(file_name, "r");
  bytes = buffer_append_all(bytes, file);
  fclose(file);

  return bytes;
}

/**
 * @function buffer_append_all
 *
 * Completely reads everything from the input FILE* putting everything
 * into a buffer.
 */
__attribute__((warn_unused_result)) extern buffer_t*
    buffer_append_all(buffer_t* bytes, FILE* input) {
  uint8_t buffer[1024];
  while (1) {
    uint64_t n_read = fread(buffer, 1, sizeof(buffer), input);
    if (n_read == 0) {
      break;
    }
    bytes = buffer_append_bytes(bytes, buffer, n_read);
  }
  return bytes;
}

/**
 * @function buffer_write_file
 *
 * Writes the contents of the buffer to the given file.
 */
void buffer_write_file(buffer_t* bytes, char* file_name) {
  FILE* file = fopen(file_name, "w");
  if (file == NULL) {
    log_fatal("Failed to open file for writing: %s", file_name);
    log_fatal("strerror(errno) = %s", strerror(errno));
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  size_t bytes_written = fwrite(bytes->elements, 1, bytes->length, file);
  if (bytes_written != bytes->length) {
    log_fatal("Failed to write %d bytes to %s", bytes->length, file_name);
    log_fatal("strerror(errno) = %s", strerror(errno));
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  if (fclose(file) != 0) {
    log_fatal("Failed to close file: %s", file_name);
    log_fatal("strerror(errno) = %s", strerror(errno));
    fatal_error(ERROR_ILLEGAL_STATE);
  }
}

/**
 * @function buffer_read_until
 *
 * Read from a FILE* until either the end of file is reached or a
 * particular "end-of-line" character is read. Every character except
 * the end_of_line character is appended to the buffer.
 *
 * This can be used to read normal Unix "lines" or another use is to
 * read "lines" that end with NUL (U+0000) or until say a seperator
 * like "," is encountered.
 *
 * This function (and string_tokenize) should take a character set
 * which can unify their interface and allow U+0000 to be a member of
 * the set (unlike using a C string (aka char*) as the character set).
 *
 * TODO(jawilson): write a proper test!
 */
buffer_t* buffer_read_until(buffer_t* buffer, FILE* input, char end_of_line) {
  while (!feof(input)) {
    int ch = fgetc(input);
    if (ch < 0) {
      return buffer;
    }
    if (ch == end_of_line) {
      return buffer;
    }
    buffer = buffer_append_byte(buffer, ch);
  }
  return buffer;
}

/**
 * @function buffer_read_ready_bytes
 *
 * Read from a FILE* until either the end of file is reached,
 * max_bytes has been read, or there are no ready bytes. This function
 * should never block.
 */
extern buffer_t* buffer_read_ready_bytes(buffer_t* buffer, FILE* input,
                                         uint64_t max_bytes) {
  int file_number = fileno(input);
  return buffer_read_ready_bytes_file_number(buffer, file_number, max_bytes);
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

/**
 * @function file_peek_char
 *
 * Returns the next byte from the input (as an int not u8_t) or -1 if
 * the end of the file input has been reached. A a byte is read, then
 * the byte is "pushed back" into the input stream so that if
 * file_peek_char, fgetc, or a host of other functions attempt to read
 * the input then
 */
int file_peek_byte(FILE* input) {
  if (feof(input)) {
    return -1;
  }
  int result = fgetc(input);
  // ungetc doesn't "push back" -1 according to
  // https://en.cppreference.com/w/c/io/ungetc
  // But who is going to trust that...
  if (result >= 0) {
    ungetc(result, input);
  }
  return result;
}

/**
 * @function file_eof
 *
 * Return true if an input stream is at the end of the file. I don't
 * know what "feof" really does but it doesn't actually do this.
 */
boolean_t file_eof(FILE* input) {
  return feof(input) || file_peek_byte(input) < 0;
}

#define FILE_COPY_STREAM_BUFFER_SIZE 1024

/**
 * @funtion file_copy_stream
 *
 * Copy some or all of an input stream to an output stream.
 */
void file_copy_stream(FILE* input, FILE* output, boolean_t until_eof,
                      uint64_t size) {
  if (until_eof) {
    size = ULLONG_MAX;
  }

  uint8_t buffer[FILE_COPY_STREAM_BUFFER_SIZE];
  while (size > 0) {
    int minimum = size < FILE_COPY_STREAM_BUFFER_SIZE
                      ? size
                      : FILE_COPY_STREAM_BUFFER_SIZE;
    uint64_t n_read = fread(buffer, 1, minimum, input);
    if (n_read == 0) {
      break;
    }
    fwrite(buffer, 1, n_read, output);
    size -= n_read;
  }
}

/**
 * @function file_skip_bytes
 *
 * Skip n_bytes from the given input stream unless the end of the file
 * is reached first in which case we silently return without
 * indicating that condition. Gemini claims that fseek may not work on
 * "stdin" and further claims there may be a seekable proprety
 * associated with an input stream (which seems to match up when
 * running on Debian Bookworm...).
 */
void file_skip_bytes(FILE* input, uint64_t n_bytes) {

  // We'd try to do it like this but Gemini claims that this doesn't
  // reliably work for stdin. That is bonkers!
  //
  // fseek(in, size, SEEK_CUR);

  while (1) {
    if (n_bytes == 0 || feof(input)) {
      return;
    }
    int ch = fgetc(input);
    if (ch < 0) {
      // TODO(jawilson): fixme?
      return;
    }
    n_bytes--;
  }
}
