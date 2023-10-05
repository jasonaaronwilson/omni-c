# Omni-C

A C like programming language very strongly resembling C with modern
conveniences, default runtime safety, and a new take on a standard library.

## Additions to C23

1. namespaces, imports, and libraries (no header files!)
1. closures
1. garbage collection (possibly precise, manual management still possible)
1. overloaded functions (like C++)
1. overloadable semantics for \[\] operator, properties, and method/chaining
   syntax
1. improved syntax and safer semantics for typedef
1. improved enums (can be converted to strings, etc.)
1. more powerful structs (which can be turned off so pure C structs are still
   available for low-level compatibility when required)
1. generically typed functions and structures (not as complex as you think!)
1. updated and consistent naming of types (like Rust perhaps) and other style
   conventions focused on readability instead of number of characters
1. multiple array types (byte_array, array, fixed_array, unsafe_array) and
   bounds checking
1. hashtree, hashmap, hashset, tree, immutable_tree
1. guaranteed self tail calls
1. non-textual macros (I have no idea what this looks like yet)
1. dynamic references and match by type (like any in Go)
1. switch statements don't have fallthrough
1. C23 style attributes, embed, etc.
1. multiple return results
1. exceptions
1. language level threads
1. scheme like "let" expressions allows statements to be executed in expression
   contexts
1. defer and block_defer for easier interaction with exceptions and code
   organization
1. utf8 immutable zero strings (convertible at not cost to C char\* style
   strings)
1. interpolated strings remove the need for printf
1. fluid_let global variables don't have to be bad?
1. block scope for case constructs so variables can be defined without resorting
   to an additional block which makes the code uglier

Importantly mising from this feature list is inheritance and therefore most of
these features are straightforward to describe and most constructs appear in
some other C like language. Most of the benefits of OOP are provided by runtime
introspection and some syntactic sugar.

The biggest change from C is probably generic structures and functions. Without
inheritance these become simpler than one might expect (simpler than C++
templates).

Omni C also aims to replace the aging C standard library with one that is more
readable, organized, less error prone and powerful. When using the transpiler,
the C standard library and other C libraries are still easily utilized.
(Additionally, non generic Omni C is trivial callable from C allowing a graceful
conversion of a code base from Standard C to Omni C.)

## Features Removed from Standard C

1. many types of silent conversions
1. switch fall through
1. special syntax for array types (since we have multiple types of arrays)
1. many obsolete keywords like register and the auto storage specifier (static
   still present)

## Using Omni C

Omni C by default is a transpiler targeting C and wasm via C to wasm
translation.

Eventually we may add multiple native compilation targets such as:

1. LLVM
1. Comet-VM (interpreter, RISC-V64, ARM64 - full tail recursion and amazing
   debugging)
1. wasm

## Omni C Syntax

The general rule is that the familar C syntax should be used except where it
interferes with other goals. Especially important are C expressions - we deviate
somewhat more at namespace level constructs like typedef.

The full syntax with examples will be provided in another file but a motivating
example is shown below.

### Samples

```
// Files can contains code to place in multiple name spaces. namespace headers
// extend to the end of file or the next namespace declaration.
namespace std::collections::printer;

import std::collections::iterator;

byte_array* pretty_print(reference object) {
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

Most C language successors, with the exception of Go, deviate significantly from
C's simplity usually with the introduction of inheritance. C is a painful
language to write large programs in but a few tweaks and this changes
dramatically.

Omni C might be closest to Java or C# without classes but still with a nod to
low-level programming use cases and of course AOT (ahead-of-time compilation)
being the default model.

Targeting C means that Omni C can run just about everywhere (including the
browser via wasm). wasm also allows Omni C code to be used from other
programming languages that have wasm interop.

The features I've included in Omni C have been vetted by 25+ years of
professional software engineering including many large projects across the
entire stack (I've written "microcode" level code, backend server code, command
line utilities, web frontends, etc.) I've had an interest in programming
languages design and implementation even longer than that.

Most importantly Omni C is the language that I personally want to write my code
in. I want the code to look exactly the way I expect, I want the feature set
that I think will make me most productive even if this means giving up some
overall performance. I want my code to run anywhere I need it and be as easy to
debug as possible. I want to be able to print out code like a book and I want
the code to be readable and writable without a fancy IDE. I want a efficient
target for code generation.

I think there are some dynamic languages which are easy to read and write but
static type checking is essential to me because I tend to introduce numerous
small errors in programs as I write them and the static type checking system
finds many of these immediately without even running tests. (And again, many of
these dynamic languages still employ inheritance.)
