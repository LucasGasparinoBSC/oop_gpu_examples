# Simple self instantiation in Fortran

This example illustrates a self instantiation scenario in Fortran using OpenACC. Here, the derived data type `Point` contains a dynamically allocated pointer of real data, and the driver creates an array of `Point` objects, computing 2 kernels on it.

## Details

This illustrates how simple object oriented programming can be done in Fortran + OpenACC in a more advanced fashion, with self-instantiation of derived data types. An important detail is that the attribute `data` is a pointer, not an allocatable array. This makes its behaviour more similar to C++ and allows for a closer mapping to its concepts. The downside is that the user has to manage memory more carefully, and the code is more verbose. As well, here the attribute has to be created, then attached to the host, instead of a simple copyin.

## Exercises

1. Remove the intermediate print and run the example with NSYS. Is there any data transfer occuring between the 2 kernels? Use larger data sizes to see the effect more clearly.
2. Substitute the `create+attach` directives with a `copyin` directive. Does the code still work? Why (not)?
3. Make the attributes private and add get/set methods to access them. Does the code still work? Why (not)?
4. Make the kernels in the driver into methods of the `Point` class. Does the code still work? Why (not)?

## Compilation

The included CMake structure compiles the set of examples. The executable for this example is located in:

```bash
build/openacc/fortran/self_instantiation/self_inst_f
```

Remember to use the NVHPC compilers!