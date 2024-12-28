# @file random.c

An implementation of "xorshiro128**", a pseudo-random number
generator.

This is not a high quality source of entropy and is intended for
use in tests or other places where determinism is important
(including across platforms and C library implementations).

See: https://prng.di.unimi.it/xoroshiro128starstar.c
 
## @function random_next

Return a random uint64_t from the current state (and update the
state).
 
## @function random_next_uint64_below

Return a random `uint64_t` that is below some maximum. As much as
the underlying random number generartor allows, this should be
uniform.
 
## @function random_state

Return a shared random state. If the random state has not been
initialized yet, it is initialized based off the timestamp.
 
## @function random_state_for_test

Return a consistent initial random state for tests.
 
