# @file string-tree.c

This is a balanced binary tree to associate a string to a value
using an underlying value-tree.c.
 
## @macro string_tree_foreach

Perform an inorder traversal of a string-tree.

key_var is created in a new block scope with type char*.

value_var is created in a new block scope with type value_t and you
will probably want to use something like ".ptr" or ".u64" on the
value to obtain the actual value.

statements should be a normal C block, aka, something like:
```
{
  statement1();
  statement2();
}
```

Unforunately it is not possible to use "break" or "continue" with
this style of loop (and worse, there will be no compilation error
or warning if you accidentally do that...)
 
## @function string_tree_delete

Delete the association of key (if it exists in the tree). It is not
an error to delete a key that isn't present in the table.
 
## @function string_tree_find

Find the value associate with the key in the tree.
 
## @function string_tree_insert

Insert an association of key and a value (or update the current
value stored in the tree).
 
