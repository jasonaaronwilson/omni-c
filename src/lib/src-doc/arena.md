# @file arena.c

(experimental)

This is an optional memory allocator that can be used to replace
allocate.c. Related terms to arena are memory pools, bump
allocation, and linear allocation.

Arena style allocation is especially well suited to long-lived
request based architectures like web-servers. For simple
command-line utilities, simply never calling free isn't the worst
option in the world on modern machines with 64bit address spaces
(and lots of RAM) though using an Arena in a few places where lots
of garbage is created can improve on this situation without a lot
of work.

Gemini suggested Meson, APR (pools), MicroAllocator, and MPS as
possible arena libraries you could use instead (though wiring those
up correctly into c-armyknife-lib would make it harder to provide
as a single-file library though I would accept PULL requests if
anyone wants to give it a try - just make sure it is opt-in of
course and folks should always have this arena implementation as a
fall-back). Like most of c-armyknife-lib, the emphasis is therefore
on correctness and simplicity over performance/memory-efficiency.

Since I didn't ask Gemini the right question, it didn't talk about
[Boehm garbage
collector](https://en.wikipedia.org/wiki/Boehm_garbage_collector). Initially
allocate.c was only meant to be a very thin shim on top of C's
malloc/free that would simply allow turning a knob to enable using
the Boehm collector (which I think would still be interesting to
"provide" at some point so I will probably do that), however
allocate.c evolved to have some interesting debugging options
(probablistic under-write/over-write detection) which may be kept
here.
 
## @struct arena_t

An arena_t represents both the abstract notion of an arena as well
as the head of a list of arena segments. Our hope is that arenas
are initialized in such as way that you don't end up with
additional segments very often so we will collect enough data to
help you figure that out when you need to.

The general idea is going to be to put a arena_t* into a
thread-local variable initialized from main. For now we will just
use a simple "global" variable (the_current_arena) instead since
c-armyknife-lib currently isn't focused on multi-threading. Using a
thread-local means we can effectively use a "fluid-let" *like*
(this is C) to control (via dynamic scoping) what the_current_arena
is which means we don't have to explicitly pass-around the arena
just to do an allocation in some remote leaf function. While
explicit data-dependency to this level has some advantages,
sometimes being able to ignore details like this can be useful to
keep things as simple as possible.
 
## @macro free_bytes

This is meant to be the same as free except that currently arena's
don't supporting free of any objects so this is simply a NOP.
 
## @macro malloc_bytes

This is essentially the same as malloc but the memory is always
zeroed before return it to the user. We use a macro here to call
checked_malloc so that the file and line number can be passed.
 
## @macro malloc_copy_of

This provides a convenient way to allocate a copy of a given
"source". Generally you would only use it with a pointer to a
structure though in theory it could be used on other things.

See also: string_duplicate which automatically calls strlen, etc.
 
## @macro malloc_struct

This provides a convenient way to allocate a zero-filled space big
enough to hold the given structure with sizeof automatically used
and the result automatically casted to a pointer to the given type.
 
## @function arena_checked_malloc

Allocate the given amount bytes or cause a fatal error. The memory
is also zeroed.

If possible, use the macros malloc_bytes or malloc_struct instead
for an easier to use interface. Those macros simply call
checked_malloc.
 
## @function checked_free

This is mostly a NOP currently.
 
## @function checked_malloc_copy_of

Allocate amount bytes and initialize it with a copy of that many
bytes from source.
 
