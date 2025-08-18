# Self instantiation (advanced)

Complex example of self-instantiation on device using nested objects and AOOs.

## Details

This extends the `self instantiation` example by employing nested and hierarchical object structures.

The `Line` class contains AOOs for `Point` and `GaussPoint`, where the latter is a child class of the former. The main program will also create an array of `Line` objects.
This is a simplified structure that might occur in a Finite Elements code, where elements described as objects are composed of arrays of lesser objects.

Similar to the simpler example, each class Constructor/Setter calls `#pragma acc enter data copyin(this)`, ensuring that the entire object is available on the device. However,
when `Line` creates the AOOs, it must account for the Setters already building device objects! So, it performs as follows:

1. call `#pragma acc enter data create(AOO[:])` for `Point` and `GaussPoint` arrays
2. Loop over the arrays calling the Setters for each of these objects

In this way, the `copyin` operations in each Setter only fill up device data for each AOO entry, which is valid OpenACC code. The process is identical for creating
the array of `Line` objects: after the host allocates the AOO, `data create` is used to generate the empty device array, then the Setter for `Line` ensures that each entry is filled up as requested.

The kernel can then safely perform `loop gang` operations over the number of lines, and `loop vector/worker` operations over points and Gauss points in a line.
Notice that scalar variables have automatic updates once the objects are created, so the printers must only update the attributes. In particular, printing the line
requires no additional updates, as the `Point/GaussPoint` printer methods only need to update their local copies of the data.

## Exercises

1. Create a `QuadElement` class that instantiates 4 `Line` objects. Ensure that point indexing is adjusted accordingly.
2. Based on the simple example, build the Destructor methods for each class.
3. Add a 2nd kernel modifying the `Line` objects. Analyze with NSYS.
4. Discuss the pros/cons of this approach over flat data structures with no object-oriented features.

## Compilation

The included CMake structure compiles the set of examples. The executable for this example is located in:

```bash
build/openacc/c_cpp/self_instantiation_adv/self_instantiation_adv
```

Remember to use the NVHPC compilers!

## NSYS execution

```bash
nsys profile --trace=nvtx,cuda,openacc -f true --cuda-memory-usage=true -o [reportName] ./build/openacc/c_cpp/self_instantiation_adv/self_instantiation_adv
```