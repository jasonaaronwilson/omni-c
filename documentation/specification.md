# Omni C Language Specification

## Version

The current version is "OC.2023.0".

## Reserved Words

```
auto
boolean
break
case
catch
catch
complex32
complex64
continue
default
defer
do
else,
enum
false
float32
float64
for
goto
if
int16
int32
int64
int8
nullptr
static_assert
string
struct
switch
throw
true
try
try
typedef
typedef_lower
uint16
uint32
uint64
uint8
utf8_code_point
void
while
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




