# @file io.c

This contains routines to read the contents of a file or write a
new file.
 
## @function buffer_append_all

Completely reads everything from the input FILE* putting everything
into a buffer.
 
## @function buffer_append_file_contents

Completely reads a file and appends the contents to the passed in
buffer. This is often much more convenient than streaming a file.
 
## @function buffer_read_ready_bytes

Read from a FILE* until either the end of file is reached,
max_bytes has been read, or there are no ready bytes. This function
should never block.
 
## @function buffer_read_until

Read from a FILE* until either the end of file is reached or a
particular "end-of-line" character is read. Every character except
the end_of_line character is appended to the buffer.

This can be used to read normal Unix "lines" or another use is to
read "lines" that end with NUL (U+0000) or until say a seperator
like "," is encountered.

This function (and string_tokenize) should take a character set
which can unify their interface and allow U+0000 to be a member of
the set (unlike using a C string (aka char*) as the character set).

TODO(jawilson): write a proper test!
 
## @function buffer_write_file

Writes the contents of the buffer to the given file.
 
## @function file_eof

Return true if an input stream is at the end of the file. I don't
know what "feof" really does but it doesn't actually do this.
 
## @function file_peek_char

Returns the next byte from the input (as an int not u8_t) or -1 if
the end of the file input has been reached. A a byte is read, then
the byte is "pushed back" into the input stream so that if
file_peek_char, fgetc, or a host of other functions attempt to read
the input then
 
## @function file_skip_bytes

Skip n_bytes from the given input stream unless the end of the file
is reached first in which case we silently return without
indicating that condition. Gemini claims that fseek may not work on
"stdin" and further claims there may be a seekable proprety
associated with an input stream (which seems to match up when
running on Debian Bookworm...).
 
## @funtion file_copy_stream

Copy some or all of an input stream to an output stream.
 
