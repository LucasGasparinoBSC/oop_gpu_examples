! Module containing the data type definition
module ddt_mod
    type ddt
        integer(4)           :: id
        real(4), allocatable :: data(:)
    end type ddt
end module ddt_mod

program single_ddt_with_allocatable

    ! GPU modules
    use openacc
    use cudafor

    ! Module containing the data type definition
    use ddt_mod

    ! Declare variables
    implicit none
    integer(4), parameter :: data_size=10
    integer(4)            :: i
    type(ddt)             :: h_obj ! Host object
    type(ddt)             :: d_obj ! Device object

    ! Host:
    ! Initialize the host object
    h_obj%id = 1
    allocate(h_obj%data(data_size))
    h_obj%data = 1.0
    do i = 1, data_size
        print *, h_obj%data(i)
    end do
    print *, ""

    ! Device:
    allocate(d_obj%data(data_size))
    !$acc enter data copyin(d_obj, d_obj%id, d_obj%data)
    !$acc kernels present(d_obj, d_obj%id)
    d_obj%id = 2
    !$acc end kernels
    !$acc parallel loop vector present(d_obj, d_obj%data)
    do i = 1, data_size
        d_obj%data(i) = 2.0
    end do
    !$acc end parallel loop
    !$acc exit data copyout(d_obj%data, d_obj%id, d_obj)
    print *, d_obj%id
    do i = 1, data_size
        print *, d_obj%data(i)
    end do

end program single_ddt_with_allocatable