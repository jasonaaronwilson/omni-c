# do while loops

do while loops are like while loop except that the body is immediately
executed and then the test to continue looping is at the bottom of the
loop. Unless the program performs a goto, or break, the loop body is
executed at least once (while may have zero executions of the loop
body).

```
void example() {
  do {
    stdout.println("Hello World!");
  } while (true);
}
```
