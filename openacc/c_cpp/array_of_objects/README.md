# Array of Objects

More complex example of Object Orientation, in which an outer object can contain an array of inner objects, with the inner objects themselves containing dynamic pointers.

## Details

This is a far more complex version of the AoS case: each Line object created contains an array of Point objects; each Point object contains a dynamic Data pointer. The test then creates an array of Line objects. This complex structure is reminiscent of what a Finite Elements code could do, nesting different levels of elements inside each other.

Notice that, different from the AoS case, class data is now PRIVATE, requiring getter methods to be accessible.

Here, instead of trying to first create a host AOO, then deep copying it to the device, methods in the classes are responsible for managing internal pointers: for example `setPoint` will first allocate the `pData` array on host with `calloc`, then copy it to device, resulting in a host object associated with device pointers. Later on, the `Line` class ensures that all `Point` objects are properly moved to the device.

This example also adds a simple NVTX macro for making profiling with `Nsight Systems` more detailed.

## Exercises

1. Can you remove the `acc data` clauses from class members? Would a host-then-device strategy work?
2. Make the class attributes public and redesign the device data management.
3. OpenACC offers the possibility of using automagic data management. Test it on this example: eliminate all explicit data clauses and recompile with MANAGED=ON.
4. Extend the example by adding a `Face` class holding an array of `Lines`, then create an array of faces.
5. Run the examples with `nsys` to check the data transfers. Repeat for any modifications you make. What do you observe?

## Compilation

The included CMake structure compiles the set of examples. The executable for this example is located in:

```bash
build/openacc/c_cpp/array_of_objects/aoo
```

Remember to use the NVHPC compilers!

## NSYS execution

```bash
nsys profile --trace=nvtx,cuda,openacc -f true --cuda-memory-usage=true -o [reportName] ./build/openacc/c_cpp/array_of_objects/aoo
```
