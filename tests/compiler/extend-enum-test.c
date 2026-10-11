
typedef animal_t = enum {
    ANIMAL_UNKNOWN = 0,
    ANIMAL_DOG,
    ANIMAL_CAT,
    ANIMAL_HAMSTER
};

extend_enum animal_t {
  ANIMAL_GOLDFISH,
  ANIMAL_SALMON
};

extend_enum animal_t {
  ANIMAL_LOBSTER,
  ANIMAL_CARB
};

void test_to_string(void) {
  if (!(string_equal("ANIMAL_DOG", animal_to_string(ANIMAL_DOG)))) {
    test_fail("expected ANIMAL_DOG");
  }
  if (!(string_equal("ANIMAL_GOLDFISH", animal_to_string(ANIMAL_GOLDFISH)))) {
    test_fail("expected ANIMAL_GOLDFISH");
  }
  if (!(string_equal("ANIMAL_LOBSTER", animal_to_string(ANIMAL_LOBSTER)))) {
    test_fail("expected ANIMAL_LOBSTER");
  }
}
