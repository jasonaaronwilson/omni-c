# TODO

## Cleanup & Build

1. sometimes I use make_X and sometimes I used malloc_X. I think we
   should use make_X and if we need an empty array_t, just allocate it
   in the make_X routine (aka, constructor).

1. fix "for" loop debug information which definitely causes errors to
   be reported on the wrong line since we introduce a newline in many
   cases where we don't follow it up with a line directive and then
   things kind of get "off".

1. "print-tokens" is maybe not the worst utility available. maybe
   start a tokenizer tool?

1. put git hashes into archives (optionally and conditionally add
time-stamps?)

1. detect the same "source path" for a file and error out if they are
the same.

1. do tests still write an .out file to the tests/ directory?

1. convert lib/*.sh tests to some kind of "large" test?

## our source code only uses nullptr instead of NULL

We still need to handle them in compund_literals... We seem to be able
to handle C style "casted" compound literals now but we should now
parse compound_literal appropriately so maybe just remove
compound_literal macro/hack?

## Simple Compiler Improvements

1. use a proper stack in balanced construct parser.

1. automatic test main file generation

1. add token_to_cson - when to use it?

1. allow experimental overloading support when using clang

## Compiler Improvements

1. better error messages on parse failures - clear errors better and
   also store furthest progress. maybe make save points more explicit
   with a name stack?

1. Properly/fully parse struct_literals (and other initializers)
   (mostly need more tests?)

1. Document and implement multi-level parsing and C macro expansion
   via "cpp".

1. Write initial linearize() function. This will make it much easier
   to figure out if we parsed everything correctly

1. some kind of generic types

## Other

1. refactor c-armyknife-lib so that the few "definitions" we need for
   tests can be specified somehow...

1. option to keep cast and block_expr in place so we can parse our own
   output?

1. include the open source C test suite and maybe re-enable more
   tests?

1. introduce a tempory "method call" syntax (maybe :> or some nice
   unicode?)

1. make sure parser can handle more of unicode?

## Parse System Includes

*Not Yet*

The goal here is to be able to parse all of the includes for omni-c
itself (after using cpp to expand them) and extend the list to all the
header file I can find on my system for common C libraries like
SQLLite, SDL, Raylib, etc.

The current known hold-up is our inability to fully parse complicated
C types (including function pointers) though many other things are
likely to arise.

