# Implementation Overview

Omni C is implemented by "transpiling" Omni C source code to standard
C source code.

Conceptually this should be somewhat easy. A great deal of the syntax
of Omni C and C are identifical and the semantics of most constructs
is the same.

Additionally, the C pre-processor, while not the must powerful macro
language, is a potential asset, for example, we could implement our
type names such as "uint32" as a macro that simply replaces it with
"uint32_t" (though typedef's would probably be more suitable for
this example).

It should be obvious that the C pre-processor can only reasonably be
expected to go so far. When the original C++ compiler was created this
was not accomplished solely with macros. Indeed if the C pre-processor
did have sufficiently powerful macro language, perhaps C would have
evolved more quickly as popular macro tweaks to the language might
have been standardized.

The reality is that there are several important things we want to do,
such as function overloading, which are not practical to achieve with
the C pre-processor.

## Phases

1. parsing (in addition to parsing Omni C, we also parse C files to
   obtain definitions, etc.)
2. OCNode creation
3. Alpha Renaming
4. type checking - we assigned a type to every expression
5. closure conversion
6. linearization and output

### Parsing

We are doing to try using the tree sitter framework for our parsing
needs. It provides what appears to be high-speed parsers for a large
variety of programming languages, most notably C, and makes these very
easy to use from C as well.

### OCNode Creation

OCNode's are tied to the source input files that make them difficult
to actually edit (especially moving to other files, etc.). Instead we
wrap the important data from the nodes into our own node type called
OCNodes. While this isolates us from the choice of parsing technology
to some degree, that isn't actually a strong reason for this step.

Once we have our own node data-struture, we can augment these nodes
with additional initially empty fields, such as "expression_type" to
hold the outcome of various analysis.

### Alpha Renaming

Alpha renaming removes shadowing of variables by other variables. To
make it easier to implement defer statements and perhaps simplify
other transformations, a perform alpha renaming earlier in the
compilation process and make sure any new variables we add will not
result in any new shadowing.

we can either prefix all local variables with something or suffix all
local variables with something. Traditionally a suffix is used and
often times a simple global counter. In order to enable more
determistic output if we ever translate using multiple threads,
instead we will use the line and column of the identifer when it was
defined such as _14_32. (If we suffix all of the variables we create
with a per (top-level) function count, like _3_, then these won't
conflict with line and column (just _3 is insufficent actually).

### Type Checking

Type Checking walks the nodes in the same order as they would be
evaluate or executed and annotates all expressions. Generally to
assign a type to a node, we first need to annotate all of the child
nodes and then that information can be used to assigned to type to the
node.

During type checking we are resolve references to overloaded functions
(and report an error if such a function isn't found).

#### Closure Conversion

The goal here is to move the code of a closure up to the namespace
level. This means converting used of captured variables (currently
assignment to capture variables is illegal) to data that is passed
into the closure.

So step one is to create a structure to represent the captured
variables. Using a structure raw than say raw data is beneficial
because C compilers understand structures quite well and of course gdb
or lldb will be much more helpful.

Now we need to rewrite the body and arguments of the closure. First we
add a new first argument which is a pointer to an instance of the
structure we defined above. Next every reference to a captured
variable is replaced by an expression to reads the value from the
structure instead.

Lastly we can add the rewritten function, with a shiny new name to the
namespace level like an ordinary function which is now is. Where the
closure was being created, we need to allocate the closure (on the
heap) and set each of capture variables to something) and fill in the
address of the closure into the first slot so that the closure can be
called.

The newly created name-space level functions may itself contain
closures of it's own. It actually turns out to be easier to first
rewrite any closures inside of a closure first, and only then rewrite
the current closure.

There is one last thing that must be done to support closures. When a
closure is called, we need to adjust the call site itself to pass the
closure as the first argument to the function as well as extracting
the function address. We think it might be cleanest to have a built in
function called  call which does this (it can also be used in the
chaining syntax - my_closure.call(arguments) or call(my_closure,
arguments...)).

## Defer Conversion

We need to place code at the exit sites of any block.

## Exception & Call Converion



## Lineration and Output

Unfortunately linearization is not as simple as merely writing
everything back out as C code. One problem is let which allows
statements where normally only expression are allowed in C. Removing
this feature from the language seems like an option until you see
things like closure conversion where let is put to good use.

Even without let, we still need to handle exceptions. Essentially we 

