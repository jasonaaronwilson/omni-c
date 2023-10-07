# for loops

For loops can be used in both the traditional C style or "for each"
style.

```
void example() {
  for (int32 i = 0; i < 100; i++) {
    stdout.println(i.to_string());
  }
}

The foreach style will repeatedly call an iterator until that iterator
is exhausted.

```
void example(iterator(int32) iter) {
  for (auto value : iter) {
    stdout.println(value.to_string());
  }
}
```
