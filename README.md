# Omni-C

A C like programming language strongly resembling C with modern conveniences
such as:

1. Namespace/Modules (no header files!)
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

Omni C is meant to have multiple transpiled (mostly readable) targets:

1. C23
3. Go

C23, and Go can all be compiled to wasm though we may add multiple native compilation targets:

1. LLVM
2. Comet-VM (RISC-V64, ARM64)
3. wasm

## Omni C Syntax

The general rule is that the familar C syntax should be used except where it interferes with other goals. 
Especially important are C expressions - we can deviate much more at program level constructs like typedef.

The full syntax with examples will be provided in another file.

## Omni C Implementation

Omni C will utilize the tree-sitter parse framework and it's reference grammar will be a tree-sitter grammar.

The Omni C transpiler will be written in C23. Excluding the tree-sitter grammar and generated parser, we expect
expect the transpiler to be about 5K lines of code with an additional 3K of library code.

Any additional backends will be written in Omni C of course.
