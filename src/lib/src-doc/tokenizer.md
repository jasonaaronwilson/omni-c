# @file tokenizer.c

 
## @function buffer_tokenize

Tokenize the current contents of a buffer into a value_array_t of
(non-empty) strings. The input buffer should contain a valid UTF-8
encoded string.

NUL bytes inside the buffer are automatically treated as an
additional delimiter.
 
## @function string_tokenize

Tokenize a string into a a value_array_t of (non-empty) strings.

Delimiters terminate the current token and are thrown away. The
delimiters string is treated as a sequence of delimiter characters,
it does not mean a delimiter can be multiple characters.
 
## @function tokenize_memory_range

Tokenize a memory range into a value_array_t of (non-empty) strings. The
range should contain a valid UTF-8 encoded string.

NUL bytes are automatically treated as an additional delimiter.
 
