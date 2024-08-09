#include <stdio.h>

// Declare a global anonymous structure
struct {
    int count;
    double value;
} globalData;

int main() {
    // Initialize the global structure
    globalData.count = 5;
    globalData.value = 3.14159;

    // Access and print the values
    printf("Count: %d, Value: %lf\n", globalData.count, globalData.value);

    return 0;
}
