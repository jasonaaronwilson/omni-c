#include <stdio.h>

// Forward declaration with incomplete types
extern int matrix[][3][4];

int main() {
  printf("%p\n", &matrix);
  return 0;
}

// Actual definition with complete dimensions
int matrix[2][3][4] = {
    { {1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12} },
    { {13, 14, 15, 16}, {17, 18, 19, 20}, {21, 22, 23, 24} }
};
