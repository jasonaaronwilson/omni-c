#include <stdio.h>

char* strings[] = { "A", "B", "C", };

int main(int argc, char** argv) {
  int num_elements = sizeof(strings) / sizeof(strings[0]);
  for (int i = 0; i < num_elements; i++) {
    printf("%s", strings[i]);
  }
  printf("\n");
  return 0;
}

