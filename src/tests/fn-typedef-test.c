#include <stdio.h>

// TODO(jawilson): this shouldn't be necessary. Factor out of
// c-armyknife-lib

#define fn_t(return_type, ...) typeof(return_type(*)(__VA_ARGS__))

typedef callback_fn_t = fn_t(int, char* str, int);

int my_callback_fn(char* str, int x) {
  return x + str[0];
}

int main(int argc, char** argv) {
  callback_fn_t callback = &my_callback_fn;
  int callback_result = (*callback)("a", 10);
  if (callback_result != 107) {
    printf("ERROR %d != 107\n", callback_result);
    return 1;
  }
  return 0;
}

