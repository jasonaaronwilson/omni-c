//
// Test some operations on byte-arrays
//

#include <stdlib.h>

#define C_ARMYKNIFE_LIB_IMPL
#include "../c-armyknife-lib.h"

void test_random_next() {
  random_state_t state = random_state_for_test();
  for (int i = 0; i < 10; i++) {
    uint64_t next = random_next(&state);
    fprintf(stdout, "i=%d random=%lu\n", i, next);
  }
}

void test_random_next_u64_bellow() {
  int counts[100] = {0};
  random_state_t state = random_state_for_test();
  for (int i = 0; i < 1000000; i++) {
    uint64_t next = random_next_uint64_below(&state, 100);
    test_assert(next < 100);
    counts[next]++;
  }
  for (int i = 0; i < 100; i += 5) {
    fprintf(stderr, "%d -- %d\n", i, counts[i]);
  }
}

int main(int argc, char** argv) {
  test_random_next();
  test_random_next_u64_bellow();
  exit(0);
}
