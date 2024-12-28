//
// Test some operations on uint64_t
//

#define C_ARMYKNIFE_LIB_IMPL
#include "../c-armyknife-lib.h"

void test_uint64_highest_bit_set() {
  test_assert(0 == uint64_highest_bit_set(1ULL << 0));
  for (int i = 0; i < 64; i++) {
    test_assert(i == uint64_highest_bit_set(((uint64_t) 1) << i));
  }
}

void test_uint64_highest_bit_set_random() {
  random_state_t state = random_state_for_test();
  for (int i = 0; i < 64; i++) {
    uint64_t next = random_next(&state);
    // Mask off all but the bottom "i" bits. About 50% of the time
    // this should have the "i"th bit set. All bits above i should be
    // zero and randomy stuff will appear below. The weakness of this
    // random test is that we still have about 50%50% probability for
    // each lower bit. Some kind of attenuate function could choose
    // "freqencies" (a power of two) and randomly "attenuate" those.
    next &= ((1ULL << i) - 1);
    // Now mix that in with the
    uint64_t messy_number = (((uint64_t) 1) << i) | next;
    if (i != (uint64_highest_bit_set(messy_number))) {
      test_fail("loop i=%d next=%lu", i, next);
    }
  }
}

void test_min_max() {
  test_assert(min(100, 50) == 50);
  test_assert(max(100, 50) == 100);

  // Not technically uint64_t...
  test_assert(min(100, -50) == -50);
  test_assert(max(-100, -50) == -50);
}

int main(int argc, char** argv) {
  open_arena_for_test();
  test_uint64_highest_bit_set();
  test_uint64_highest_bit_set_random();
  test_min_max();
  close_arena_for_test();
  exit(0);
}
