# @file allocate.c

Hook into malloc and free to make them more convenient and possibly
safer. For example, allocated memory is always zero'd and macros
like malloc_struct are more readable. We also have a novel LRU
memory-bounds checker that found a bug that valgrind didn't.

For missing calls to free, we are fully compatbile with valgrind
(since we just call malloc/free). (Valgrind also has a memcheck
mode though it actually masked a bug instead of finding it.)

We also have our own memcheck mode. The basic idea is to maintain a
LRU style lossy hashtable and to repeatedly scan this set of
allocations for overwrites. While this will not detect all
instances of an overwrite, when it does, it will be pretty
convenient to use in conjuction with a debugger to track the bug
down.

There should be no run-time penalty when our additional debugging
options are turned off (though I still like how valgrind doesn't
even require recompilation so maybe if our different techniques
pays off, perhaps we can port it into valgrind so please send
feedback if you find armyknife-lib's memcheck mode helpful or not.
 
## @macro free_bytes

This is essentially the same as free.
 
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
 
## @function checked_free

Allocate amount bytes or cause a fatal error. The memory is also
zeroed.

If possible, use the macros malloc_bytes or malloc_struct instead
for an easier to use interface. Those macros simply call
checked_malloc.
 
## @function checked_malloc

Allocate the given amount bytes or cause a fatal error. The memory
is also zeroed.

If possible, use the macros malloc_bytes or malloc_struct instead
for an easier to use interface. Those macros simply call
checked_malloc.
 
## @function checked_malloc_copy_of

Allocate amount bytes and initialize it with a copy of that many
bytes from source.
 
## @debug_compiliation_option ARMYKNIFE_MEMORY_ALLOCATION_END_PADDING

The amount of padding to place at the back of each allocation. We
have several ways to then check that no one overwrites this padding
though we won't catch every case.

Unlike start padding, this does not effect alignment and so values
as small as 1 make perfect sense though I still recommend 4 or 8
bytes especially on 64bit big-endian architectures.
 
## @debug_compiliation_option ARMYKNIFE_MEMORY_ALLOCATION_HASHTABLE_SIZE

This determine how big the lossy hashtable is. On every allocation
or deallocation the *entire* lossy hashtable is scanned to see if
the padding bytes have been perturbed which makes it possible to
find some memory overwrite errors earlier than waiting for the free
call (or potentially even if the memory isn't ever freed).

It makes no sense to set this unless either
ARMYKNIFE_MEMORY_ALLOCATION_START_PADDING or
ARMYKNIFE_MEMORY_ALLOCATION_END_PADDING is non-zero.
 
## @debug_compiliation_option ARMYKNIFE_MEMORY_ALLOCATION_MAXIMUM_AMOUNT

This is just a fail-safe to catch extremely dumb allocation amounts
(at least as of 2023). If you know you will have a fixed size
amount of memory, you could set this lower and potentially get a
slightly nicer error message.
 
## @debug_compiliation_option ARMYKNIFE_MEMORY_ALLOCATION_START_PADDING

The amount of padding to place in front of each allocation. We have
several ways to then check that no one overwrites this padding
though we won't catch every case.

This should be a multiple of 8 or else the expected alignment
(which malloc doesn't make that explicit...) will be broken.
 
