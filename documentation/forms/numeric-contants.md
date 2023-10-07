# Numeric Constants

The general principle is that numbers start with a decimal digit and
then contain 0-9, and (a-f for hexidecimal). Numbers may end with a
suffix to indicate their size is different from int64 (so that
compiler can detect when the number doesn't fit in the given size).

Floating-point numbers start with a digit as well but contain a
decimal point or a floating point size suffix. There are a few other
formats for scientific notation.

Numbers end when a non-alphanumeric (except ".") symbol or whitespace
is encounterd.

## Integer Constants

### Decimal

```
0
123
1000
1_000
1000000
1_000_000
100_uint16
```

### Hexidecimal

```
0x0
0xcafe
```

### Binary

```
0b0
0b11111
0b111110011011
0b1111_1001_1011
```

### Octal

This is different from standard C because not so much on source code
but parsing routines that treat a leading zero as indicating a
non-decimal string cause things to almost work in some cases but of
course not really.

```
0o777
```

### Floating Point Numbers

```
0.0
1000.0
1_000_000.0
1.0_float16
1.0_float64
```
