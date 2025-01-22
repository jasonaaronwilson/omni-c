#include <stdio.h>

// Declare a global array of anonymous structures
struct {
    int id;
} globalDataArray[2];

int main() {
    // Initialize the elements of the array
    globalDataArray[0].id = 101;
    globalDataArray[1].id = 102;
    return 0;
}
