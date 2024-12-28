#line 2 "allocate.c"

/**
 * @file allocate.c
 *
 * Hook into malloc and free to make them more convenient and possibly
 * safer. For example, allocated memory is always zero'd and macros
 * like malloc_struct are more readable. We also have a novel LRU
 * memory-bounds checker that found a bug that valgrind didn't.
 *
 * For missing calls to free, we are fully compatbile with valgrind
 * (since we just call malloc/free). (Valgrind also has a memcheck
 * mode though it actually masked a bug instead of finding it.)
 *
 * We also have our own memcheck mode. The basic idea is to maintain a
 * LRU style lossy hashtable and to repeatedly scan this set of
 * allocations for overwrites. While this will not detect all
 * instances of an overwrite, when it does, it will be pretty
 * convenient to use in conjuction with a debugger to track the bug
 * down.
 *
 * There should be no run-time penalty when our additional debugging
 * options are turned off (though I still like how valgrind doesn't
 * even require recompilation so maybe if our different techniques
 * pays off, perhaps we can port it into valgrind so please send
 * feedback if you find armyknife-lib's memcheck mode helpful or not.
 */

#ifndef _ALLOCATE_H_
#define _ALLOCATE_H_

#include <stdint.h>

extern uint8_t* checked_malloc(char* file, int line, uint64_t amount);
extern uint8_t* checked_malloc_copy_of(char* file, int line, uint8_t* source,
                                       uint64_t amount);
extern void checked_free(char* file, int line, void* pointer);

extern void check_memory_hashtable_padding();

/**
 * @macro malloc_bytes
 *
 * This is essentially the same as malloc but the memory is always
 * zeroed before return it to the user. We use a macro here to call
 * checked_malloc so that the file and line number can be passed.
 */
#define malloc_bytes(amount) (checked_malloc(__FILE__, __LINE__, amount))

/**
 * @macro free_bytes
 *
 * This is essentially the same as free.
 */
#define free_bytes(ptr) (checked_free(__FILE__, __LINE__, ptr))

/**
 * @macro malloc_struct
 *
 * This provides a convenient way to allocate a zero-filled space big
 * enough to hold the given structure with sizeof automatically used
 * and the result automatically casted to a pointer to the given type.
 */
#define malloc_struct(struct_name)                                             \
  ((struct_name*) (checked_malloc(__FILE__, __LINE__, sizeof(struct_name))))

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
  (checked_malloc_copy_of(__FILE__, __LINE__, source, number_of_bytes))

// TODO(jawilson): malloc_copy_of_struct

#endif /* _ALLOCATE_H_ */

// ======================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

boolean_t is_initialized = false;
boolean_t should_log_value = false;

uint64_t number_of_bytes_allocated = 0;
uint64_t number_of_malloc_calls = 0;
uint64_t number_of_free_calls = 0;

static inline boolean_t should_log_memory_allocation() {
  if (is_initialized) {
    return should_log_value;
  }
  char* var = getenv("ARMYKNIFE_LOG_MEMORY_ALLOCATION");
  is_initialized = true;
  if (var != NULL && strcmp(var, "true") == 0) {
    should_log_value = true;
  }
  return should_log_value;
}

/**
 * @debug_compiliation_option ARMYKNIFE_MEMORY_ALLOCATION_START_PADDING
 *
 * The amount of padding to place in front of each allocation. We have
 * several ways to then check that no one overwrites this padding
 * though we won't catch every case.
 *
 * This should be a multiple of 8 or else the expected alignment
 * (which malloc doesn't make that explicit...) will be broken.
 */
#ifndef ARMYKNIFE_MEMORY_ALLOCATION_START_PADDING
#define ARMYKNIFE_MEMORY_ALLOCATION_START_PADDING 8
#endif

/**
 * @debug_compiliation_option ARMYKNIFE_MEMORY_ALLOCATION_END_PADDING
 *
 * The amount of padding to place at the back of each allocation. We
 * have several ways to then check that no one overwrites this padding
 * though we won't catch every case.
 *
 * Unlike start padding, this does not effect alignment and so values
 * as small as 1 make perfect sense though I still recommend 4 or 8
 * bytes especially on 64bit big-endian architectures.
 */
#ifndef ARMYKNIFE_MEMORY_ALLOCATION_END_PADDING
#define ARMYKNIFE_MEMORY_ALLOCATION_END_PADDING 8
#endif

/**
 * @debug_compiliation_option ARMYKNIFE_MEMORY_ALLOCATION_HASHTABLE_SIZE
 *
 * This determine how big the lossy hashtable is. On every allocation
 * or deallocation the *entire* lossy hashtable is scanned to see if
 * the padding bytes have been perturbed which makes it possible to
 * find some memory overwrite errors earlier than waiting for the free
 * call (or potentially even if the memory isn't ever freed).
 *
 * It makes no sense to set this unless either
 * ARMYKNIFE_MEMORY_ALLOCATION_START_PADDING or
 * ARMYKNIFE_MEMORY_ALLOCATION_END_PADDING is non-zero.
 */
#ifndef ARMYKNIFE_MEMORY_ALLOCATION_HASHTABLE_SIZE
#define ARMYKNIFE_MEMORY_ALLOCATION_HASHTABLE_SIZE 16
#endif

/**
 * @debug_compiliation_option ARMYKNIFE_MEMORY_ALLOCATION_MAXIMUM_AMOUNT
 *
 * This is just a fail-safe to catch extremely dumb allocation amounts
 * (at least as of 2023). If you know you will have a fixed size
 * amount of memory, you could set this lower and potentially get a
 * slightly nicer error message.
 */
#ifndef ARMYKNIFE_MEMORY_ALLOCATION_MAXIMUM_AMOUNT
#define ARMYKNIFE_MEMORY_ALLOCATION_MAXIMUM_AMOUNT (1L << 48)
#endif

#define START_PADDING_BYTE (170 & 0xff)
#define END_PADDING_BYTE ((~START_PADDING_BYTE) & 0xff)

struct memory_hashtable_bucket_S {
  // malloc will never allocated at address 0 so if this field is
  // zero, then this spot in the hashtable is occupied.
  uint64_t malloc_address;
  uint64_t malloc_size;
  char* allocation_filename;
  uint64_t allocation_line_number;
};

typedef struct memory_hashtable_bucket_S memory_hashtable_bucket_t;

memory_hashtable_bucket_t memory_ht[ARMYKNIFE_MEMORY_ALLOCATION_HASHTABLE_SIZE];

void check_start_padding(uint8_t* address) {
  for (int i = 0; i < ARMYKNIFE_MEMORY_ALLOCATION_START_PADDING; i++) {
    if (address[i] != START_PADDING_BYTE) {
      fatal_error(ERROR_MEMORY_START_PADDING_ERROR);
    }
  }
}

void check_end_padding(uint8_t* address, char* filename, uint64_t line) {
  for (int i = 0; i < ARMYKNIFE_MEMORY_ALLOCATION_END_PADDING; i++) {
    if (address[i] != END_PADDING_BYTE) {
      fprintf(stderr,
              "FATAL: someone clobbered past an allocation %lu. (allocated "
              "here: %s:%lu)\n",
              cast(uint64_t, address), filename, line);
      fatal_error(ERROR_MEMORY_END_PADDING_ERROR);
    }
  }
}

void check_memory_hashtable_padding() {
  for (int i = 0; i < ARMYKNIFE_MEMORY_ALLOCATION_HASHTABLE_SIZE; i++) {
    if (memory_ht[i].malloc_address != 0) {
      uint64_t malloc_start_address = memory_ht[i].malloc_address;
      uint64_t malloc_size = memory_ht[i].malloc_size;
      check_start_padding(cast(uint8_t*, malloc_start_address));
      check_end_padding(
          cast(uint8_t*,
               (malloc_start_address + ARMYKNIFE_MEMORY_ALLOCATION_START_PADDING
                + malloc_size)),
          memory_ht[i].allocation_filename,
          memory_ht[i].allocation_line_number);
    }
  }
}

// I got this from a blog post by Daniel Lemire (who was actually
// pushing a different scheme...) A terrible hash function will sink
// our scheme but anything that isn't terrible just gets us closer to
// some ideal.
uint64_t mumurhash64_mix(uint64_t h) {
  h *= h >> 33;
  h *= 0xff51afd7ed558ccdL;
  h *= h >> 33;
  h *= 0xc4ceb9fe1a85ec53L;
  h *= h >> 33;
  return h;
}

// Start tracking padding for a given allocated address. This includes
// setting the padding to particular values and of course putting the
// address into the tracking table.
void track_padding(char* file, int line, uint8_t* address, uint64_t amount) {
  // First set the padding to predicatable values
  for (int i = 0; i < ARMYKNIFE_MEMORY_ALLOCATION_START_PADDING; i++) {
    address[i] = START_PADDING_BYTE;
  }
  uint8_t* end_padding_address
      = address + amount + ARMYKNIFE_MEMORY_ALLOCATION_START_PADDING;
  for (int i = 0; i < ARMYKNIFE_MEMORY_ALLOCATION_END_PADDING; i++) {
    end_padding_address[i] = END_PADDING_BYTE;
  }

  if (ARMYKNIFE_MEMORY_ALLOCATION_HASHTABLE_SIZE > 0) {
    // Now replace whatever entry we might already have there. This is
    // why we have more LRU semantics. We could use another signal to
    // probalistically delay updating the hashtable when the bucket is
    // already occupied but I think LRU might work well most of the
    // time. (Mostly a hunch I will admit.).
    int bucket = mumurhash64_mix(cast(uint64_t, address))
                 % ARMYKNIFE_MEMORY_ALLOCATION_HASHTABLE_SIZE;
    memory_ht[bucket].malloc_address = cast(uint64_t, address);
    memory_ht[bucket].malloc_size = amount;
    memory_ht[bucket].allocation_filename = file;
    memory_ht[bucket].allocation_line_number = line;
  }
}

void untrack_padding(uint8_t* malloc_address) {
  check_start_padding(malloc_address);
  // Unfortunately, since we don't know the size of the allocation, we
  // can't actually check the end padding! When there is enough start
  // padding (say at least 64bits), then we could potentially store
  // say 48bits worth of an allocation amount in it.
  //
  // On the other hand, we do check the end padding if it is still
  // tracked in the lossy memory hashtable.

  if (ARMYKNIFE_MEMORY_ALLOCATION_HASHTABLE_SIZE > 0) {
    // Now finally zero-out the memory hashtable.
    int bucket = mumurhash64_mix(cast(uint64_t, malloc_address))
                 % ARMYKNIFE_MEMORY_ALLOCATION_HASHTABLE_SIZE;
    memory_ht[bucket].malloc_address = 0;
    memory_ht[bucket].malloc_size = 0;
    memory_ht[bucket].allocation_filename = 0;
    memory_ht[bucket].allocation_line_number = 0;
  }
}

/**
 * @function checked_malloc
 *
 * Allocate the given amount bytes or cause a fatal error. The memory
 * is also zeroed.
 *
 * If possible, use the macros malloc_bytes or malloc_struct instead
 * for an easier to use interface. Those macros simply call
 * checked_malloc.
 */
uint8_t* checked_malloc(char* file, int line, uint64_t amount) {

  if (amount == 0 || amount > ARMYKNIFE_MEMORY_ALLOCATION_MAXIMUM_AMOUNT) {
    fatal_error(ERROR_BAD_ALLOCATION_SIZE);
  }

  if (should_log_memory_allocation()) {
    fprintf(stderr,
            "ALLOCATE %s:%d -- n_bytes=%lu already_allocated=%lu n_calls=%lu\n",
            file, line, amount, number_of_bytes_allocated,
            number_of_malloc_calls);
  }

  check_memory_hashtable_padding();

  uint64_t amount_with_padding = ARMYKNIFE_MEMORY_ALLOCATION_START_PADDING
                                 + amount
                                 + ARMYKNIFE_MEMORY_ALLOCATION_END_PADDING;
  uint8_t* result = malloc(amount_with_padding);
  if (result == NULL) {
    fatal_error_impl(file, line, ERROR_MEMORY_ALLOCATION);
  }

  if (should_log_memory_allocation()) {
    fprintf(stderr, "ALLOCATE %s:%d -- %lu -- ptr=%lu\n", file, line, amount,
            cast(unsigned long, result));
  }

  memset(result, 0, amount_with_padding);
  track_padding(file, line, result, amount);

  number_of_bytes_allocated += amount;
  number_of_malloc_calls++;

  return result + ARMYKNIFE_MEMORY_ALLOCATION_START_PADDING;
}

/**
 * @function checked_malloc_copy_of
 *
 * Allocate amount bytes and initialize it with a copy of that many
 * bytes from source.
 */
uint8_t* checked_malloc_copy_of(char* file, int line, uint8_t* source,
                                uint64_t amount) {
  uint8_t* result = checked_malloc(file, line, amount);
  memcpy(result, source, amount);
  return result;
}

/**
 * @function checked_free
 *
 * Allocate amount bytes or cause a fatal error. The memory is also
 * zeroed.
 *
 * If possible, use the macros malloc_bytes or malloc_struct instead
 * for an easier to use interface. Those macros simply call
 * checked_malloc.
 */
void checked_free(char* file, int line, void* pointer) {
  if (should_log_memory_allocation()) {
    fprintf(stderr, "DEALLOCATE %s:%d -- %lu\n", file, line,
            cast(uint64_t, pointer));
  }
  if (pointer == NULL) {
    fatal_error_impl(file, line, ERROR_MEMORY_FREE_NULL);
  }

  // Check all of the padding we know about
  check_memory_hashtable_padding();

  uint8_t* malloc_pointer
      = cast(uint8_t*, pointer) - ARMYKNIFE_MEMORY_ALLOCATION_START_PADDING;

  // Check this entries padding (in case it got lossed from the global
  // hashtable), and also remove it from the hashtable if it was
  // found.
  untrack_padding(malloc_pointer);
  number_of_free_calls++;
  free(malloc_pointer);
}
