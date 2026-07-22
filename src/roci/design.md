# The Roci VM Design

This document describes the initial implementation choices in the Roci
VM which are somewhat unusual but fairly simple.

## Runtime Data Model

The most important constraint is that the Roci VM is designed to be
compatible with the hans-boehm conservative garbage collector. This
generally means that pointers must live on aligned addresses.

## Heap Values

When data lives in the heap, we represent "atoms" with a 16 byte
structure containing a raw 64bit value and a 64bit "tag". This 2X
increase just to hold the tag is where fancy implementations of
dynamic languages get very creative while we have chosen the simplest
choice possible.

Note that lists are currently implemented very inefficiently as well
(if we had C++ vectors in omni-c right now we could get rid of a
needless level of indirection or do better by having a dense tag array
and a 64bit chunk raw data array).

## Programs (Basic Blocks)

For simplicity, the basic unit of a compiled program is the basic
block instead of say all of the bblocks in a function or all of the
bblocks in a file, etc. (This may effect performance.) These are
garbage collected like any other C structure.

Given the limited number opcodes, we only need a single byte to
represent them which means the natural interleaving of a byte with a
64bit pointer would violate the GC alignment rule. So we use this
unusual layout.

```
            .align 8
[ HEADER  ] 64bits * 1
[ DATA    ] 64bits * ?
[ OPCODES ] 8bit * ?
```

## Stacks

Since the VM is stack based, we have a value (data) stack. This is
currently organized as two separate stacks, one for tags and one for
64bit raw values (like how we would have implemented lists).

With real hardware and many virtual machines, the stack typically
contains return addresses though we have a separate stack just for
continuations. This probably vastly simplifies the implementation of
tail-recursion.
