# let

let expressions allow statements to be run in the middle of an
expression. let expression can also bind variables to expressions. The
type of the variables is automatically inferred without using the auto
keyword.

```
void example() {
  let ((name "Moby") (age 99)) {
    stdout.println("Name is: " + name);
    stdout.println("Age is: " + age);
  };
}

void example2() {
  x = 12 + let ((y 5) (z 10)) { y + z };
}
```

let can also be used to create loops by naming the block and using
that name like a function call.

```
void example(linked_list list) {
  let ((list list)) loop: {
    if (!null?(list)) {
      loop(cdr(list));
    }
  }
}
```
