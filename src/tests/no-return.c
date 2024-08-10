#include <stdlib.h>

_Noreturn void doexit(void) {
  exit(0);
}

int main(int argc, char** argv) {
  doexit();
}
