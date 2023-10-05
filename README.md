# Omni-C

A C like programming language strongly resembling C with modern conveniences
such as:

1. Namespace, imports, and libraries (no header files!)
2. ML style closures
3. Garbage collection (possibly precise)
4. Overloaded functions like C++
5. Overloadable semantics for [] operator
6. Improved Syntax and safer semntics for typedef
7. Improved enums
8. More powerful structs (which can be turned off to be pure C structs mathcing the platform)
9. generically typed functions
10. generically typed structures (possible inlined)
11. consistent naming of types
12. readable style conventions
13. multiple array types (byte_array, array, fixed_unsized_array, fixed_sized_array, raw_array)
14. hashtree, hashtable, unsorted_set, sorted_set, sorted_map
15. Guaranteed self tail calls
16. Improved Macros
17. dynamic references and match by type
18. switch statements don't have fallthrough
19. C23 style attributes, embed, etc.

Omni C is meant to have multiple transpiled (mostly readable) targets:

1. C23

C23, and Go can all be compiled to wasm though we may add multiple native compilation targets:

1. LLVM
2. Comet-VM (RISC-V64, ARM64)
3. wasm

## Omni C Syntax

The general rule is that the familar C syntax should be used except where it interferes
with other goals. Especially important are C expressions - we can deviate much more at
program level constructs like typedef.

The full syntax with examples will be provided in another file.

### Samples

```
// Files can contains code to place in multiple name spaces. namespace headers
// extend to the end of file or the next namespace declaration.
namespace std::collections::printer;

import std:collections::iterator;

byte_array*, error pretty_print(reference object) {
   // switch is able to switch on the type contained in an object reference.
   switch(object.type) {
      case int64 | uint64 | float64 | float32:
        return pretty_print_number(ref);
      case string:
        return print_print_string(ref);
   }
   itertor(any)* iterator = object.type.iterator(object);
   if (iterator) {
      return pretty_print(byte_array, iterator), no_error();
   }
   auto stream = object.type.code_point_stream(object);
   if (append_source) {
     return byte_array.append(stream), no_error();
   }
   return null, error("object is not printable by normal means")
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

## Omni C Implementation

Omni C will utilize the tree-sitter parse framework and it's reference grammar will be a tree-sitter grammar.

The Omni C transpiler will be written in C23. Excluding the tree-sitter grammar and generated parser, we expect
expect the transpiler to be about 5K lines of code with an additional 3K of library code.

Any additional backends will be written in Omni C of course.
