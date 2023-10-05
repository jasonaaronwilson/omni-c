# Omni C Language Specification

## Version

The current version is "OC.2023.0".

## Reserved Words

```
auto, void, boolean, int64, int32, int16, int8, uint64, uint32, uint16, uint8,
utf8_code_point, float32, float64, complex32, complex64, if, do, while, switch, case, break,
for, typdef, enum, struct, string, try, catch, throw, defer, goto, else,
continue, default, nullptr, true, false, static_assert, typedef_lower, try, catch,
```

## Standard annotations

noreturn, inline, tailcall, etc.

## Operators & Precedence

All of the operators defined by C23 with the same precedence rules (except
"->" for which "." is used).

## Namespace Statements

typedefs, structs, functions, enumerations, variables,

## Full Grammar

The reference grammer for this version is in tree-sitter grammar.

## Standard Types and Libraries

### Types

byte_array,
complex64,
complex32,
iterator,
map,
hashtree,
array,
fixed_array(N, x),
sorted_set(T),
source(T) (aka iterable)
sink(T),
unsorted_set,




