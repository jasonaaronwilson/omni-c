# @file value-tree.c

This is a balanced binary tree to associate a value to another
value.

Generally an alist is prefered for small "maps", and hashtables are
prefered for large maps, but value_tree is the easiest way to get
sorted results (which is most important for reproducibility).

Currently we are using "AA" trees (see
https://en.wikipedia.org/wiki/AA_tree) since it has simpler code
than many other balanced trees (like red-block trees) and the
Wikipedia article and paper spell out *most* of the non-trivial
details.
 
## @macro value_tree_foreach

Perform an inorder traversal of a value-tree.

key_var is created in a new block scope with type value_t.

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
 
## @function value_tree_delete

Delete the association of key (if it exists in the tree). It is not
an error to delete a key that isn't present in the table.
 
## @function value_tree_find

Find the value associate with the key in the tree.
 
## @function value_tree_insert

Insert an association of key and a value (or update the current
value stored in the tree).
 
