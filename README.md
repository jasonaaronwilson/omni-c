# Omni-C

A C derived programming language very strongly resembling C in look
and semantics with modern conveniences, runtime safety, and a revamped
standard library.

## Status

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
* (We will take header file extraction further and eventually only
  shared macros will need to be inside of the header block so that at
  least all of the code I write works this way. It is important to eat
  one's own "dogfood".)
* The short-cuts we took mean we can't radically transform the bodies
  of functions where most of the interesting transformations will
  occur (templates being the most interesting, over-loaded functions
  kind of have to come along for the ride with that, but both require
  identifying callsites precisely and in some ways maintaining certain
  C pre-processor behaviors could be hard so we might need to work on
  a few other things in c-armyknife-lib first so that isn't going to
  be easy for several reasons).
* At almost anytime, we could convert to a certain subset of C++ as
  the output language which is one of several targets anyways but that
  would mean we couldn't be run on platforms where there isn't a great
  C++ compiler. It's also less academic which let's be honest is part
  of what I'm doing here. Go will still up being better than omni-c
  for writing compilers including omni-c and once there is a standard
  comitte an independent open source compiler with great compatibility
  and performance, I'm going wonder why I did all of this (oh yeah, to
  point out that exceptions are not the problem and we can have
  them). Our goal is actually between Go (garbage collected) and Rust
  (to avoid the infamous borrow checker) and provide a pathway for
  porting large amounts of C code to something better. Even if that
  means conversion to Rust or Zig, that will be a win for portability
  of C code).
* SO the easiest next thing we can do are actually around top-level
  constructs for example enum->string and
  buffer_sub_string->enum. Since neither has to be perfectly precise
  during bootstrapping so it shouldn't add much cogniative load above
  what we are already doing for extracting prototypes).
* I dislike the current hand-written parser though it has been useful
  for learning some do's and don'ts but is why I am hesitant to go
  into function bodies to turn them into parse trees because the first
  version seldom work great the first time and require debugging
  instead of trivally seeing the error when confronted with a
  counter-example. While we probably still need heuristics from PEG in
  some cases (a sub-tree that gets the fartherest is likely the error
  folks want), we have the inklings of a decent error reporting
  mechanism.
* Self-compilation performance seems likely to be dominated by gcc or
  clang for a while longer so that isn't a reason to reason to deep
  dive into a combinatorial parser, a parser generator, etc., just
  yet...

In summary, this project is in flight but unforunately only has a
single (lazy) developer.

# Older README

Even if this is my exact feature set (which it probably isn't), it
doesn't lay out interesting things like a timeline for each feature to
become available.

## Features

These are probably the less controversial features (either because
they are widely desired, are already in C++ (or another popular C
derived language), or you don't need to use the feature if you don't
want to):

1. namespaces, imports, and libraries (no header files!)
1. overloaded functions (I think exactly like C++ modulo type
   conversion rules)
1. syntactic sugar - method call/chaining syntax (so code might look
   like it is OOP even though it isn't), overloadable semantics for
   \[\] operator, and automatic C# style "properties"
1. improved syntax and safer "casting" behavior for typedef so that it
   can create opaque zero cost abstractions
1. improved enums (which can be converted to strings, etc.,
   essentially automatic code generation that can be turned off if you
   don't need it on a per enum basis)
1. consistent naming of types uint64, int64, uint8, int8, float64,
   float32, float16, etc. (Does anyone actually like size_t?)
1. switch statements don't have fallthrough behavior by default and
   statements after a switch case are automatically a block so that
   variables can be defined there, defer_block can be used, etc.
1. C23 style attributes (aka [[xyz]]) though the set of standard
   annotations is a bit different from C23.
1. multiple return values (modern C compilers already efficiently
   support returning structures so this is actually just syntactic
   sugar).
1. C# style exceptions + defer and defer_block (essentially allows
   some aspects of Resource Acquisition Is Initialization which I
   think may be badly named actually because it seems to have more to
   do with resource deallocation)
1. utf8 strings by default (convertible at no cost to C char\* style
   strings for C and OS inter-op).
1. language level threads (not necessarily Go lang level cheap though)
   (Let's hold off on making promises...)
1. guaranteed self tail calls with the aim of getting tail calls
   supported in all cases without *too* much of a performance hit.
1. "_" can be used in numeric constants (like ' in C23)
1. empty arguments lists mean that the function takes zero arguments
   (like C++ etc.) instead of taking arbitrary arguments, like having
   an argument list simply be "(void)" in standard C.

Next are some features that really change the feel of the language and
thus the standard library:

1. generically typed functions and structures (not as complex as you
   think because we *don't* have inheritance!)
1. multiple array types (array, fixed_array, unsafe_array) and bounds
   checking (dynamic if necessary) for all but unsafe_array (which
   only exists for C interop) ; the controversial part may be that []
   is no longer allowed in types since it is now ambiguous given
   multiple array types and we'd have to choose a default (which is
   obviously just array it I *had* to choose).
1. dynamic references and switch by type (like any in Go)
1. interpolated strings remove the need for (unsafe) printf type
   functions being easier to read in many cases.
1. scheme like "let" *expressions* allows statements to be executed in
   expression contexts (which was already somewhat possible in C via
   inline functions though this allows these statements to side-effect
   variables and even "loop" or otherwise change control flow). gcc
   may already allow this but it's not standard C.
1. fluid_let - this makes thread local variables "cool" again and
   inverts dependency injection back to just having
   dependencies. (global variables that are fluid must have the
   `fluid` anntoation applied to them).
1. eliminate the C pre-processor and eliminate purely textual macros
   (honestly I have no idea what this looks like yet meaning this is
   the hardest part to design - templates and constexpr look to carry
   C++ pretty far and Rust has a model).

I'd like to optionally have these features. In some sense they are
tied together though and will not be an initial focus.

1. closures (simpler and prettier than C++, more like Java or ML)
1. garbage collection (manual management will still be an option for
   embedded systems or other low-level programming [^1]

The Omni C standard library provides a minimal set of collections
including at least:

1. `vector`
1. `hashmap`
1. `hashset`
1. `treemap`
1. `immutable_treemap` (aka a "persistent" tree data-structure, a nod
   to clojure though the concept predates clojure).

Although not a collection in the traditional sense, `buffer`, which is
similar to an array of bytes, is used for a variety of purposes such
as building strings (so like StringBuilder in Java) as well as a
buffer for two way communication with another thread (so to Unix pipes
or channels in Go).

The Omni C standard library doesn't exist but c-armyknife-lib is kind
of a painful and not memory safe version of what it might look like
and if we can write a compiler using it, it probably isn't horrible...

### No Inheritance

Missing from this feature list is *inheritance* a hallmark of OOP. Not
only is that space already fully occupied by Java, C#, C++, Crystal,
etc., I personally don't like that style of abstraction and I'd like
to provide a compelling alternative. There are some domains such as UI
"widgets" where that style fits well, but it often misses. If you
can't define all of the behavior in a piece of code, that is fine,
explicitly pass in behavior via closures.

(The initial compiler is using hand-written (inline) functions like
to_field_node() and to_node() which do dynamic type-checking - kind of
like ugly typed-union's on obviously a little code-gen and you might
have something more explicit than Go's `any` but maybe not as
type super awesomeness as a "tagged union" in ML).

## Features Removed from Standard C

1. many types of silent type conversions
1. switch fall through (and probably therefore Duff's device unless
   you want to write some gotos and hope the compiler does that right
   thing)
1. [] is going to become "syntatic sugar" instead of the dangerous
   (though well defined in terms of the transformation) syntatic sugar
   it already is. Certain kinds of profiling might be able to point
   out where [] is more expensive than say lg-n so it shouldn't
   completely shock the sensibilities of old-timers (and more
   importantly the right data-structure is used).
1. some obsolete keywords like `register` will continue to have no
   function but we will go further and simply eliminate them for most
   backends and let them do their "thing"
1. octal - still supported but requires the "0o" prefix. Octal bugs
   are not that common overall but it is worth making things more
   consistent anyways. --allow-ocatal=true and be used when porting
   code.
1. VLA - since we don't have default arrays, VLA arrays kind of don't
   make sense. This doesn't mean that VLA will not be utilized in
   certain contexts by generated code.

## Rationale

The biggest change from C is probably generic structures and generic
functions and that's what I am experimenting with here the most
despite these being readily available in most C derived
languages. Without inheritance these become much simpler than one
might expect in terms of the type system and hence we are keeping with
the simplicity aspect of the C language.

The aging C standard library can feel bloated while still being
insufficient for modern programming. One of the largest C programs,
the Linux kernel, doesn't even use the C standard library.

One reason may be that large parts of the C standard library is just a
wrapper around calling into the kernel and the kernel probably
wouldn't want to call into itself in the exact same way (though there
is something interesting about the kernel being able to call itself
via a standard library that has the same interface as user code).

If the C standard library was better organized (aka modular) and more
powerful, the Linux kernel would *want* to use *parts* of that
library. (Certainly not the raw IO parts...)

Namespaces and libraries immediately confer code organization benefits
to Omni C and we should apply the same to the standard library by
organizing it as well as keeping it out of the default namespace as
much as possible. Additionally we will use more readable identifiers
(unless the abbreviation is very widespread like stdout instead of
standard_output).

Just like C++, there has to be some mode where it's possible to FFI
with minimal overhead to C libraries. (We should probably provide
mechanisms to work with C++ as seamlessly but that is not part of the
MVP).

Unlike Java, there isn't a 1:1 correspondance between a file and
namespaces (some other modern languages also follow this path). This
makes it easier to add or remove things from a namepsace to accomodate
deltas for a platform level at the "build" level rather than using the
C preprocessor for this task. The Java style makes it slightly easier
to find the exact definition, grep and other tools will be able to
help.

While Omni C will initially focus the standard library on
algorithms/collections because that is where the C library is very
weak, POSIX for all it's benefits of allowing C code to run across
different platforms is also an aging interface. For example, the
io_uring interface being introduced to the Linux kernel is radically
changing how user programs interact with the kernel and we want to
jump on that model (which is similar to CSP, aka Erlang, which is a
succesful model and seems to scale better to many core
computing). (io_uring itself is so complicated that you are supposed
to interact with it via a library which is unusual for the
kernel/process abstraction layer - I think there is a different
"process" to "kernel" architecture brewing and it will be async
messages all the way down).

Omni C can easily call (aka interop) with C (and to a degree
vice-versa) so nothing is lost by focusing on the standard C libraries
weakpoints and fleshing out the standard library later.

## Using Omni C

Note: the first version of Omni-C will probably output C++ instead of
C as that means we can use templates and inline functions without
lowering them to C code.

Omni C by default is a transpiler targeting C (and thus wasm via C to
wasm translation).

To produce a native binary, simply invoke the Omni C compler something
like so:

```
  omni-c build --executable=my_program *.oc sub-directory/*.oc
```

This assumes either clang or gcc are available in PATH and whichever
is found first will be used but obviously you can force a particular C
compiler to be used with flags. Both are high quality C compilers and
overall optimized code seem to perform similarly.

We actually want to target the generated code to something like "tcc"
if easily achievable. By using less of the C standard (especially the
newest standards) then you can use alternatate compilers with Omni C.

Rather than `--executable` flag, you can use `--library=foo.ocl` to
produce a distributable source library that could contain everything
necessary to use the "compiled" code (since compilation is pushed back
as much as possible). Libraries are not actually compiled at all, the
source files and metadata are simply concatentated.

Like many compilers, omni-c can be told to stop at a certain
points. omni-c can simply be used to generate the C source files (and
a header file for interop) which you can compile into your C program
via higher level means.

For the full list of flags, see the documentation for the to C
transpiler.

Hopefully we can also target "tcc" for lightning fast debug builds
though tcc doesn't support RISCV-64.

Eventually we may add multiple native compilation targets such as:

1. LLVM
1. Comet-VM (interpreter, RISC-V64, ARM64 - full tail recursion and
   amazing debugging)
1. wasm

## Omni C Syntax

The general rule is that the familar C syntax should be used except
where it interferes with other goals. Especially important are C
function statements and expressions - we deviate somewhat more at
namespace level constructs like typedef. We hope to produce a tool
that will automatically convert 90% of C23 and prior versions of C to
Omni C, but this is much harder to achieve in many code bases because
of prevasive use of the C pre-processor and macros (which are used so
much because standard C is lacking).

The full syntax with examples will be provided in another file but a
small example is shown below showing some features of Omni C.

### Samples

```
// Files can contains code to place in multiple name spaces. namespaces
// extend to the end of file or the next namespace declaration.

namespace std::collections::printer;

import std::collections::iterator;

byte_array* pretty_print(byte_array* output, reference object) {
   // switch is able to switch on the type contained in an object reference.
   switch (object.type) {
      case int64 | uint64 | float64 | float32:
        return pretty_print_number(ref);
      case string:
        return print_print_string(ref);
   }
   itertor(reference)* iterator = object.type.iterator(object);
   if (iterator) {
      return pretty_print(byte_array, iterator), no_error();
   }
   auto stream = object.type.code_point_stream(object);
   if (append_source) {
     return byte_array.append(stream), no_error();
   }
   throw std::exception::unimplemented("The given reference can not be pretty-printed.");
}

///
/// Appends a human readable textual representation of the elements from an
/// iterator.
///

[[public]]
byte_array* pretty_print(byte_array*, iterator(any) iterator) {
   byte_array = byte_array.append("{\n");
   boolean is_first = true;
   // The more concise syntax for (ref item : iterator) {} could also be used
   while (iterator.has_next()) {
      ref item = iterator.next();
      if (!is_first) {
        byte_array = byte_array.append(",\n");
        byte_array, error err = pretty_print(item);
        if (err) {
          return null, err
        }
      }
   }
   return byte_array, no_error()
}
```

# Purpose

Most C language successors, with the exception of Go, deviate
significantly from C's simplity usually with the introduction of
inheritance. (Go went with Javascript esque "duck typing' which
apparently creates it's own problems.)

C is a painful language to write large programs in for many reasons,
but a few tweaks changes this dramatically. Overloaded functions alone
(even without namespaces already resolve many naming collisions.

Omni C might be closest to Java or C# (without classes) but still with
a nod to low-level programming use cases and of course AOT
(ahead-of-time compilation) being the default model. A program
compiled by Omni C contains all it needs to run on the same
platform. While there are many competing ways to distribute programs,
this simple (default) approach adheres the the KISS principle.

Targeting C means that Omni C can run just about everywhere (including
the browser via wasm and potentially non gcc or clang embedded systems
via alternare C compilers). wasm also allows Omni C code to be used
from other programming languages that have wasm interop.

The features I've included in Omni C have been vetted by 25+ years of
professional software engineering including many large projects across
the entire software stack (I've written "microcode" level code,
backend server code, command line utilities (many of which are bash
scripts though), web frontends, etc.). I've had an interest in
programming languages design and implementation even longer than that.

Most importantly Omni C is the language that I personally want to
write my code in and feel like that code I write will last.

Naturally I want the code to look exactly the way I expect, I want
Omni C to have the feature set that I think will make me most
productive even if this means giving up a bit of overall
performance. I want my code to run anywhere I need it and be as easy
to debug as possible. I want to be able to print out code like a book
and be readable and writable without a fancy IDE. I want high
performance code code though we are never going to prefer benchmarks
over ease of use and correctness. I want Omni C to have as little
undefined behavior as possible.

There are some dynamic languages which are easy to read and write but
static type checking is essential to me because I tend to introduce
numerous small errors in programs as I write them and the static type
checking system finds many of these immediately without even running
tests. (And again, many of these dynamic languages still employ
inheritance.)

# Possible Features

While most of this document is a bit more thought out (even if I roll
back those decisions to be more compatible with the C/C++ lineage
(after all, many will simply prefer Go if you want a better C and
aren't keen on C++, Java, C#, etc. for various reasons), this is a
little more freeform:

1. there should eventually be some form of reflection on types (but
   not values themselves)
1. can we extend enumerations / aka define pieces in multiple places?

1. and if that's a good idea, would it make sense to treat structs the
   same way, functions the same way (we kind of do sort of, it's a bit
   like apples and oranges for example advise in Lisp might be
   something that could be done statically...)
1. line numbers for things like log_info


[^1] Go, C# and the CLR, Java and the JVM, and other languages have
shown that GC doesn't have to be problematic though garbage collection
is still a difficult problem. Swift and Rust have other mechanisms
though these mechanisms are harder to explain and hence puts more
burden on the programmer.

