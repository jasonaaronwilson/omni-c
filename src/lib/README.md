# C Armyknife Library

A small fully open sourced C library that extends mainly POSIX with
safer collections and a host of conveniences to enable you to write
better medium sized C projects.

# Status is DEPRECATED

Due to lack of interest in this library (zero stars on GitHub though
one "person" is watching, I guess that's something...), I am going to
stop maintaining this library in 2025 but leave it here for others to
learn from. I will first do some cleanup such as moving arena.c to
"the attic" since I never debugged that. I'll attempt to do a better
job of documenting what I think is "good" vs. "needs work" for what's
left.

Despite being deprecated, I'm very open to pull requests to fix flaws
like critical errors, additional tests, improved documentation,
support for non Linux operating system, etc. At the very least I will
try to convey any known errata that are found and your contributions
are likely to help omni-c which I mention below.

My AATree implementation fixes a bug that is present in the
psuedo-code on Wikipedia which was actually a bit tricky to find (but
luckily I use psuedo random "large" tests (wih "knobs" to scale up)
for tricky stuff like trees or I wouldn't have noticed for quite a
long time as the bug only appears in pretty big trees.

The LRU memory-padding based bounds-checker surprisingly found bugs
that valgrind didn't. The truth is that once I debugged this library
and used it "religiously" within omni-c (you won't find many raw C
arrays in omni-c), I haven't needed it so much.

This library has been used to develop several tools though the primary
goal was always writing
["omni-c"](https://github.com/jasonaaronwilson/omni-c) my personal
take on a non OOP successor to C with an implementation strategy
similar to the original C++ "cfront" compiler (aka transpilation -
eventually to very old C variants for you retro-computing
enthusists). Eventually those tools or libraries will be written in
omni-c (which means you can still use them from C or C++ -- the same
calling conventions and such). I'm not sure omni-c will ever be
popular though pushing the C language farther might supply ideas to
the C standards committee.

## Features

* an extremely easy to use command line parser

* LOG as easy as printf.

```
    log_info("Splitting %s off from %s", token_to_string(node->name),
              token_to_string(struct_node->name));
```

* a fail fast fatal error mechanism.

* our allocator always zeros on allocation - less undefined behavior
  (similar to Go BTW)

* enhanced or at least more readable C utf-8 string operations:
  string_append, string_hash, encoding/decoding UTF-8 strings to code
  points, string_left_pad, ...

* a byte buffer abstraction that makes it really easy to accumulate
  bytes in memory, treat or extract sub-parts as UTF-8 strings, etc.

* growable arrays of "primitive values" (so anything that fits in
  64bits including pointers...)

* three similar interfaces for "maps" that provide time/space
  tradeoffs
  * string_alist for very small maps
  * string_ht for medium to very large maps
  * string_tree for sorted maps

* cross-platform *deterministic* random. (Useful for unit tests for
  one thing.)

* a decent test suite - there are too many blobs of code on github
  without test suites

* a [companion documentation
  project](https://github.com/jasonaaronwilson/c-javadoc-extractor)
  so you can write your Javadoc style documentation in markdown and
  extract them to simple markdown files.

  NOTE: this functionality will eventually be folded into omni-c and
  do a much better job.

* a non-Posix sub_process facility which I hope already works on MacOS
  and can be ported to Windows.

* a highly readable JSON "alternative" I call CDL (C Data Language)
  though you might prefer to call is JASON (Just Another Simple Object
  Notation). Once we transition this library to omni-c, either CDL or
  JSON output format will be available for either input or output just
  by setting a flag (I'm pretty certain I can auto-detect on reads...)
  and because 

## Links

* [Source Code Documentation](src-doc/README.md)

### Memory Deallocation

While some thought was put into "malloc" to make programs have less
undefined behavior, tests and sample programs are not valgrind clean
yet.

For substantial tools, you probably want to use
https://en.wikipedia.org/wiki/Boehm_garbage_collector though simply
not freeing anything is another option for simple tools that don't
generate much intermediate garbage.

## Compatibility

Unit tests run with gcc, clang, and tcc on x86-64 Linux.

```
gcc --version
gcc (Debian 10.2.1-6) 10.2.1 20210110

clang --version
Debian clang version 11.0.1-2
Target: x86_64-pc-linux-gnu
Thread model: posix
InstalledDir: /usr/bin

tcc --version
tcc version 0.9.27 (x86_64 Linux)
```

## Status

As noted above, pending cleanup, this library will receive minimal
updates though if you do encounter a bug, I may be very motivated to
help fix it since this library will form the basis of the omni-c
compiler which has been in the works as a hobby project for longer
than a year.
