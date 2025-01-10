# TODO

## Cleanup & Build

1. stop writing anything to "/tmp"

## Simple Compiler Improvements

1. start optionally putting out line directives and see what happens
   during debugging. This should be better than the current comments
   we put out.

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

1. parse system includes?
