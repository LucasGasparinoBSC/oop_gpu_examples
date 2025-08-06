# Simple OOP Fortran

Simple example to demonstrate Fortran OOP limitations when using OpenACC.

## Details

In this example, the type definition is more similar to a C/C++ `class`, containing a private attribute and public methods to access and modify it.
Although this example is perfectly valid Fortran code on host, trying to call any of the methods inside an OpenACC region will result in a compilation error:

```bash
NVFORTRAN-F-1252-Call to type-bound procedures are not allowed on device - setscalar$tbp.
```

## Exercises

1. Modify the type definition to allow attribute modification from the device. The attribute must remain private.

## Compilation

The included CMake structure compiles the set of examples. The executable for this example is located in:

```bash
build/openacc/fortran/simple_oop_fortran/simple_oop_fortran
```

Remember to use the NVHPC compilers!
NOTE: This example will NOT compile as-is, given the above mentioned limitations of Fortran OOP with OpenACC.