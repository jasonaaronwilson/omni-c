# Condition Expressions

Conditionals are similar to if in that behavior is dependent on a
condition expression (sometimes called a test) but conditionals can
appear as an expression and they result in one of two values (which
must have the same) type being selected.

```
void example(boolean b) {
  uint32 x = (cond (b 10) (else 5));
}

void example_1(boolean b, boolean c) {
  uint32 x = (cond (b 5) (c 10) (else 20));
}
```
