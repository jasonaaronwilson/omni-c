#include <stdio.h>

long numbers[] = { 1, 2, 3, 4, 5, 6 };

int main(int argc, char** argv) {
  int num_elements = sizeof(numbers) / sizeof(numbers[0]);
  long sum = 0;
  for (int i = 0; i < num_elements; i++) {
    long num = numbers[i];
    if ((num & 1) == 0) {
      sum += num;
    }
  }
  printf("%ld \n", sum);
  return 0;
}

