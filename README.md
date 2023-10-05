# Omni-C

A C like programming language strongly resembling C with modern conveniences,
default runtime safety, and a new take on a standard library.

## Additions to C23

1. namespaces, imports, and libraries (no header files!)
2. closures
3. garbage collection (possibly precise)
4. overloaded functions (like C++)
5. overloadable semantics for [] operator
6. improved syntax and safer semantics for typedef
7. improved enums (can be converted to strings, etc.)
8. more powerful structs (which can be turned off to be pure C structs for low-level
   compatibility when required)
10. generically typed functions and structures
12. consistent naming of types
13. readable style conventions
14. multiple array types (byte_array, array, fixed_unsized_array, fixed_sized_array, raw_array)
15. hashtree, hashtable, unsorted_set, sorted_set, sorted_map
16. guaranteed self tail calls
17. non-textual macros
18. dynamic references and match by type
19. switch statements don't have fallthrough
20. C23 style attributes, embed, etc.
21. multiple return results
22. exceptions
23. language level threads

Most of these features are straightforward to describe and operate like other languages.
The biggest departure from C is generic structures and functions. Without inheritance
these become simpler than one might expect (simpler than C++ templates).

Omni C aims to replace the aging C standard library with one that is more readable,
organized, and powerful.

## Features Removed from Standard C

1. many types of silent conversions
2. switch fall through
3. special syntax for array declarations (since we have multiple types of arrays)

## Using Omni C

Omni C will have at least one transpiler targeting C23. C23 can be compiled to
wasm which in theory may be enough to support interop with other languages.

Eventually we may add multiple native compilation targets such as:

1. LLVM
2. Comet-VM (RISC-V64, ARM64 - full tail recursion and amazing debugging)
3. wasm

## Omni C Syntax

The general rule is that the familar C syntax should be used except where it interferes
with other goals. Especially important are C expressions - we can deviate much more at
namespace level constructs like typedef.

The full syntax with examples will be provided in another file.

### Samples

```
// Files can contains code to place in multiple name spaces. namespace headers
// extend to the end of file or the next namespace declaration.
namespace std::collections::printer;

import std::collections::iterator;

byte_array* pretty_print(reference object) {
   // switch is able to switch on the type contained in an object reference.
   switch(object.type) {
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
