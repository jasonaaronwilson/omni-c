# Omni C Compiler Source Code

This is the Omni C transpiler source code.

## Compiling Omni C

If you just want to use Omni C, currently just do this:

1. sudo apt install <prerequisites> (including gcc, clang and
   lua5.4). Obviously this is specific to your linux flavor.
1. cd ~
1. mkdir src
1. cd src
1. `git clone https://github.com/jasonaaronwilson/c-single-source-file`
1. `(cd c-single-source-file ; make && make test && make install)`
1. `git clone https://github.com/jasonaaronwilson/c-armyknife-lib.git`
1. `(cd c-armyknife-lib ; make && make test && make install)`
1. `git clone https://github.com/jasonaaronwilson/omni-c.git`
1. `(cd omni-c/src/ ; make bootstrap && make)`

There's no "install" for omni-c yet because it's not even reached an
alpha level (even though it bootstraps itself!). You can do anything
you want with the resulting binary omni-c/src/omni-c. I say kick the
tires by throwing some programs at it.

## Developing Omni C

If you want to modify Omni C, it's just a bit harder to set everything
up. I resisted for far too long having a stable and development
branch but that is exactly 

Essentially we need both a "stable" tree and development tree.

So do the above AND then also do this:

1. `git clone https://github.com/jasonaaronwilson/omni-c.git omni-c-stable`
1. `(cd omni-c-stable/src/ ; make bootstrap && make)`

Now just leave the "omni-c-stable" directory like it is. Only update
it when when believe it's a "good" release. If anyone but me is
actually using Omni C, we're going to figure stuff out.

THEN, while developing I typically do something like:

1. `make clean && make extract-prototypes && make && make test`

"make cf" will probably be our friend. This is a short-hand for make
clean && make clang-format.

Before checkin, for now you will do:

1. `make save-bootstrap-prototypes`

Followed by "make cf" and also checkin those files underneath
bootstrap that actually changed.

At some point I'm going only going to only keep the semi-readable
"self.c" file around and that will be all that is in the bootstrap
directory. Something as simple as `gcc -o omni-c self.c` will be the
bootstrap step.
