# single_c_struct

Demonstrates how to instantiate and initialize a single struct object containing scalar attributes in C+OpenACC code.

## Details

The device code is remarkably similar to the host one, apart from the OpenACC pragamas to perform GPU operations. In summary:

- The 'acc enter data create' directive allocates memory for the struct on the device;
- The 'acc kernels' allows the GPU to manipulate the attributes of the struct, provided that the 'present' option is added to ensure that the object is available on the device;
- The 'acc update host' directive ensures that the host version of the struct is updated, and that its info can be printed;

Importantly, if the 'present' option is not used for the kernel, no operation is performed on the device struct, and the printed info is incorrect.