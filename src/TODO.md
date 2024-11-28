# TODO

1. Move all includes into a single files (at least in the compiler
   source directory).

1. Remove lexer-to-string-helper.c. I think type.c is also dead but
   I'm not 100% sure.

1. Properly/fully parse struct_literals

1. start building with -O3

1. Put out a header with command line used for the build.

1. Document and implement multi-level parsing

1. less noisy logging, maybe examine why error code doesn't get reset,
   maybe come up with a way to clear it automatically

1. better error messages on parse failures

1. use a proper stack in balanced construct parser.

1. remove all of the C single header file stuff

1. remove all to_string functions?

1. start optionally putting out line directives and see what happens
   during debugging (we could start by making our existing comments
   better?)

1. option to keep cast and block_expr in place so we can parse our own
   output

1. add instructions for the extra test-suite

1. try out hans-boehm collector

1. macro and other overrides

1. introduce a tempory "method call" syntax (maybe :> or some nice
   unicode?)

1. make sure parser can handle more of unicode?

1. parse system includes after running through the cpp?

1. add build command that will automatically invoke a C compiler.
