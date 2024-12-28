# @file value-array.c

This file contains a growable array of "values".

Certain algorithms require that growth occurs based on the current
capacity of an array, not a fixed amount. For now we simply double
the current capacity when more space in the backing array is
required though we may scale this back to something more like 1.5X
for "large" arrays to save space.
 
## @typedef value_array_t

A growable array of 64bit "values" (so integers, doubles, and
pointers).
 
## @function make_value_array

Make a value array with the given initial capacity.

An initial capacity of zero is automatically converted to a
capacity of at least 1 or more since this makes the "growth" code a
bit simpler.

When the array runs out of capacity because of calls to add, push,
etc., then the backing array is automatically increased *based on
the current capacity* which generally leads to better big-O
properties.

If the initial or later increases in capacity are not fulfillable,
then a fatal_error occurs since these are generally not recoverable
anyways.
 
## @function value_array_add

Add an element to the end of an array. If more space is required
then the backing array is automatically resized. This resizing
means that a fatal_error() may occur if malloc() can not satisfy the
allocation request.
 
## @function value_array_delete_at

Deletes the element at the given position (and return it so that it
can potentially be freed by the caller).

If the position doesn't point to a valid element then
fatal_error(ERROR_ACCESS_OUT_OF_BOUNDS) is called.
 
## @function value_array_get

Get the value stored at index `index`. If the index is outside of
the range of valid elements, then a fatal_error is signaled.
 
## @function value_array_insert_at

Insert an element into some existing position in the array (or at
the end if position == the current array length).

This operation is not efficient for large arrays as potentially the
entire array must be moved to new locations (there are
data-structures like trees that can make this more efficient though
such a data-structure isn't in this library yet).

If the position is > than the length length, then
fatal_error(ERROR_ACCESS_OUT_OF_BOUNDS) is called.

If memory allocation is required and malloc() fails, then
fatal_error(ERROR_MEMORY_ALLOCATION) is called.
 
## @function value_array_pop

Returns the last element of the array (typically added via push)
and modifies the length of the array so that the value isn't
accessible any longer. (We also "zero out" the element in case you
are using a conservative garbage collector.)

If the array is currently empty, then
`fatal_error(ERROR_ACCESS_OUT_OF_BOUNDS)` is called.
 
## @function value_array_push

This is a synonym for value_array_add which serves to make it more
obvious that the array is actually being used like a stack.
 
## @function value_array_replace

Replace the value at a given `index`. If the index is outside of
the range of valid elements, then a `fatal_error` is signaled.
 
