# while

A while loop is begun with the while keyword, then has a test
expression in parens and finally have a block or single statement
after it.

```
while (true) {
  stdout.println("I'm in an infinite loop!");
}

while (true)
  stdout.println("I'm in an infinite loop!");
```

The keyword break can be used to terminate the loop. On termination,
and statements deferred using defer are automatically executed.

The keyword continue can be used to cleanly jump to the top of the
loop. deferred statements are not run since the loop has not yet
terminated.
