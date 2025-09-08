# Advanced self instantiation in Fortran

This example is a simplified version of the C++ one, illustrating the limitations of Fortran OOP when using OpenACC.

## Details

The idea here is similar to the C++ one: a `Line` derived data type contains an array of `Point` derived data types. Each `Point` has a dynamically allocated array of reals representing its data. The key differences are:

- All attributes are public, since getter/setter methods must be type-bound, which is not supported by Fortran OpenACC
- No type-bound methods are used, since they are not supported by Fortran OpenACC

Although the same logic as the C++ example is followed, the end result is not the same: the code compiles and runs, but the printed data does not reflect the kernel executed. The nature of Fortran OOP with OpenACC is such that the compiler does not properly handle the nested derived types with dynamic memory allocation. This is a limitation of the current Fortran compilers and OpenACC implementations.

## Exercises

In this case there is no alternative routine: the data structures have to be flattened manually :grimacing:
Trying to switch from `allocatable` to `pointer` and see what changes, although this still doesn't solve the fundamental issue: the subroutines working with `this`work on copies of the object, losing references to the original data (no deep-copying).

## Compilation

The included CMake structure compiles the set of examples. The executable for this example is located in:

```bash
build/openacc/fortran/self_instantiation_adv/main
```

Remember to use the NVHPC compilers!