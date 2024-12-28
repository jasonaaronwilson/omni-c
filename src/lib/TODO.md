# TODO

## Version 0.0

1. Add version/build number to the generated header file
2. Move source code into a src directory?
3. Figure out what to do about c-single-source-file
4. implemented auto generated command line help plus enums and aliases
   and such

## Version 0.1

1. add compile time option to make values contain "a type" and do
   dynamic type checking.
2. valgrind all unit tests that return gracefully ; free of constants
   or global variables should be a NOP.
3. hashtables should grow dynamically
4. time benchmarks
5. space benchmarks plus being able to turn off code
5. extensible errors?

## Future Versions

1. make it easy to use Boehm garbage collector. make it possible to
   use other allocators such as an arena allocator (possibly even
   write a very simple arena allocator ourselves).
2. all "maps" should take values as keys plus a key type descriptor to
   provide compare, equal, and append to buffer.
3. all "maps" etc. should be "printable" to byte buffers (which can
   then be turned into strigs)
4. "persistent" and accessible by index trees
