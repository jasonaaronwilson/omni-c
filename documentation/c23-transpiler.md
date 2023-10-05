# The C23 Transpiler

The C23 transpiler converts source files and libraries (which are simply the
concatention of one or more sources files) into C code conforming to the C23
standard which can then be compiled with suitable C compilers such as clang
and gcc.

## High Level Overview

1. the command line is parsed to determine compiler options and all source files
   are loaded into memory
3. all of the source files are parsed. this will require at least as much space
   as the source files and libraries themselves
4. we assemble namespaces by collecting each of the namespace fragments
5. for each namespace we create the private and public top-level definitions
6. type checking, inference and annotation - every expression is given a type
7. closure conversion - (removes nested functions)
11. for each namespace, we output a C source file via an inorder treewalk
    of all functions in the namespace sorted by function name
12. for each namespace collect all of the function prototype fragments, enum fragments,
    etc.
13. typologically sort the elements for the header file. enumerations come first, structures (including environments) come next then prototypes  
14. invoke the C compiler on all of the generated source files

Most of these steps are inherently parallel so eventually the compiler will
utilize as many compute threads as possible while still producing determenistic
results.

## Function Code Generation

Essentially we have statements and expressions.

The two most interesting deviations from C that cause problems are exceptions and
(self) tail calls (when not using gcc or clang).

Self tail calls requires that we add a label as the first statement of the function and then the self call must instead of doing the call, perform a bunch of assignments, execute deferred expressions, then do a goto which obviously is not itself a C expression.

Exceptions also require a fair amount of code at the call/return site in order to short circuit.
This code is also not itself a C expression. In fact exceptions require that we do something like this for each call (source line directives have been removed):

```
struct oc_struct__oc_return_result__199 oc__tmp_675 = oc__function_functionname_signature(tmp99);
if (oc_tmp__675.exception) {
  // For each exception we catch, or maybe this is a switch?
  if (oc_tmp__675.exception.type == OC__EXCEPTION_TYPE) {
     goto oc_label__777;
  } else {
     // this is the re-throw case
     oc_exception = oc_tmp__675.exception;
     goto oc_label_defer_and_return;
  }
}

oc_label_defer_and_return:
for (int i = 0; oc_num_deferred; i++) {
   int oc_defer_code = oc_deffered_codes[i];
   switch (oc_defer_code) {
      case 1: {
        // TODO(jawilson): how to handle variables that go out of scope?
        // the code generation for the deferred statement goes here.
        // what to do with additional exceptions?
      }
   }
}
return { oc_return_result, oc_exception };
```
In order to support both of these, we simply linearize expressions into trivial statements
by introducing a bunch of temporaries at which point the call/return site is a single statement
we can place code after (or before if we end up needing that). This transformation would also
enables us to essentially make things like blocks inside of expressions work (whereas in C
you would only be able to do this with function inlining which can't interact with local
variables in the normal way).

### Temporary Name Generation

The full name (which includes the namespace and the type signature) is appended with a local temporary count number string which is then hashed to produce a 64bit integer which is converted to a 16 digit hexidecimal string. Collisions that matter would be rare since a single function only
has on the order of thousands of temporaries. With a cost, we can detect temporary name collions
and use either a different hash function (same size or larger) or a different seed for the same hash function.

## Strucuture Layout

We will examine the C rules for structure packng. In any event, an attribute is provided to force us to use the platforms ABI structure layout (which is not possible with generic structures).

## Generic Structure Members

The slow way to do this is to replace variably size fields with a pointer that points to a heap allocated space for the field and automatically manipulate it (we can also use stack allocated little arrays for locals). A similar trick could be used for (tagged) unions. We have to make copies of these when a structure is assigned to a variable.

A more sophisticated approach would still use a pointer but initialize it to space allocated adjacent to the object itself. Again when copying, we need to copy more bytes than just the structure and
rewrite all such pointers relative to the address of the new target.

In theory all structures with generically typed fields must remember their shape so we will have
something akin to a type pointer at the start of such structures.

## Library Format

We output a series of items describing each file (aka, metadata) and then the content of the file as it existed when the library was created.

A library item looks like this:

```
type=<embed-data or source-file or ...>
file-name=foo/bar/file.oc
data-length=<number-of-bytes>
data=.........
```
data always comes last in a library item (which necessarily means that data-length precides it).

When making libraries the tool should ideally parse the sources files to catch some errors early.



