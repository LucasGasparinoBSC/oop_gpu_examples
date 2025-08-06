# Multiple DDT with Allocatable

More complex example of using OpenACC with Fortran derived data types (DDT): now, an Array of Objects (AOO) is created, where each object contains an allocatable array.

## Details

Given that all attributes are publicly accessible, the device data handling is fairly straightforward: as the host object is created, `acc data` clauses ensure that all attributes are copied to the device after they are initialized. In particular, we first ensure that the host is fully initialized (not just allocated), then `acc copyin` within a loop.

To test object permanence, two kernels are used. The 1st kernel is executed inside a subroutine, demonstrating that only the object can indeed be passed as an argument once its properly created on both host and device.

The 2nd kernel demonstrates how shared memory and block-private arrays can safely be used with device object data. Although not necessary here, a non-coalesced access to the inner data array in an object can be optimally handled by through this technique.

Finally, the data is copied out back to the host: notice that a loop over all objects first copies back all attributes, and then the AOO is copied back to the host.

## Exercises

1. Remove all explicit `acc` data management and recompile the code with managed memory support. Does it work as expected, or at all?
2. Run the `Nsight Systems` profiler on the code with larger datasets. Compare the kernel times vs. the data transfer times.
3. This class can be represented as an array of IDs and a matrix of data. Compare that with the OOP approach presented here.

## Compilation

The included CMake structure compiles the set of examples. The executable for this example is located in:

```bash
build/openacc/fortran/multiple_ddt_with_allocatable/multiple_ddt_with_allocatable
```

Remember to use the NVHPC compilers!