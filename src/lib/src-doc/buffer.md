# @file buffer.c

A growable array of bytes.

Buffers are useful for constructing UTF-8 strings (for example by
using buffer_printf) though they can hold any binary data including
interior NUL bytes. Buffers automatically grow as data is appended
(or inserted into) them reducing large classes of errors.
 
## @struct buffer_t

Buffers are non-thread-safe memory regions that generally "grow"
over time. Whenever a buffer grows beyound it's capacity, it is
moved to accomodates its new capacity (even if there may be other
pointers to it's insides, so don't do that unless you have decided
the buffer will never grow again).

While buffers may seem scary, used properly from a single thread,
they are actually quite predictable.
 
## @function buffer_adjust_region

This is primarily used as a helper for buffer_replace_all though it
can also be used to efficiently delete a region or simply "open up"
space within a buffer.
 
## @function buffer_append_byte

Append a single byte to the byte array.
 
## @function buffer_append_bytes

Append multiple bytes to the byte array.
 
## @function buffer_append_code_point()

Append a single code-point according to UTF-8 encoding (so 1 to 4
bytes). While you can put anything you want into a buffer_t (not
just valid UTF-8 sequences), if you then try to make a C string
from the buffer then it might end up with a NUL ('\0') byte in the
middle of it if you add code_point == 0 somewhere besides the end
of the string.

@see utf8_decode(const uint8_t* utf8_bytes).
 
## @function buffer_append_repeated_byte

Append 'count' copies of byte to the passed in buffer. This can be
used for things like indentation or horizontal rules (composed from
say '-', '=', or '*').
 
## @function buffer_append_string

Append all of the bytes from a C string (except the ending NUL
char).
 
## @function buffer_append_sub_buffer

Append all of the bytes between src_buffer[start_position,
end_position) to the dst_buffer. It is illegal to append parts of a
buffer to itself because of how buffer's are implemented which we
are considering changing for this and other reasons.
 
## @function buffer_beginning_of_line

Look backwards starting at the start position until we reach a
position where the previous character is a newline or the beginning
of the buffer.
 
## @function buffer_c_substring

Extract a newly allocated string that contains the bytes from start
to end (appending a zero byte to make sure it's a legal C string).
 
## @function buffer_end_of_line

Look forwards starting at the start position until we reach the
position of the first newline. If we reach the end of the buffer
without encountering a newline, simply return the length of the
buffer.
 
## @function buffer_from_string

Return a buffer initialized with the contents of a particular
string. The trailing NUL byte C string terminator byte is not
included in the buffer.
 
## @function buffer_get

Get a single byte from a byte array.
 
## @function buffer_increase_capacity

Allow the capacity of a buffer to be increased (or return if the
buffer already exceeds the given capacity). Generally this is not
necessary to properly use a buffer (and occasionally could actually
hurt performance if done incorrectly).
 
## @function buffer_length

Clear a buffer without resizing it.
 
## @function buffer_length

Return the number of bytes that have been added to this byte array.
 
## @function buffer_match_string_at

Determine if the buffer contains "str" at start_position.
 
## @function buffer_printf

Format like printf but append the result to the passed in buffer
(returning a new buffer in case the buffer exceeded it's capacity).
 
## @function buffer_region_contains

Determine if a buffer contains the specified text within a region.
 
## @function buffer_replace_all

Find all occurences of original_text and replace them with
replacement_text.
 
## @function buffer_replace_matching_byte

Replace all occurences of the original byte value with the
replacement value. This is useful for example to turn NUL bytes
into something more useful like a space character so the buffer can
be turned into a C string without getting truncated.
 
## @function buffer_to_c_string

Extract a newly allocated string that contain all of the bytes in the byte
buffer as a NUL (zero byte) terminated C string.
 
## @function buffer_to_lowercase

Call tolower on each character in the buffer.

TODO(jawilson): make sure this process full unicode even if it can
only uppercase ASCII latin characters.
 
## @function buffer_to_uppercase

Call toupper on each character in the buffer.

TODO(jawilson): make sure this process full unicode even if it can
only uppercase ASCII latin characters.
 
## @function buffer_utf8_decode

Similar to utf8_decode but operating on a buffer_t. (This routine
adds bounds checking beyound utf8_decode).
 
## @function make_buffer

Make an empty byte array with the given initial capacity.
 
