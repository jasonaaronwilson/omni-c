# The Standard Library

The standard library should contain enough functinality to produce
command lines tools, servers, etc.

For now the standard library will not contain graphics related
functionality since that is provided in so many different ways even on
the same platform (though some image reading and writing support will
be provided such as a png encoder/decoder).

## `std::io`

The base of the `std:io` namespace contains `stdin`, `stdout`, and
`stderr` as well as various forms of read() and write().

There is also eof?, open_file, close_file, open_socket, close_socket,
open_pipe, close_pipe.

When read() or write() are simply passed a filename, then a file is
automatically opened and a new buffer contain the entire contents of
the file is returned.

There are also numerous routines for dealing with files on disk, such
as delete_file, move_file, make_symbolic_link, directory_iterator,
etc.

## `std::string`

Various routines for working with strings, classifying unicode code
points, etc. Also routines for writing strings to buffers (with the
ability to use alternate character encodings).

## `std::collections`

Here is where the various kind of array types are defined.

Additionally varous kinds of sets and maps.

## `std:random`

Provides several types of random number generators.

## `std::sync`

## `std:compress`

## `std:crypto`

## `std::image`

## `std::time`

## `std::net`

## `std::path`

## `std::channels`

## `std::threads`

## `std::unsafe`
