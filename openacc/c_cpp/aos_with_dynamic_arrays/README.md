# Array of Structures

Simple example of how to allocate and use an Array of Structures (AoS) in OpenACC + C.

## Details

The main detail here is how the AoS is transferred to the device: since each structure contains a dynamically allocated pointer, these have to be managed separately. Instead of using a `acc enter data create` clause, we opted to `acc enter data copyin` the array of objects, then loop over each object to `acc enter data copyin` the pointer to the data. Notice that the scalar objects are never copied, as those transfers are done by copying the objects themselves.
Once the objects live in memory, it is simple to use them in a parallel construct, where the `gang` dimension handles the object index and the `vector` dimension handles the inner data pointers. The `present` option in the clause ensures the compiler that the object is indeed in device memory: if this is not true, the code will break at runtime.
The use of two kernels serves to illustrate that, once the object is in memory, as well as its pointer attribute, no implicit data management is necessary. Check the `nsys` output to verify this fact.
Finally, the data is copied back to the host by using the `acc exit data copyout` clause, which copies the data pointer first, followed by the actual data. If the objects are copied out first, access to the data pointers are lost, which would result in a segmentation fault when trying to access the data on the host.

## Exercises

1. Modify the code to use `acc update` instead of `acc exit data copyout` to copy the data back to the host.
2. Try using `acc enter data create` instead of `acc enter data copyin` for the array of objects. What happens? Why?
3. Verify whether the `value` pointers need to be private in the `vector` loop. What happens if they are not?
4. Experiment using a matrix instead of an AoS. Is there any performance difference? Why?
5. Run the examples with `nsys` to check the data transfers. Repeat for any modifications you make. What do you observe?

## Compilation

The included CMake structure compiles the set of examples. The executable for this example is located in:

```bash
build/openacc/c_cpp/aos_with_dynamic_arrays/aos_with_dynamic_arrays
```

Remember to use the NVHPC compilers!

## NSYS execution

```bash
nsys profile --trace=nvtx,cuda,openacc -f true --cuda-memory-usage=true -o [reportName] ./build/openacc/c_cpp/aos_with_dynamic_arrays/aos_with_dynamic_arrays
```