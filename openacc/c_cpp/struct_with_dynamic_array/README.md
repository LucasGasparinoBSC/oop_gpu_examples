# struct_with_static_array

Demonstrates how to instantiate and initialize a single struct object containing both an int scalar and a static float array.

## Details

The device code is remarkably similar to the host one, apart from the OpenACC pragamas to perform GPU operations. In summary:

- The 'acc enter data create' directive allocates memory for the struct on the device;
- The 'acc kernels' allows the GPU to manipulate the attributes of the struct, provided that the 'present' option is added to ensure that the object is available on the device;
- For the static array, a 'parallel loop' directive is used, including the 'present' option indicating that the struct object is in memory;
- The 'acc update host' directive ensures that the host version of the struct is updated, and that its info can be printed;

For this case, access to the array attribute in the device object is quite simple, and requires no special allocation apart from the structure object itself. As well, updating the host requires only that the object itself is updated, not the array.