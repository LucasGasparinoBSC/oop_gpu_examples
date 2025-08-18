# Self instantiation

Siple example showing how `this` can be created on device from the constructor

## Details

In this example, the class parametrized constructor ensures that both the attribute and itself are copied over to the device before any ops can be performed on it.
This is handled by the clause `acc enter data copyin(this)`, which ensures that any object created on host belonging to this class will have a device counterpart.

The destructor also handles memory clean-up, ensuring that the attribute is cleared first to avoid a loss of reference.

In the printer, only the `data` pointer needs to be updated to the host, as the object itself is already present. OpenACC will handle the scalar attributes automatically.

In the main program, 2 kernels use the object, altering the contents of the data attribute. Notice the use of `present(obj)` to enforce that the object is in device memory before accessing it.

## Exercises

1. Add another dynamic array attribute to the `Basic` class. Ensure it is properly managed in the constructor, destructor, and any relevant methods.
2. What happens if the program tries to create an array of `Basic` objects?
3. Write a host-side kernel to modify the data attribute. Does it have to be a member of `Basic`?
4. Create another class that uses `Basic` objects.
5. Create a child class of `Basic` with additional data attributes, then override the relevant methods.

## Compilation

The included CMake structure compiles the set of examples. The executable for this example is located in:

```bash
build/openacc/c_cpp/self_instantiation/self_instantiation
```

Remember to use the NVHPC compilers!

## NSYS execution

```bash
nsys profile --trace=nvtx,cuda,openacc -f true --cuda-memory-usage=true -o [reportName] ./build/openacc/c_cpp/self_instantiation/self_instantiation
```