# Omni-C

A C derived programming language so similar to C you may not even
realize it's different. If you already know C, you can start using
omni-c right away.

The main feature is that you'll enjoy a fully independent declaration
order (so you never have to write a header file again), an enhanced
standard library with basic collections to promote additional run-time
safety, overloaded functions, generic programming via templates,
function calling with method a method syntax, and even some basic
reflection capabilities. (You'll never worry about turning an enum to
a string or vice-versa).

Since Omni C is a transpiler you will get the same code quality as
your favorite C compiler such as gcc, clang, tcc, msvc, and custom
compilers for embedded development work (we will "poly-fill" to
ancient C compilers because we can).

## Using Omni C

To produce a native code binary, simply invoke the Omni C compiler
like so:

```
  omni-c build --executable=my_program *.c sub-directory/*.c
```

If you are building larger programs, you may want to create libraries
and begin using a build system of your choice, but omni-c isn't
forcing this on you.

We will probably have a (versioned) package mechanism like all other
modern languages so you can stand on the shoulders of giants instead
of writing all of your own code. We will eventually supply an LLM
prompt so that LLMs just work for you too.

## Omni C Standard Library

The Omni C standard library provides a minimal set of "collections":

1. `buffer_t`
1. `array_t<T>`
1. `hashmap_t<K,V>`
1. `hashset_t<T>`
1. `treemap_t<K,V>`

Although not exactly a collection in the traditional sense,
`buffer_t`, is similar to an array of bytes that automatically grows,
and can be used consistently for a variety of purposes such as
building strings (like StringBuilder in Java). You can even printf
into a buffer_t though we think you'll prefer string interpolation
more (they still use buffer_t underneath).

Unlike the C standard library, everything added to Omni C uses very
readable names.

I'm using the precursor library called c-armyknife-lib to write omni-c
and low-level memory curruption is now very rare. While C++ can
probably argue the same, you can use Omni C when a C++ compiler isn't
available but unlike C++, there is quite a bit less magic going on (no
move semantics, etc.)

## Features

1. forward declarations and a particular declaration order is not
   necessary (though allowed and verified...)
1. a much more sane type declaration syntax (especially convenient for
   things like functions: `fn_t<void,uint64_t*>*` is much easier to
   read than the C equivalent so much less need for typedefs!)
   built-in cast() operator to make casting more readable too.
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
   decide to not allow unsigned types.
1. raw arrays without length are depcrecated as is other forms of
   pointer arithmetic. This will lead to a much safer language (though
   still not as safe as languages like Go which hide this
   functionality even though they still have it).
1. interpolated strings are simply nicer than printf functions being
   easier to read in many cases.
1. scheme like "let" *expressions* allows statements to be executed in
   expression contexts (which was already somewhat possible in C via
   inline functions though this allows these statements to side-effect
   variables and even "loop" or otherwise change control flow). gcc
   may already allow this but it's not standard C yet.
1. fluid_let - this makes thread local variables "cool" again and
   inverts dependency injection back to just having
   dependencies. (global variables that are fluid must have the
   `fluid` anntoation applied to them).
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

In summary, this project is in flight but unforunately only has a
single developer.
