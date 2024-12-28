# @file value-alist.c

An association list (a type of map) from value_t to value_t.
 
## @macro value_alist_foreach

Allows iteration over the keys and values in a string association
list.
 
## @function value_alist_delete

Delete the key and associated value from the given association
list. Neither the key nor the value associated are themselves
freed.
 
## @function value_alist_find

Find the value associate with the given key. Use is_ok() or
is_not_ok() to see if the value is valid (i.e., if the key was
actually found).
 
## @function value_alist_insert

Insert a new key and value into an assocation list.
 
## @function value_alist_length

Determine the length of an alist.

The alist argument MAY be null.
 
