# multiple_c_struct

Demonstrates how to instantiate and initialize an array of struct objects containing scalar attributes in C+OpenACC code.

## Details

The device code is remarkably similar to the host one, apart from the OpenACC pragamas to perform GPU operations. In summary:

- The device object is initially created as a pointer, and malloc is used to create a host array;
- The 'acc enter data create' directive allocates memory for the array of structs on the device;
- The 'acc kernels' allows the GPU to manipulate the attributes of each struct, provided that the 'present' option is added to ensure that the objects are available on the device;
- The 'acc update host' directive ensures that the host version of the array of structs is updated, and that its info can be printed;

Importantly, if the 'present' option is not used for the kernel, no operation is performed on the device struct, and the printed info is incorrect. The 'acc kernels' directive can be substituted with 'parallel loop' to achieve the same result.