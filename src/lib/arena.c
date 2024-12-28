#ifdef C_ARMYKNIFE_LIB_USE_ARENAS
#line 3 "arena.c"

/**
 * @file arena.c
 *
 * (experimental)
 *
 * This is an optional memory allocator that can be used to replace
 * allocate.c. Related terms to arena are memory pools, bump
 * allocation, and linear allocation.
 *
 * Arena style allocation is especially well suited to long-lived
 * request based architectures like web-servers. For simple
 * command-line utilities, simply never calling free isn't the worst
 * option in the world on modern machines with 64bit address spaces
 * (and lots of RAM) though using an Arena in a few places where lots
 * of garbage is created can improve on this situation without a lot
 * of work.
 *
 * Gemini suggested Meson, APR (pools), MicroAllocator, and MPS as
 * possible arena libraries you could use instead (though wiring those
 * up correctly into c-armyknife-lib would make it harder to provide
 * as a single-file library though I would accept PULL requests if
 * anyone wants to give it a try - just make sure it is opt-in of
 * course and folks should always have this arena implementation as a
 * fall-back). Like most of c-armyknife-lib, the emphasis is therefore
 * on correctness and simplicity over performance/memory-efficiency.
 *
 * Since I didn't ask Gemini the right question, it didn't talk about
 * [Boehm garbage
 * collector](https://en.wikipedia.org/wiki/Boehm_garbage_collector). Initially
 * allocate.c was only meant to be a very thin shim on top of C's
 * malloc/free that would simply allow turning a knob to enable using
 * the Boehm collector (which I think would still be interesting to
 * "provide" at some point so I will probably do that), however
 * allocate.c evolved to have some interesting debugging options
 * (probablistic under-write/over-write detection) which may be kept
 * here.
 */

#ifndef _ARENA_H_
#define _ARENA_H_

#include <stddef.h>
#include <stdint.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>

#ifndef ARMYKNIFE_MEMORY_ALLOCATION_MAXIMUM_AMOUNT
#define ARMYKNIFE_MEMORY_ALLOCATION_MAXIMUM_AMOUNT (1L << 48)
#endif

/**
 * @struct arena_t
 *
 * An arena_t represents both the abstract notion of an arena as well
 * as the head of a list of arena segments. Our hope is that arenas
 * are initialized in such as way that you don't end up with
 * additional segments very often so we will collect enough data to
 * help you figure that out when you need to.
 *
 * The general idea is going to be to put a arena_t* into a
 * thread-local variable initialized from main. For now we will just
 * use a simple "global" variable (the_current_arena) instead since
 * c-armyknife-lib currently isn't focused on multi-threading. Using a
 * thread-local means we can effectively use a "fluid-let" *like*
 * (this is C) to control (via dynamic scoping) what the_current_arena
 * is which means we don't have to explicitly pass-around the arena
 * just to do an allocation in some remote leaf function. While
 * explicit data-dependency to this level has some advantages,
 * sometimes being able to ignore details like this can be useful to
 * keep things as simple as possible.
 */
typedef struct arena_S {
  // This is essentially a bump pointer and is always kept 16 byte
  // aligned (the default alignment of all allocations). Although I
  // saw at least one argument for growing arena's backwards, the
  // current implementation will grow upwards for now. The general
  // allocation interface doesn't specify the direction of growth but
  // we are keeping it simple and predictable by growing from lower to
  // higher.
  //
  // When current > end, this means that this arena (segment) and all
  // of it's sibling segments have been closed (deallocated). It's
  // definitely a problem to try to allocate into such an arena, look
  // at previous segments, look at objects between start and end,
  // etc. Normally we would use "nullptr" to indicate this condition
  // because it's kind of more natural in the C world but setting to
  // end means we can use a single comparision rather than two
  // comparisions before using this as a bump pointer in the common
  // case of allocating a small object that will fit into the current
  // segment.
  uint64_t current;

  // This is the first address that is greater than the memory
  // allocated to the arena.
  uint64_t end;

  // This is the address of the start of the arena. Since this may
  // come from malloc and in some cases may not have the desired 16
  // byte alignment.
  uint64_t start;

  // This is the (potentially rounded up to at least a 4K page size in
  // the future) number of bytes for each "segment" of an arena. When
  // utilizing malloc instead of mmap(), you may want to keep this
  // value low. This is stored explicitly since in order to support
  // objects larger than the segment size, `end - start` is not
  // necessarily the right default when allocating a new segment when
  // this segment becomes full.
  uint64_t default_segment_size;

  // A list of the older "segments" that are logically part of this
  // arena. Closing an arena will also close all previous_segments so
  // one should only close the current head segment.
  struct arena_S* previous_segments;
} arena_t;

// TODO(jawilson): make thread local when we finally support threads
arena_t* the_current_arena = NULL;

extern arena_t* arena_open(uint64_t default_segment_size);

extern void arena_close();

extern uint8_t* arena_checked_malloc(char* file, int line, uint64_t amount);

extern uint8_t* arena_checked_malloc_copy_of(char* file, int line,
                                             uint8_t* source, uint64_t amount);

extern void arena_checked_free(char* file, int line, void* pointer);

// TODO(jawilson):
//
// arena_close();
// get_current_arena(); ?
// statistics, logging, and debugging ?

/**
 * @macro malloc_bytes
 *
 * This is essentially the same as malloc but the memory is always
 * zeroed before return it to the user. We use a macro here to call
 * checked_malloc so that the file and line number can be passed.
 */
#define malloc_bytes(amount) (arena_checked_malloc(__FILE__, __LINE__, amount))

/**
 * @macro free_bytes
 *
 * This is meant to be the same as free except that currently arena's
 * don't supporting free of any objects so this is simply a NOP.
 */
#define free_bytes(ptr) (arena_checked_free(__FILE__, __LINE__, ptr))

/**
 * @macro malloc_struct
 *
 * This provides a convenient way to allocate a zero-filled space big
 * enough to hold the given structure with sizeof automatically used
 * and the result automatically casted to a pointer to the given type.
 */
#define malloc_struct(struct_name)                                             \
  ((struct_name*) (arena_checked_malloc(__FILE__, __LINE__,                    \
                                        sizeof(struct_name))))

/**
 * @macro malloc_copy_of
 *
 * This provides a convenient way to allocate a copy of a given
 * "source". Generally you would only use it with a pointer to a
 * structure though in theory it could be used on other things.
 *
 * See also: string_duplicate which automatically calls strlen, etc.
 */
#define malloc_copy_of(source, number_of_bytes)                                \
  (arena_checked_malloc_copy_of(__FILE__, __LINE__, source, number_of_bytes))

#endif /* _ARENA_H_ */

// ======================================================================

uint64_t number_of_segments_allocated = 0;
uint64_t number_of_segments_deallocated = 0;
uint64_t number_of_segment_bytes_allocated = 0;

uint64_t number_of_bytes_allocated = 0;
uint64_t number_of_malloc_calls = 0;
uint64_t number_of_free_calls = 0;

arena_t* arena_allocate(uint64_t segment_size, uint64_t default_segment_size,
                        arena_t* previous_segments) {
  if (segment_size < 4096 || default_segment_size < 4096) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }

  arena_t* result = cast(arena_t*, malloc(sizeof(arena_t)));
  if (result == NULL) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  memset(result, 0, sizeof(arena_t));
  result->default_segment_size = default_segment_size;

  // Finally allocate the space for the allocations themselves
  result->start = cast(uint64_t, malloc(segment_size));
  if (result->start == 0) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  memset(cast(uint8_t*, result->start), 0, segment_size);
  result->current = (result->start + 0xf) & ~0xf;
  result->end = (result->start + segment_size) & ~0xf;

  number_of_segments_allocated += 1;
  number_of_segment_bytes_allocated += segment_size;

  return result;
}

void arena_deallocate(arena_t* arena) {
  while (arena != NULL) {
    arena_t* next = arena->previous_segments;
    free(cast(uint8_t*, arena->start));
    free(arena);
    arena = next;
  }
}

extern arena_t* arena_open(uint64_t default_segment_size) {
  the_current_arena
      = arena_allocate(default_segment_size, default_segment_size, NULL);
  return the_current_arena;
}

extern void arena_close() {
  arena_deallocate(the_current_arena);
  the_current_arena = NULL;
}

/**
 * @function arena_checked_malloc
 *
 * Allocate the given amount bytes or cause a fatal error. The memory
 * is also zeroed.
 *
 * If possible, use the macros malloc_bytes or malloc_struct instead
 * for an easier to use interface. Those macros simply call
 * checked_malloc.
 */
uint8_t* arena_checked_malloc(char* file, int line, uint64_t amount) {
  if (the_current_arena == NULL) {
    fatal_error(ERROR_ILLEGAL_STATE);
  }

  if (amount == 0 || amount > ARMYKNIFE_MEMORY_ALLOCATION_MAXIMUM_AMOUNT) {
    fatal_error(ERROR_BAD_ALLOCATION_SIZE);
  }

  number_of_malloc_calls += 1;
  amount = (amount + 0xf) & 0xf;

  if (the_current_arena->current + amount < the_current_arena->end) {
    // memset is not required here because we do memset after
    // alocating the segment (or later will use mmap to get zero
    // filled pages).
    number_of_bytes_allocated += amount;
    uint8_t* result = cast(uint8_t*, (the_current_arena->current));
    the_current_arena->current += amount;
    return result;
  } else {
    // TODO(jawilson): Allocate a new segment, at least large enough
    // to hold this allocation.
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  // This should be reached.
  return 0;
}

/**
 * @function checked_malloc_copy_of
 *
 * Allocate amount bytes and initialize it with a copy of that many
 * bytes from source.
 */
uint8_t* arena_checked_malloc_copy_of(char* file, int line, uint8_t* source,
                                      uint64_t amount) {
  uint8_t* result = arena_checked_malloc(file, line, amount);
  memcpy(result, source, amount);
  return result;
}

/**
 * @function checked_free
 *
 * This is mostly a NOP currently.
 */
void arena_checked_free(char* file, int line, void* pointer) {
  number_of_free_calls += 1;
}

#endif /* C_ARMYKNIFE_LIB_USE_ARENAS */
