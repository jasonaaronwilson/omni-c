union c {
  struct a aa;
  struct b bb;
};

struct a {
  int foo;
};

struct b {
  struct a bar;
};
