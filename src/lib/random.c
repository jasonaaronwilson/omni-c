#line 2 "random.c"

/**
 * @file random.c
 *
 * An implementation of "xorshiro128**", a pseudo-random number
 * generator.
 *
 * This is not a high quality source of entropy and is intended for
 * use in tests or other places where determinism is important
 * (including across platforms and C library implementations).
 *
 * See: https://prng.di.unimi.it/xoroshiro128starstar.c
 */

#ifndef _RANDOM_H_
#define _RANDOM_H_

struct random_state_S {
  uint64_t a;
  uint64_t b;
};

typedef struct random_state_S random_state_t;

extern random_state_t random_state_for_test(void);
extern uint64_t random_next_uint64(random_state_t* state);
extern uint64_t random_next_uint64_below(random_state_t* state,
                                         uint64_t maximum);

#endif /* _RANDOM_H_ */

#include <time.h>

/**
 * @function random_state_for_test
 *
 * Return a consistent initial random state for tests.
 */
random_state_t random_state_for_test(void) {
  return compound_literal(random_state_t,
                          {.a = 0x1E1D43C2CA44B1F5, .b = 0x4FDD267452CEDBAC});
}

static random_state_t shared_random_state = {0};

/**
 * @function random_state
 *
 * Return a shared random state. If the random state has not been
 * initialized yet, it is initialized based off the timestamp.
 */
random_state_t* random_state(void) {
  if (shared_random_state.a == 0) {
    shared_random_state.a = 0x1E1D43C2CA44B1F5 ^ cast(uint64_t, time(NULL));
    shared_random_state.b = 0x4FDD267452CEDBAC ^ cast(uint64_t, time(NULL));
  }

  return &shared_random_state;
}


static inline uint64_t rotl(uint64_t x, int k) {
  return (x << k) | (x >> (64 - k));
}

/**
 * @function random_next
 *
 * Return a random uint64_t from the current state (and update the
 * state).
 */
uint64_t random_next(random_state_t* state) {
  uint64_t s0 = state->a;
  uint64_t s1 = state->b;
  uint64_t result = rotl(s0 * 5, 7) * 9;
  s1 ^= s0;
  state->a = rotl(s0, 24) ^ s1 ^ (s1 << 16); // a, b
  state->b = rotl(s1, 37);                   // c

  return result;
}

/**
 * @function random_next_uint64_below
 *
 * Return a random `uint64_t` that is below some maximum. As much as
 * the underlying random number generartor allows, this should be
 * uniform.
 */
uint64_t random_next_uint64_below(random_state_t* state, uint64_t maximum) {
  if (maximum == 0) {
    fatal_error(ERROR_ILLEGAL_ARGUMENT);
  }
#if 1
  // This is simpler and works well in practice (it seems).
  return random_next(state) % maximum;
#else
  // This version in theory should be a bit more fair than modulous
  // but I can't really detect a difference.
  int mask = (1ULL << (uint64_highest_bit_set(maximum) + 1)) - 1;
  while (1) {
    uint64_t n = random_next(state);
    n &= mask;
    if (n < maximum) {
      return n;
    }
  }
#endif /* 0 */
}
