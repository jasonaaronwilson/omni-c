# Closures

Lexical closures are also simply called "functions" since there type
always begins with the word "function".

There are two ways to create a closure.

The first is using the fn keyword:

```
void example() {
  auto square = fn(uint64 x) { return x * x };
  stdout.println(interpolate("5 squared is ${square(5)}"));
}
```

Note that the return type is automatically infered since the compiler
must determine the actual type of the function in order to do type
checking.

A syntactic shortcut allows you to simply this function somewhat:

```
void example() {
  fn square(uint64 x) { return x * x };
  stdout.println(interpolate("5 squared is ${square(5)}"));
}
```

A thunk is a term used to describe (usually anonymous) closures that
take no arguments. Another shortcut is available to make a closure is
simply:

```
void example() {
  auto thunk = {
    stdout.println("Hello World!");
  };
}
```

In the above example, the block looks like the blocks for special
forms like if and while though the behavior is quite different. With
the if and while and other statements, the syntactic block is
evaluated per its evaluation rules whereas the closure body is not
evaluated at all but rather a closure created that can then later be
called to execute the statementy in the closure's body.

Another way to create a closure is the & prefix operator applied to
the name of a function from the enclosing namespace:

```
uint64 square(uin64 x) {
  return x * x;
}

void example() {
  auto square_closure = &square;
  stdout.println(square_closure(5));
}
```
