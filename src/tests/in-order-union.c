struct a {
  int foo;
};

struct b {
  struct a bar;
};

union c {
  struct a aa;
  struct b bb;
};

// main
