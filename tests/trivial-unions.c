union int_and_char_pointer {
  int a;
  char* b;
};

// This isn't really that trivial. In this case we are allowed to not
// name the field of the union and it kind of bubbles up to the parent
// structure (or presumably parent union...)
typedef struct {
  union {
    double dbl;
    void* ptr;
  };
  long foo;
} interior_anonymous_union_t;

