! Module containing the derived data type (ddt) definition
module ddt_mod
    implicit none
    ! This is similar to defining a `Struct` in C/C++: no methods, just public attributes.
    type ddt
        integer :: id ! Simple scalar attribute
    end type ddt
end module ddt_mod

! Driver program for testing the derived data type with OpenACC
program single_ddt_with_scalar

    ! GPU modules
    use openacc
    use cudafor

    ! Module containing the data type definition
    use ddt_mod

    ! Declare variables
    implicit none

    ! Notice that declaring the ddts here is already an object instantiation.
    type(ddt) :: h_obj ! Host object
    type(ddt) :: d_obj ! Device object (unnecessary, for demonstration purposes only)

    ! Host:
    h_obj%id = 1
    print *, "Host: h_obj%id = ", h_obj%id
    print *, ""

    ! Device:
    ! Copy both the object and its scalar attribute to the device
    !$acc enter data copyin(d_obj, d_obj%id)

    ! Execute a simple device operation on the object attribute
    !$acc kernels present(d_obj, d_obj%id)
    d_obj%id = 2
    !$acc end kernels

    ! Copy the modified object back to the host
    ! NOTE: the attribute must be copied back BEFORE the object itself, otherwise it will become undefined.
    !$acc exit data copyout(d_obj%id, d_obj)
    print *, "Device: d_obj%id = ", d_obj%id

end program single_ddt_with_scalar