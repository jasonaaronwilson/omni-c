#include <stdio.h>

int main() {
    int num = 5;
    int result = 1;
    int i = 1;

    while (i <= num) {
        result *= i;
        i++;
    }

    printf("Factorial of %d = %d\n", num, result);

    return 0;
}
