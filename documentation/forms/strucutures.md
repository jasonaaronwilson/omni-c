# Structures

Structures are for absracting data as functions are for abstracting
code. Strucutures have named fields (also called members) that can be
read and written by code using the "." operator.

```
struct person {
  string first_name;
  array(string)* middle_names;
  string last_name;
  uint8 age;
}
```

Eventually strucutures will support generic types.

```
struct linked_list(T) {
  T head;
  linked_list* next;
}
```

T is often a pointer and takes up say 64bits but T could be another
"inlined" structure that takes up an aritrary number of bytes. This
means that when the generic structure is instantiate with a concrete
type that we need to reserve enough room for the element (plus align
the field to an alignment suitable for the field's type).

Because of changing sizes and alignments, accessing generic fields it
a bit tricky. The layout that the compiler uses for fields is
automatically optimized by layouting out larger fields before smaller
fields reducing the number of padding bytes needed. (This reordering
can be turned off with the attribute no_reorder_fields attribute.)
Since we don't know the size or alignment of a generic field, we move
those logically to the end of the structure and include one 32 bit
number per field to describe it's offset from the beginning of the
structure and one field per structure to describe the overall size of
all of the structure itself.

Here is an example of how re-ordering works without the precense of
generic fields:

```
struct foo {                        struct omni_c__foo {
  uint8 b1;                           uint64_t biggest_number;
  int32 bigger_number;        --->    int32_t bigger_number;
  uint16 ch;                          uint16_t ch;
  uint64 biggest_number;              uint8_t b2;
}                                   }
```

Now let's add in two generic fields:

```
struct foo(K, V) {                  struct omni_c__foo {
  uint8 b1;                           uint64_t biggest_number;
  int32 bigger_number;        --->    int32_t bigger_number;
  uint8 b2;                           int32_t omni_c_a_structure_total_size;
  uint64 biggest_number;              int32_t omni_c__key_offset;
  K key;                              int32_t omni_c__value_offset
  V value;                            uint8_t b1;
                                      uint8_t b2;
                                      // Generic fields are placed
                                      // here at runtime
}                                   }
```
