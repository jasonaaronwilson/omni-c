# use

`use` allows symbols from another namespace to be "imported" into the
current namespace.

## packages

When a `use` is prefixed with certain properties, this can also
specify a package of code of some sort, possibly from a remote
computer (but cached locally) to be made available to this file (and
consequently to any file using the same namespace in the compilation
unit).

```
[git="--branch 29.5 http://github.com/jasonaaronwilson/sorting"]
use sorting::*;

[git="http://github.com/jasonaaronwilson/math-stuff"]
use "numerical-algorithms"::*;

[url="http://github.com/jasonaaronwilson/sorting/29.5.oca"]
use sorting::*;

[url="http://some-download-service.com/blah-blah-sorting-29.5.oca"]
use sorting::*;

[url="/users/jawilson/sorting-library-29.5.oca"]
use sorting::*;
```

When the git property is added, it is possible to start with a branch
tag in order that you may more carefully control what version of code
you compile against.

When the url proprty is added, if means the URL will point to an "oca"
file which is a library format understood by Omni C.

## platform property and platform dependencies

When a `platform` is placed on any definition or use statement, that
statement has no effect unless the compiler is targeting that
platform.

## dependencies.oc

For small programs, especially one file programs, explicitly defining
dependencies is not very common. For larger programs, dependency
mangagement becomes more crucial. By convention a single file called
"dependencies.oc" is frequently used to specify all of the
dependencies that are used across the entire project. It is simply a
valid Omni C source code file but it typically only contains `use`
statments and comments.



