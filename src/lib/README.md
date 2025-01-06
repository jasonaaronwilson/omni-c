# Omni C Extended Standard Library

This library will be the basis of the Omni C "standard library". It
was imported from
["c-armyknife-lib"](https://github.com/jasonaaronwilson/c-armyknife-lib).

## Future Evolution

The "collections" portion of this library is expected to shift
dramatically with the introduction of generic structures and generic
functions. I've been thinking about this for at least a year and
learning (via great CppCon talks on YouTube) more about C++ especially
templates, so while it will actually be similar to C++ templates in
certain ways, there will hopefully be fewer sharp edges -- we don't
have an OOP based type system so covariance and contravariance should
not be a problem anymore.

## Features

* a byte buffer abstraction that makes it really easy to accumulate
  bytes in memory, treat or extract sub-parts as UTF-8 strings,
  etc. It's low-level but still convenient and *memory safe*. In some
  cases it is more performant than certain C patterns. For example,
  buffer_printf, followed by a single call to write should out-perform
  using many "small" printfs.

* enhanced or at least more readable C utf-8 string operations:
  string_append, string_hash, encoding/decoding UTF-8 strings to code
  points, string_left_pad, ... We recommend treating all char*
  instances as "const" and using these immutable style routines or
  better yet using byte buffers when that makes more sense.

* *collections* for "modern programming". Growable arrays, small
  unsorted maps, hashtables, and potentially large sorted maps (aka
  balanced trees). It's hard to make software that scales without
  either recreating these data-structures or using something more
  advanced. These are the minimum data structures you need and Java
  provided them in about 1998 so it's safe to say they should be
  standard by now.

* logging is as easy as printf.

```
    log_info("Splitting %s off from %s", token_to_string(node->name),
              token_to_string(struct_node->name));
```

* an extremely easy to use command line parser - build up you flag
  definitions and then let the parser do it's business. You'll get
  "help" functionality almost for free.

* a simple alternative to JSON for highly readable data-structure
  printing

* our allocators always zeros on allocation - less undefined behavior,
  data leaks, etc. to worry about. This is the Go model.

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

* a fail fast fatal error mechanism suitable for many non server
  software (consider using SQlite for rubustness and face of a crash
  if you need to be more robust).

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

TODO(jawilson): this isn't true anymore. I still consider these three
compilers to be our primary "targets", but I've gotten a little lazy
for velocity temporarily but I've found that clang and tcc tend to
closely follow gcc if you follow C standards and don't go crazy.

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

2025-01-01: most "library" tests now get run as part of running the
"make test" target for omni-c itself and using the run-tests.lua
script. It would still be a good idea to not make major changes to
both the compiler and the library at the same time. If the compiler
needs a new library function, put it into the library first vetted and
tested, and then use it in the compiler.

2024-12-28: this library was (badly) imported into the omni-c git
repository. Since omni-c currently doesn't offer a true first class
c-preprocessor, I don't expect all unit tests to run right away.

## History

I began writing "c-armyknife-lib" well before attempting to write the
omni-c compiler itself knowing I would need certain things like
growable arrays and hashmaps to write a semi-performant "compiler". I
went down some paths that didn't quite work out. It's possible that
simply using a subset of C++ from the begining would have gotten to an
interesting place sooner but I think the experience of writing the
"collections" portion in C first shapes things quite a bit. Sometimes
you have to throw stuff away and start again. Don't confuse this with
the "second-system effect" which is where the first version works
well, but one adds too much stuff into the second version. In my case,
I had inflated confidence and expectations writing the first version
and dialed it back to get a "minimum viable product".

For example, one original idea was to make mutating operations return
the (possibly) updated pointer to the collection after being moved to
save some pointer indirections. gcc and clang both offer
`__attribute__((warn_unused_result))` which would catch some of the
issues around this approach, certainly not all, and just didn't feel
like C.

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
