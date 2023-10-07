# try, catch, throw

try and catch are used to execute code knowing that some exceptions
that occur in the block can be caught.

throw is used to actually throw an exceptions.

```
void example_try_catch() {
  try {
    example_throw();
  } catch (exception ex) {
    stdout.println("caught an exception: " + ex.to_string());
  }
}

void example_throw() {
     throw exception("this is a test exception");
}
```

There is no finally clause as in other languages. Instead use defer to
defer a statement until the try block is complete since deferred
statements are run even if an exception occurs (they are run before
the catch code is run.
