# Omni C Language Specification

## Version

The current version is "OMNI-C2023.0".

## Reserved Words

```
auto		do		int32		try
boolean		else		int64		typedef
break		enum		int8		typedef_lower
case		false		nullptr		uint16
catch		float32		static_assert	uint32
complex32	float64		string		uint64
complex64	for		struct		uint8
continue	goto    	switch		utf8_code_point
default		if		throw		void
defer		int16		true		while
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




