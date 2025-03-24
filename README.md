# Omni-C

## The Pitch

1. Fast - omni-c is fundamentally C and compiles to C and can call C
   libraries (or even custom assembly). All of the hard work of the
   clang, gcc, msvc tool-chains are thus available when you choose one
   of those as your backend. You could also use Fil-C or TrapC for
   even more memory safety or even "zig cc" if you want to integrate
   with zig.

2. Familiar - omni-c should be very familar to anyone that knows C. The
   code you write can look just like it would in C unless you want to
   use more advanced features. For generics, it will look a lot like
   C++ (and Java) but the semantics are somewhat simpler. This is a
   less radical departure than Rust or Go which both have good ideas
   that got incorporated into omni-c.

3. Fun - unlike plain C, certain things that should be trivial, like
   moving a function from one file to another, is actually trivial. I
   never found it fun to worry about header files or order of
   declarations or how to write the type of a function pointer with
   it's strange syntax, so I fixed it. For folks finding C++ or Java
   too complex because of OOP but who still like the "subject object
   ..." syntax of methods calls, that will soon become available as
   simple syntactic sugar.

## The Description

Omni C is an extended C programming language with convenience and
safety enhancement features.

The main features beyound C is an independent declaration order (so
you never have to write a header file again), an enhanced standard
library with basic collections to promote additional run-time safety,
overloaded functions, generic programming via templates, function
calling with "method" syntax, and basic reflection capabilities. (For
example, enum to string and string to enum conversion.)

Since Omni C is a transpiler, code quality should be similar to the
underlying C compiler used (gcc, clang, tcc, msvc, etc., and we may
"poly-fill" (aka back-ported to older environments automatically) to
even older C standards than C99).

## Using Omni C

To produce a native code binary, simply invoke the Omni C compiler
like so:

```
  omni-c build --executable=my_program *.c sub-directory/*.c
```

(There are other options like archives to help structure larger
programs, or you can use or output more standard C libraries with
header files. We will eventually have a (versioned) package mechanism
like all other modern languages so you can stand on the shoulders of
giants instead of writing all of your own code.)

## Omni C Standard Library

The Omni C standard library provides a minimal set of "collections":

1. `buffer_t`
1. `array_t<T>`
1. `hashmap_t<K,V>`
1. `hashset_t<T>`
1. `treemap_t<K,V>`

Although not exactly a collection in the traditional sense,
`buffer_t`, is similar to an array of bytes that automatically grows,
and is used consistently for a variety of purposes such as building
strings (like StringBuilder in Java). You can even printf into a
buffer_t though we think you'll prefer string interpolation more (they
still use buffer_t underneath).

Unlike the C standard library, everything added to Omni C uses very
readable names.

## Features

1. forward declarations and a particular declaration order is not
   necessary (though allowed and verified...)
1. options for more readable type declarations (especially convenient
   for things like functions: `fn_t<void,uint64_t*>*` is much easier
   to read than the C equivalent so much less need for typedefs!)
   built-in cast() operator to make casting more readable too (or at
   least easier to locate since cast's are a source of unsafe C).
1. overloaded functions (these are technically already available in
   clang but you don't need to use an attribute, etc.)
1. syntactic sugar - method call/chaining syntax (so code might look
   like it is OOP even though it isn't), overloadable semantics for
   \[\] operator, and perhaps even C# style "properties"
1. automatically generated run-time reflection capabilities and other
   code generation niceties so you can focus on writing real
   code. Want to dump a structure for debugging? That code never needs
   to be written by hand on a case by case basis. (If gdb can do it,
   we can do it better).
1. consistent naming of types uint64_t, etc. This promotes the same
   behavior across all platforms and IMHO looks nice (especially with
   the right syntax highlighting). Unlike Java, we don't arbitrarily
   decide to not allow unsigned types and pointer vs. values are still
   available.
1. raw arrays without length are depcrecated as are other forms of
   pointer arithmetic (though we are tracking Fil-C as a target C
   compiler which may allows these to be safe again). This will lead
   to a much safer language (though still not as safe as languages
   like Go which hide this functionality even though they still have
   it).
1. interpolated strings are simply nicer than printf functions being
   easier to read in many cases.
1. scheme like "let" *expressions* allows statements to be executed in
   expression contexts (which was already somewhat possible in C via
   inline functions though this allows these statements to side-effect
   variables and even "loop" or otherwise change control flow). gcc
   may already allow this but it's not standard C yet.
1. fluid_let - this makes thread local variables very useful inverting
   dependency injection back to just having dependencies. (global
   variables that are fluid must have the `fluid` anntoation applied
   to them).
1. C++ style templates as a basis for generic programming. No "OOP"
   required. You can write your own generic container and since there
   is no inheritance (because there is no OOP), I can't see the reason
   that we need to worry about various forms of type variance like
   "covariance", "contravariance", "invariance", and
   "bivariance". While there are very real concerns about "code
   explosion" because code is over specialized, I think we'll be able
   to find a balance.
1. Namespaces and modules when you want them (modules can all
   automatically initialize themselves allowing much richer
   initialization than is typically done in C).
1. Easy stuff like being able to set default values for fields of a
   struct even if it's heap allocated.
1. switch statements can switch on "char*" or buffer_t as easily as
   integers.
1. iterators from the standard library allows "foreach".
1. an alternative to the C pre-processor's purely textual macros (Zig
   has some nice stuff though way to much "const-expression"
   aware mentatility).

## Status

* (2025-03-07) I haven't updated status for a while so there is a lot
  here, some of it's user visible and kind of cool though the vast
  amount of work was moving towards a mono-repo where *everything* is
  compiled with omni-c (which increases coverage and should allow more
  focus/progress/changes/ease-of-installation, etc.). I'm finally
  putting out line directives for all statements which seems to work
  well for debugging (at least when not -O3) and also helps with C
  compiler error messages that occur when omni-c doesn't catch the
  problem itself (too frequently). nullptr is always available even
  when compiling to C99. The new "typedef foo = BAR;" syntax is
  usually easier to use than the equivalent format without that magic
  "=" sign and maybe even seasoned C developers would like it. This
  along with the fn_t macro made messing around with structures as
  interfaces (see byte_stream_source and byte_stream_target, not
  really utilized, more of an experiment) not that painful to write
  (and it's more readable to me at least...) Integration with the
  Boehm GC (so more Go like than ever...) The "build" sub-command
  automatically invokes the C compiler (with several different C
  compilers supported out of the box like gcc, tcc, clang and even
  "zig cc"). Rounding out the changes, I finally incorporated
  "omni-archives" which are a convenient way to package up source code
  for "all at once" compilation model - an extremely simple
  alternative to zip or tar which is about 100 lines of somewhat
  commented code (see src/lib/oarchive.c), handles unlimited length
  utf-8 filenames, etc.
* (2024-12-10) Tested on an Orange Pi Zero 2 (1GB, Debian Bookworm
  based OS provided by Orange Pi, Quad Core ARM Cortex-A53 64-bit
  1.5GHz processor) a less than $40 computer. Building is much slower
  than my usual baseline machine (N100 based mini-pc with 16GB) though
  the vast bulk of that time is spent in steps involving gcc
  invocation. This uncovered at least one remaining bootstrapping
  problem related to c-armyknife-lib (the solution is to still do make
  install in c-armyknife-lib).
* (2024-11-24) We can now parse both omni-c + c-armyknife-lib at the
  full statement/expression level! This promises to make bootstrapping
  much easier as a single extra file can serve as the (stable)
  bootstrap compiler. Once bootstrapping is fully supported, we can
  remove some annoying cruft from C files and truly stop worrying about
  header files and begin using features like code generation right
  away. We'll still require a type-annotation phase to really be able
  to add some of the cooler features promised above.
* (2024-08-27) We now use "cson" (JSON without commas and "opened-up"
  using " = " instead of ":" - it just *looks* better) for parse-tree
  debug output. Golden files have been added (based on cson) so we are
  unlikely to go backwards and more golden file will be added for
  non-expression constructs.
  At this point, I feel like Omni C is unlikely to use the native C
  "cast" syntax ` (type) value ` but the same construct is available
  via ` cast(type, value) ` where cast is a new keyword. While it's
  much more readable, and it's a one line macro so you could start
  using it today, if your are casting things this much, you may have a
  problem omni-c can't fix.
* (2024-08-11) The top-level parser has been rewritten in a new style
  which is much easier to read and debug. The statement parser is
  showing signs of life but is incomplete. Some naive assumptions
  about how to parse C types (which in reality are called declarators
  because they tend to include a *name* in them) were made. This lead
  to learning about cdecl, the spiral rule, and humous quotes from the
  likes of the creator of C++ (who didn't bother to fix it...). We'd
  like to be able fully parse C types and I will give it a better
  attempt later but this is another thing I plan to change for the
  enhanced Omni C syntax (more like Java or Dart and as easy to read
  as Go types since they actually DID fix C's declarators). Some
  preliminary support for reflection information is being
  explored. The test runner was rewritten in lua (part of a plan to
  reduce reliance on bash so that we can eventually build on
  windows). Etc.
* We now (2024-07-29) can turn our own source code into a single C
  file that can be compiled (extensive testing on this to make sure
  it's equivalent hasn't been done yet). Unfortunately there are still
  some problems that prevent us from producing a single global header
  file (namely global variable - we might just be able to not output
  them temporarily). Many short-cuts were taken so this is unlikely to
  work for arbitrary C code. The git tag is milestone-1.
* We now (2024-06-19) have a decent lexer and a very incomplete parser
  but that is good enough to *extract prototypes* from our own source
  code. In theory we never have to write prototypes again for extern
  functions or care about their order in the file (more work is
  required to handle static and also inline functions though I think
  those won't be too much trouble - handling types fully has me more
  worried) which means we can also order functions essentially anyway
  we want now so a more top-down style can be used a tiny win but we
  can start to get a feel about how "partial bootstrapping" is doing
  to work.
* (Older status messages have been trimmed but you can find them in
  older versions of this file).
