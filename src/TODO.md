# TODO

## Cleanup & Build

## Simple Compiler Improvements

1. add build command that will automatically invoke a C compiler.

1. start optionally putting out line directives and see what happens
   during debugging. This should be better than the current comments
   we put out.

1. use a proper stack in balanced construct parser.

## Compiler Improvements

1. better error messages on parse failures - clear errors better and
   also store furthest progress.

1. Properly/fully parse struct_literals (and other initializers)

1. Document and implement multi-level parsing and C macro expansion
   via "cpp".

## Other

1. improve typedef by allowing "=" and making it simple to typedef.

1. macro and other overrides

1. option to keep cast and block_expr in place so we can parse our own
   output

1. add instructions for the extra test-suite

1. try out hans-boehm collector

1. introduce a tempory "method call" syntax (maybe :> or some nice
   unicode?)

1. make sure parser can handle more of unicode?

1. parse system includes?
