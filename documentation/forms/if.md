# if

If is used to selectively execute code depending on weather the
condition expression is true or false (or several other values that
can be converted to a boolean such as integers).

```
void example(boolean b) {
  if (b) {
    stdout.println("true);
  }
}
```

If statement may also end with an else clause:

```
void example(boolean b) {
  if (b) {
    stdout.println("true);
  } else {
    stdout.println("false);
  }
}
```

Or it can end with another if statement:

```
void example(uint32 x) {
  if (x == 0) {
    stdout.println("== 0");
  } if (x < 256) {
    stdout.println("< 256");
  } else {
    stdout.println("> 256");
  }
}
```

Note that the braces are not strictly required when only a single
simple statement is to be executed:

```
void example(uint32 x) {
  if (x == 0)
    stdout.println("== 0");
  else if (x == 1)
    stdout.println("== 1");
  else
    stdout.println("> 1");
}
```

Non simple statements include variable declarations, while/do-while
loops, other if statements, switch statements, etc. Simple statements
are thus return, continue, break, assignments/increments/decrements,
or calls.
