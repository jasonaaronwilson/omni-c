# auto

auto can appear in the grammar in place of a type in variable
declarations since the compiler must determine the type of the
initialization expression anyways and thus it is trivial to infer the
type and usually clear to the reader what the type is. The source
reader tool will display the actual type that is inferred if the mouse
hovers over the auto keyword.

```
string hello() {
  auto name = "Mork";
  return "Hello ".append(name).append("!");
}

uint64 hello() {
  auto four = 4;
  return four;
}
```
