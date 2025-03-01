# TODO

## Cleanup & Build

1. put git hashes into archives

1. detect the same "source path" for a file and error out if they are
the same.

## nullptr instead of NULL

Need to fix inside of compund_literals (which probably means we need
to parse them correctly?)

## Parse System Includes

The goal here is to be able to parse all of the includes for omni-c
itself (after using cpp to expand them) and extend the list to all the
header file I can find on my system for common C libraries like
SQLLite, SDL, Raylib, etc.

The current hold-up is our inability to fully parse complicated C
types (including function pointers).

## Simple Compiler Improvements

1. put out more line for other types of statements.

1. use a proper stack in balanced construct parser.

1. automatic test main file generation

1. add token_to_cson - when to use it?

1. allow experimental overloading support when using clang

## Compiler Improvements

1. better error messages on parse failures - clear errors better and
   also store furthest progress. maybe make save points more explicit
   with a name stack?

1. Properly/fully parse struct_literals (and other initializers)

1. Document and implement multi-level parsing and C macro expansion
   via "cpp".

1. Write initial linearize() function. This will make it much easier
   to figure out if we parsed everything correctly

## Other

1. refactor c-armyknife-lib so that the few overrides we need for
   tests can be specified which is a pre-requisite for moving
   c-armyknife-lib into the omni-c directory.

1. macro and other overrides

1. option to keep cast and block_expr in place so we can parse our own
   output

1. add instructions for the extra test-suite

1. introduce a tempory "method call" syntax (maybe :> or some nice
   unicode?)

1. make sure parser can handle more of unicode?

# Unnecessary Library Changes

I know I am adding things to "lib" that are strictly unnecessary at
the moment but it has the added benefit of giving me a better
<i>feel</i>.

1. A (buffered) file source and (buffered) file target. If the buffer
   max-size is zero, then get shitty performance but more familiar
   behavior. Hmmm.
