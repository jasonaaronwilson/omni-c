# Enumerations

Enumerations gives human readable names to zero or more constants and
also define a type so that these enumeration values can be passed
around to other functions. The enumeration values can of course be
used with switch.

```
enum color {
  unknown, white, gray, black, red, blue, yellow, green, orange
}

void example() {
  color text_color = color.white;
  switch (text_color) {
    case white, gray, black:
      stdout.println("greyscale: " + text_color.to_string());
    default:
      stdout.println("color" + text_color.to_string() + " " 
        + text_color.to_code().to_string());
  }
}
```

Note that we convert an enum value to an integer via to_code()
(essentially just a compile time case so zero cost) or to a string via
to_string(). Both of the functions are automatically generated.
