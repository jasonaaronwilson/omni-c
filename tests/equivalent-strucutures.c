/*

This doesn't work. Maybe it doesn't matter since we now have typedef X = Y;

struct a_t {
  int foo;
};
*/

typedef struct b_S {
  int foo;
} b_t;

typedef c_t = struct {
  int foo;
};

int main(int argc, char** argv) {
  // a_t a0 = compound_literal(a_t, {0});
  b_t b0 = compound_literal(b_t, {0});
  c_t c0 = compound_literal(c_t, {0});

  return b0.foo + c0.foo /* + a0.foo */;
}
