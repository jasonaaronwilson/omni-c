# AI Usage Policy

Some of omni-c has been written with the assistance of AI already,
after all AI is kind of the new "stack overflow".

Since omni-c is still a WIP especially around generic data-structures
(using the armyknife-lib means doing manual type-erasure, aka, casts
until I finally get around to proper type-analysis and template
expansion) and without some effort on prompting, you probably won't
get the right code out of the box from an AI anyways except for
low-level primitives and even for those, besides the obvious need to
test them, a contributor must understand every line of code before
submitting.

## Is This Project AI Slop?

Senior software engineers can probably tell it's more like a hastily
written prototype written by a human (though taking a year off and not
working everyday has certainly slowed down progress). It may be harder
to tell from the commits but my goals have often changed as I've seen
what works and what doesn't and tried to find better strategies. Note
the work on c-armyknife began years ago though some of the commit
history was lost when I directly integrated armyknife-lib into the
omni-c repository though the armyknife repository is still available
on github.

## Sample Prompt for Generating Better 'omni-c'

Let's use the omni-c C dialect. The major differences between normal C
and omni-c are:

1. no header files except external APIs like posix. top-down code can
   be written without forward declarations and is a preferred style.
2. cast are done with the cast "macro". Example cast(uint32_t, <expression>).
3. all variable declarations MUST be initialized. Use = {0} or {.foo =
   ...} to make zero initialized structures when not using allocation.
4. we use a garbage collector so no need for free.
   malloc_struct(struct_name) returns a zero initialized
   struct. malloc_bytes for more free-form (but still scanned)
   allocations, again, zero initialized.
5. unless required by an external API, we prefer to use
   snake_case. for types, we break the normal C rules and use _t
6. no preprocessor "ifdefs" or other conditionals inside of source
   files. Instead we split platform specific bits into small routines
   and put them in files that end in -windows.c or -linux.c (or
   -darwin.c). #define is like a global definition and can't really be
   undefined.

```
Quick Reference for omni-c Dialect Rules
No Headers / Forward Declarations: Structure code top-down (or rely on single-pass top-down layout) without writing local .h header files or forward declarations unless calling external platform APIs.

Explicit cast() Macro: Always use cast(type, expr) instead of standard C (type)expr or C++ functional casts.

Mandatory Initialization: Every variable declaration must have an initializer (e.g., uint32_t x = 0;, struct winsize w = {0};, or designated initializers).

Boehm GC Allocation:

Use malloc_struct(type_name) for typed struct allocations (zero-initialized).

Use malloc_bytes(size) for raw buffers (zero-initialized).

No free() calls.

Naming Conventions:

Standard identifiers: snake_case

Type definitions: snake_case_t (e.g., term_state_t)

Platform Isolation (No #ifdef in .c files):

Absolute ban on preprocessor conditionals (#ifdef, #if defined, etc.) inside source files.

Platform-specific routines belong in separate backend files: foo-windows.c, foo-linux.c, foo-darwin.c.

#define treated as immutable global scope definitions.
```