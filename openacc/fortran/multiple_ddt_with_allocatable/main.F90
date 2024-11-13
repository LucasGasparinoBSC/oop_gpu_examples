! Module containing the data type definition
module ddt_mod
    type ddt
        integer(4)           :: id
        real(4), allocatable :: data(:)
    end type ddt
end module ddt_mod

program multiple_ddt_with_allocatable

    ! GPU modules
    use openacc
    use cudafor

    ! Module containing the data type definition
    use ddt_mod

    ! Declare variables
    implicit none
    integer(4), parameter  :: data_size=10
    integer(4), parameter  :: num_obj=3
    integer(4)             :: i, j
    type(ddt), allocatable :: h_obj(:)
    type(ddt), allocatable :: d_obj(:)

    ! Host:
    allocate(h_obj(num_obj))
    do i=1,num_obj
        allocate(h_obj(i)%data(data_size))
        h_obj(i)%id = i
        print *, ""
        print *, "Object ", h_obj(i)%id
        do j=1,data_size
            h_obj(i)%data(j) = i*j
            print *, "Data ", j, ": ", h_obj(i)%data(j)
        end do
    end do

    ! Device:
    allocate(d_obj(num_obj))
    !$acc enter data create(d_obj)
    do i = 1,num_obj
        allocate(d_obj(i)%data(data_size))
        !$acc enter data create(d_obj(i)%id, d_obj(i)%data)
    end do

    !$acc parallel loop gang
    do i = 1,num_obj
        d_obj(i)%id = i+1
        !$acc loop vector
        do j = 1,data_size
            d_obj(i)%data(j) = (i*j)+1
        end do
    end do
    !$acc end parallel loop

    do i = 1,num_obj
        !$acc exit data copyout(d_obj(i)%id, d_obj(i)%data)
    end do
    !$acc update host(d_obj)

    do i = 1,num_obj
        print *, ""
        print *, "Object ", d_obj(i)%id
        do j = 1,data_size
            print *, "Data ", j, ": ", d_obj(i)%data(j)
        end do
    end do
end program multiple_ddt_with_allocatable