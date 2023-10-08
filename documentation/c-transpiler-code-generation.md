# C transpiler Code Generation

Here we describe some of the bigger challenges and decisions that were
made in converting functions into portable C (and hopefully efficient)
C code that implements the semantics of the Omni C language versus the
C language.

## Closure Conversion

The goal of closure conversion is to convert closures into simple
namespace level functions. All such functions accept an extra argument
initial argument which is an abstraction that allows a closure to
access it's captured variables. (This is a pointer to an automatically
generated structure.)

Closure conversion happens before linearization.

## Linearization

Linearization is the last step of the conversion to C code (besides
potentially scaffolding generation) and happens at the same time as
the actual code C code text is generated.

Linearization is necssary to support certain advanced features of Omni
C such as defer, exceptions, let, fluid_let, etc.

...FIX THE CODE BELOW...

Essentially we have statements and expressions.

The two most interesting deviations from C that cause problems are
exceptions and (self) tail calls (when not using gcc or clang).

Self tail calls requires that we add a label as the first statement of
the function and then the self call must instead of doing the call,
perform a bunch of assignments, execute deferred expressions, then do
a goto which obviously is not itself a C expression.

Exceptions also require a fair amount of code at the call/return site
in order to short circuit.  This code is also not itself a C
expression. In fact exceptions require that we do something like this
for each call (source line directives have been removed):

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

In order to support both of these, we simply linearize expressions
into trivial statements by introducing a bunch of temporaries at which
point the call/return site is a single statement we can place code
after (or before if we end up needing that). This transformation would
also enables us to essentially make things like blocks inside of
expressions work (whereas in C you would only be able to do this with
function inlining which can't interact with local variables in the
normal way).

### Temporary Name Generation

The full name (which includes the namespace and the type signature) is
appended with a local temporary count number string which is then
hashed to produce a 64bit integer which is converted to a 16 digit
hexidecimal string. Collisions that matter would be rare since a
single function only has on the order of thousands of
temporaries. With a cost, we can detect temporary name collions and
use either a different hash function (same size or larger) or a
different seed for the same hash function.

## Strucuture Layout

We will examine the C rules for structure packing. In any event, an
attribute is provided to force us to use the platforms ABI structure
layout (which is not possible with generic structures).

## Generic Structure Members

The slow way to do this is to replace variably size fields with a
pointer that points to a heap allocated space for the field and
automatically manipulate it (we can also use stack allocated little
arrays for locals). A similar trick could be used for (tagged)
unions. We have to make copies of these when a structure is assigned
to a variable rather than have the C compiler do it for us.

A more sophisticated approach would still use a pointer but initialize
it to space allocated adjacent to the object itself. Again when
copying, we need to copy more bytes than just the structure and
rewrite all such pointers relative to the address of the new target.

In theory all structures with generically typed fields must remember
their shape so we will have something akin to a type pointer at the
start of such structures.
