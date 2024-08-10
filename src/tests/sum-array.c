#include <stdio.h>

long numbers[] = { 1, 2, 3 };

int main(int argc, char** argv) {
  int num_elements = sizeof(numbers) / sizeof(numbers[0]);
  long sum = 0;
  for (int i = 0; i < 3; i++) {
    sum += numbers[i];
  }
  printf("%d \n", sum);
  return 0;
}

