# switch

A switch attempts to effeciently dispatch to a block of code based on
the value of an expression. switch is supported for integers, strings,
booleans, and enumeration constants.

```
void example() {
  uint64 number = 100;
  switch (number) {
    case 0:
      stdout.println("zero");
    case 1, 2, 3, 4, 5, 6, 7, 8, 9;
      stdout.println("single digit");
    default:
      stdout.println("> 10");
  }
}
