# Booleans

booleans are a type (`boolean`) of unsigned integer that has two
values `false` and `true` where false is zero and true is 1. By
convention variables and fields that are of type boolean and functions
that return boolean are suffixed with a question mark when this makes
them more readable.

```
boolean even?(int x) {
  boolean result = (x & 1) == 0;
  if (result) {
    return true;
  } else {
    return false;
  }
}
```

Booleans are passed to and returned from functions the same way that a
byte is passed or returned. However, when booleans are used in
structures, they are represented as a single bit and 64 booleans are
placed together by the structure packer to make this require as little
padding as possible (if you have greater than 64 booleans in a
structure as is likely, the field will be whichever smallest unsigned
integer will hold them all. This means booleans are relatively cheap
in a structure especially if you have several since they hardly take
up any space. When the `unpacked` or `c_compatible` attribute are
applied to the structure or when nocombine attribute is applied to a
field of type boolean then the field occupies an entire byte (and this
allows address operations to be used on that field otherwise that is
illegal).
