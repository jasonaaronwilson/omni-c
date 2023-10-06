# Variables

Variables are named values. Variables arise from function argument
lists or local variable declarations. Variables can have their
contents replace by using the = operator (and similar assignment
expressions like +=).

Here is an example variable simply named "n" which is a function
argument:

```
uint64 fib(uint64 n) {
  if (n <= 1) {
    return n;
  }
  return fib(n - 1) + fib(n - 2);
}
```

A variable definition also creates a variable:

```
string hello() {
  string name = "Mork";
  return "Hello ".append(name).append("!");
}
```

Above name is a variable that is initialized with the string "Mork";

See the "auto" keyword which can be used to define a variable while
infering it's typed based on the initialization expression.
