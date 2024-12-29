# Omni C Extended Standard Library

This library is the basis of the Omni C "extended" standard
library. It was imported from
["c-armyknife-lib"](https://github.com/jasonaaronwilson/c-armyknife-lib).

We expect to make significant changes from c-armyknife-lib going
forward now that omni-c is self hosting and we can begin to use
features of omni-c, namely declaration order flexibility and
eventually templates.

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

2024-12-28: this library was (badly) imported into the omni-c git
repository. Since omni-c currently doesn't offer a true first class
c-preprocessor, I don't expect all unit tests to run right away.

## Notes

My AATree implementation fixes a bug that is present in the
psuedo-code on Wikipedia which was actually a bit tricky to find (but
luckily I use psuedo random "large" tests (wih "knobs" to scale up)
for tricky stuff like trees or I wouldn't have noticed for quite a
long time as the bug only appears in pretty big trees.

The LRU memory-padding based bounds-checker surprisingly found bugs
that valgrind didn't. The truth is that once I debugged this library
and used it "religiously" within omni-c (you won't find many raw C
arrays in omni-c), I haven't needed it so much.
