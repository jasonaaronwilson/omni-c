# Variable Assignment

A variable can be assigned to new value using an assignment expression
like "=".

```
uint64 example(uint64 n) {
  n = n + (n - 1);
  return n;
}
```

The above can be simplifed in several way but we will use the +=
operator.

```
uint64 example(uint64 n) {
  n += n - 1;
  return n;
}
```

