! Module containing the data type definition
module ddt_mod
    type ddt
        integer :: id
    end type ddt
end module ddt_mod

program multiple_ddt_with_scalar

    ! GPU modules
    use openacc
    use cudafor

    ! Module containing the data type definition
    use ddt_mod

    ! Declare variables
    implicit none
    integer(4), parameter  :: num_obj=10
    integer(4)             :: i
    type(ddt), allocatable :: h_obj(:) ! Host object
    type(ddt), allocatable :: d_obj(:) ! Host object

    ! Host:
    allocate(h_obj(num_obj))
    do i=1,num_obj
        h_obj(i)%id = i-1
        print *, "Host: h_obj(",i,")%id = ", h_obj(i)%id
    end do

    ! Device:
    allocate(d_obj(num_obj))
    !$acc enter data copyin(d_obj, d_obj%id)
    !$acc parallel loop gang present(d_obj, d_obj%id)
    do i=1,num_obj
        d_obj(i)%id = i
    end do
    !$acc end parallel loop
    !$acc update self(d_obj%id, d_obj)
    do i=1,num_obj
        print *, "Device: d_obj(",i,")%id = ", d_obj(i)%id
    end do

end program multiple_ddt_with_scalar