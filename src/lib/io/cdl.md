# CDL: Clear Data Language

CDL is essentially simplified JSON. It's very easy to generate nearly
canonical "pretty-printed" form and is not harder to parse than any
other hierarchical textual data format.

## Examples

*CDL*
```
{
    tag = PARSE_NODE_RETURN_STATEMENT
    expression = {
        tag = PARSE_NODE_LITERAL
	source_position = 10023
	ratio = 3.14
        tokens = [
            "1"
	    ABC
        ]
    }
}
```

You've probably seen JSON before, I think it looks "busier" and "less
open" than *CDL* (and not just because of indentation...):

*JSON*
```
{
  "tag": "PARSE_NODE_RETURN_STATEMENT",
  "expression": {
    "tag": "PARSE_NODE_LITERAL",
    "source_position": 10023,
    "ratio": 3.14,
    "tokens": [
      "1",
      "ABC"
    ]
  }
}
```

I didn't actually convert this into JSON by hand, I let "Gemini
Advanced" (circa 2024-11-16) do it for me. It also explained how it
changed it into JSON:

```
Here's what I changed to make your CDL into valid JSON:

Replaced '=' with ':': JSON uses colons to separate keys and values.

Added quotes around strings: JSON requires string values to be
enclosed in double quotes. I've added quotes around
"PARSE_NODE_RETURN_STATEMENT", "PARSE_NODE_LITERAL", "1", and "ABC".

Removed the trailing comma after "1" in the tokens array: While some
JSON parsers are lenient, the standard does not allow trailing commas
in arrays or objects.
```

It's explanation is kind of wrong (there was no commas to remove, CDL
doesn't have commas) but this demonstrates that that this format is
certainly not going to be a buzzkill if you want to feed it into an
LLM that hasn't seen it before (especially if one were to prompt the
LLM a bit with a few examples).

Just for fun, let's have Gemini Advanced convert to s-expression
syntax:

```
(PARSE_NODE_RETURN_STATEMENT
  (PARSE_NODE_LITERAL
    (source_position 10023)
    (ratio 3.14)
    (tokens ("1" "ABC"))))
```

Gemni kind of cheated by removing tag which makes it look smaller
though I would have done this the same way for my particular use case!

I'm not aware of a high quality pretty-printer for s-expressions that
would actually format it this way that's available in non "Lispy"
languages. Using sort of similar rules as the CDL printer here uses,
this might not be that hard to achieve:

```
(PARSE_NODE_RETURN_STATEMENT
  (PARSE_NODE_LITERAL
    (source_position 10023)
    (ratio 3.14)
    (tokens (
      "1"
      "ABC"))))
```

(If you can produce Guile compatible s-expressions, then you could
invoke Guile to pretty print it.)

The other format to consider would be Google's protocol buffer text
representation.

## Analysis

There really are no winners here.

JSON looks the worse to my eyes but is the most universal format of
them all (even though it came far after s-expressions). Most projects
have some connection to the web and JSON works well with modern
browsers via JSON.parse().

If your language already has great s-expression support (especially
quasiquote), you probably want to stick with that. I will point out
that s-expressions aren't as standardized as say JSON since Scheme,
CommonLisp, and Clojure have some differences.

Our CDL printer will eventually be modified to output either JSON or
CDL based on a printer construction option. For reading, we will
probably provide an "auto" mode, JSON mode, or CDL mode.

## Conclusion

The world doesn't need another format but I made one anyway. If you
are going to be printing out lots of data-structures and don't want to
use a library that gets this right, then you may want to try out CDL.
