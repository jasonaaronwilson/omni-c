# Namespaces

Namespaces are used to organize code and prevent name collisions in
large code bases.

Every definition occurs in some "scope" and namespaces are used to
define these scopes.

An identifier can be refered to by a fully qualified name which
includes the namespace it was defined in:

```
std::stdout

googgle::common::logger
```

An identifier in a namespace may not be truly unique. For example a
type may have the same name as a function or a global variable. The
compiler always takes this into account when resolving identifiers and
the confusion to human readers is typically not problematic.

In some sense, the "::" could be replaced with "__" to form longer but
legal identifiers to achieve some of the same qualification propreties
(and this is exactly how identifers are exported to C) but the Omni C
compiler allows non qualified identifers which improves readability by
allowing much shorter names to be used.

To place new definitions inside a namespace, use the following syntax:

```
namespace namespace_name;

    or

namespace some_prefix::namespace_name;

    or

namespace these_namespaces::can_have::many_prefixes;
```

See the section on security below. (Essentially in the present form,
namespaces provide very little actual security. (Stay tuned though.))

Once a namespace is declared, all identifiers until the end of file or
until a new namespace is declared, are defined in that namespace and
hence have longer fully qualified names than what is written.

To refer to an identifier in an other namespace, you can use the fully
qualitfied name of the identifier. For example, to refer to the
function `bar()` in the namespace `my_namespace::my_other_namespace`,
you would use the following syntax:

```
my_namespace::my_other_namespace::bar();
```

Identifiers in other namespaces can be used without fully qualify them
via the "use" keyword (similar to using, import, etc. from other
languages). The use keyword accepts the "*" qualifier to allow all
"publically" defined identifiers in an entire namespace to become
visible for the duration of the the definitions in the current
namespace. Items in the namespace that are not annotated with public
are typically referred to as "private" or "namespace private" or
"package private".

```
namespace foo;

use bar::*;

void this_is_not_visible() {
}

[[public]]
void this_is_visible() {
}
```

```
namespace bar;

import foo::this_is_visible;

void example() {
  this_is_visible();
}
```

Only symbols marked with the "public" annotation are visible outside
of the namespace. This further prevents naming collisions but more
importantly clearly indicates intent so that an implementation detail
is not treated equivalently as part of a libraries API. This
furthermore makes it much more palatable to include ALL of the
identifiers from a namespace using the wildcard "*" with a use
statement.

## The Default Namespace

Without a namespace, Omni C definitions go into the default unnamed
namespace. This is normal for small to medium sized programs like
"scripts" or personal programs. This obviates the need to invent a
name for a namespace that will otherwise have little value.

It is "impossible" to `use` identifiers from the default namespace
from other namespaces.

## The program:: namespace

By convention, namespaces underneath program:: are used for non
library namespaces related to implementing one or more programs. If
you were to write a program canonically called "markdown-lint" then
you might place the code into the namespace program::"markdown-lint"
and intentionally expose part of that namespace so that another
program, say markdown-format, can call into the linter implementation
without going through an OS process abstraction.

## The C namespace

This special namespace is where C code is placed by default.

```
namespace program::cat;

use c::putc;
use c::stdout;

void example() {
  putc('o', stdout);
}

```

## Security and Isolation

Namsespace are not currently meant to be used for security since they
are not even *obscure* and as soon as unsafe code is written, any
trivial measures (aka standard obscurity techniques) are easily
circumvented.

It may be possible in the future to provide hardened isolation between
namespaces, aka, sandboxing.

## Ordering

The order of definitions in a namespace never matters and namespaces
are allowed to have mutual dependencies.

