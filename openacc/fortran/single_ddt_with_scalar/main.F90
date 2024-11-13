! Module containing the data type definition
module ddt_mod
    type ddt
        integer :: id
    end type ddt
end module ddt_mod

program single_ddt_with_scalar

    ! GPU modules
    use openacc
    use cudafor

    ! Module containing the data type definition
    use ddt_mod

    ! Declare variables
    implicit none
    type(ddt) :: h_obj ! Host object
    type(ddt) :: d_obj ! Device object

    ! Host:
    h_obj%id = 1
    print *, "Host: h_obj%id = ", h_obj%id
    print *, ""

    ! Device:
    !$acc enter data copyin(d_obj, d_obj%id)
    !$acc kernels present(d_obj, d_obj%id)
    d_obj%id = 2
    !$acc end kernels
    !$acc exit data copyout(d_obj%id, d_obj)
    print *, "Device: d_obj%id = ", d_obj%id

end program single_ddt_with_scalar