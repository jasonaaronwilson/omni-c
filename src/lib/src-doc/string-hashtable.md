# @file string-hashtable.c

A very thread-unsafe hash map of C style zero terminated byte
"strings" to a value_t. This is an oqaque reference around a
value_hashtable.
 
## @macro string_ht_foreach

Allows traversing all elements of a hashtable in an unspecified
order.
 
## @function make_string_hashtable

Create a hashtable with the given number of buckets.

The minimum number of buckets is currently 2 to make it less likely
we run into some resize loop depending on the values of
ARMYKNIFE_HT_LOAD_FACTOR and AK_HT_UPSCALE_MULTIPLIER).
 
## @function string_ht_delete

Delete an association from the hashtable. It is not an error to
delete a key that doesn't exist in the hashtable.
 
## @function string_ht_find

Find an association in the hashtable.
 
## @function string_ht_insert

Insert an association into the hashtable.
 
## @function string_ht_num_entries

Returns the number of entries in the hashtable.
 
