# Single DDT with allocatable

Very simple example on using Derived Data Types (DDT) with OpenACC in Fortran, with added caveat that the DDT contains an allocatable array.

## Details

The DDT in this case is similar to a C/C++ `struct`: it contains only publicly available attributes, in this case a simple scalar attribute and an allocatable array attribute.
Although not necessary, this test creates separate copies for the host and device objects. Once the host is fully initialized, it is copied over to the device: here, notice that the `acc data` directive copies both the object itself AND its scalar attribute, in that order.
The test then executes a simple device kernel to modify the device attribute then copies back to host. Once again, the `acc data` directive handles both object and attribute, but this time the attribute is copied back first, otherwise it will become undefined.

## Exercises

1. Remove the device object and use only the host object.
2. In a similar C/C++ example, the scalar attribute was NOT copied. Test if this is possible in Fortran as well.
3. Similar idea to above, but with the allocatable array attribute: try to not copy it to the device and see if it works.

## Compilation

The included CMake structure compiles the set of examples. The executable for this example is located in:

```bash
build/openacc/fortran/single_ddt_with_allocatable/single_ddt_with_allocatable
```

Remember to use the NVHPC compilers!