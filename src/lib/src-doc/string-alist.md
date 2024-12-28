# @file string-alist.c

An association list (a type of map) from a string to a value_t.

This simply wraps value-alist.c.
 
## @macro string_alist_foreach

Allows iteration over the keys and values in a string association
list.
 
## @function alist_delete

Delete the key and associated value from the given association
list. Neither the key nor the value associated are themselves
freed.
 
## @function alist_find

Find the value associate with the given key. Use is_ok() or
is_not_ok() to see if the value is valid (i.e., if the key was
actually found).
 
## @function alist_insert

Insert a new key and value into an assocation list.
 
## @function alist_length

Determine the length of an alist.

The alist argument MAY be null.
 
