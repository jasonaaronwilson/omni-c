#include <stdio.h>
#include <string.h>

#define MAX_PEOPLE 5

struct person {
    char* first_name;
    char* last_name;
};

int main() {
    struct person people[MAX_PEOPLE] = {
        {"Alice", "Smith"},
        {"Bob", "Johnson"},
        {"Charlie", "Brown"},
        {"Alice", "Williams"},
        {"David", "Lee"}
    };

    for (int i = 0; i < MAX_PEOPLE; i++) {
        if (strcmp(people[i].first_name, "Alice") == 0) {
            printf("Found: %s %s\n", people[i].first_name, people[i].last_name);
        }
    }

    return 0;
}
