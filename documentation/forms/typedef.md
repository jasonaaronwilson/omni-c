# typedef

The standard C typedef is somewhat confusing so we have a different
syntax (and semantics):

```
typedef int = int64
typedef char_ptr = char*
typedef append_fn = function((void), buffer*, reference)
```
