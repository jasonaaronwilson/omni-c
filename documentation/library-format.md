# Library Format

A library is simply a collection of files. The are used to bundle
together things to simplify sharing, etc. They can also be used to
help with abstracting a build process itself.

Both platform independent and platform dependent libraries can be
easily shared since it is a single file and will form our version of
node packages, go cargos, etc. Therefore libraries must be extensible
to a certain degree and have other metadata.

Libraries are platform dependent if they contain either compiled
object files, static libraries, or dynamic libraries (though they can
support multiple platforms!) Things like images or non compiled
resources to no make a library platform dependent.

## Library Header Lines

The normal encoding of a line in a utf-8 file on Unix would end with
the code-point U+000A. Instead we use code-point U+0000, aka NUL, aka
the C string terminator to end each header "line".

Lines always looks like this:

```
KEY=VALUE
```

## Library Header

The library header is at the begining of the file and ends with an
empty line. A library header can not contain data (essentially every
thing after word is the data for the library header).

Libraries have the following predefined keys:

```
OMNI=1
platform-dependent=1/0
platforms=<platform-tags>[,<platform-tags>*]
```

OMNI=1 is always included as the first "line" so that OMNI can be used
as a magic number.

After the library header, zero or more library items may be present.

## Library Items

Library items contain a library item header and then an optional
number of raw bytes.

The format of a library item header is exactly the same as a library
header, i.e., a series of lines with an empty line at the end. The
expected keys are of course different, but the format is
identicial. This should make sharing parse code quite simple.

One key is special, data-length=<decimal-ten-number-of-bytes>. When
this key is present, immediately after the empty line are that number
of bytes.

The known keys for library items are:

```
type=<embed-data|source-file>
file-name=foo/bar/file.oc
data-length=<number-of-bytes>
```
