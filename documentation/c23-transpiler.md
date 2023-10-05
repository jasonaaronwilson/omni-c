# The C23 Transpiler

The C23 transpiler converts source files and libraries (which are simply the
concatention of one or more sources files) into C code conforming to the C23
standard which can then be compiled with suitable C compilers such as clang
and gcc.

## High Level Overview

1. the command line is parsed to determine a set of libraries and source files to compile
2. in parallel, all of the source files are parsed. this will require at least as much space
   as the source files and libraries themselves
3. we assemble namespaces by collecting each of the name space fragments
4. for each namespace we create the private and public top-level definitions
5. type checking and annotation - every expression is given a type
6. we move closures to the namespace level by having them take a struct pointer for
   captured variables, aka, an environment and replace with appropriate code at the
   lambda site
7. for each namespace in parallel, we output a C source file via an inorder treewalk
    of all functions in the namespace sorted by function name (for reproduceability)
8. for each namespace in parallel, collect all of the function prototype fragments, enum fragments,
    etc.
9. we typologically sort the header file. enumerations come first, structures (including environments)
   come next then prototypes  
10. invoke the C compiler (in parallel?) on all of the generated source files

## Function Code Generation

Essentially we have statements and expressions inside some of those statements.

The two most interesting deviations from C that cause problems are exceptions and
(self) tail calls (when not using gcc or clang).

Self tail calls require that we add a label as the first statement of the function and then the self call must instead of doing the call, perform a bunch of assignments then do a goto which obviously is not itself a C expression.

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
enables us to essentially make things like blocks inside of expressions work.

### Temporary Name Generation

The full name (which includes the type signature and must be unique) is appended with a local temporary count number (in ascii) which is then hashed to produce a 64bit integer. I suppose as a backend we could do 128 bit hashes if we detect a collision.

## Generic Structure Members

The slow way to do this is to replace variably size fields with a pointer that points to a heap allocated space for the field and automatically manipulate it (we can also use stack allocated little arrays for locals). A similar trick could be used for (tagged) unions. We have to make copies of these when a structure is assigned to a variable.

A more sophisticated approach would still use a pointer but initialize it to space allocated adjacent to the object itself. Again when copying, we need to copy more bytes than just the structure and
rewrite all such pointers relative to the address of the new target.

In theory all structures with generically typed fields must remember their shape so we will have
something akin to a vtable at the start of such structures?

## Strucuture Layout






