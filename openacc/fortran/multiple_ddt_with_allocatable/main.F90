! Module containing the data type definition
module ddt_mod
    type ddt
        integer(4)           :: id
        real(4), allocatable :: data(:)
    end type ddt
end module ddt_mod

! Sunroutine for modifying the object data
pure subroutine aoo_kernel(obj_array, num_objs, data_size)
    use ddt_mod
    implicit none
    integer(4), INTENT(IN) :: num_objs, data_size
    type(ddt), INTENT(INOUT) :: obj_array(num_objs)
    integer(4) :: i, j

    !$acc parallel loop gang present(obj_array)
    do i = 1, num_objs
        obj_array(i)%id = i-1
        !$acc loop vector
        do j = 1, data_size
            obj_array(i)%data(j) = real(1, 4)
        end do
    end do
    !$acc end parallel loop

end subroutine aoo_kernel

program multiple_ddt_with_allocatable

    ! GPU modules
    use openacc
    use cudafor

    ! Module containing the data type definition
    use ddt_mod

    ! Declare variables
    implicit none
    integer(4), parameter  :: data_size=128 ! Size of the data array in each object
    integer(4), parameter  :: num_objs=1000  ! Number of objects created
    integer(4)             :: i, j         ! Access indexes

    ! Instantiate an allocatable array of ddt objects
    type(ddt), allocatable :: obj_array(:)

    ! tmp variable for testing shared memory usage
    real(4) :: tmp(data_size)

    ! Create the object:

    ! Allocate the array of ddt objects on host
    allocate(obj_array(num_objs))

    ! Create the object array on device
    !$acc enter data copyin(obj_array)

    ! Touch the object attributes
    do i = 1, num_objs
        ! Handle the scalar attribute:
        obj_array(i)%id = 0                        ! Set the obj index attribute
        !$acc enter data copyin(obj_array(i)%id)   ! Copy the modified id to device

        ! Handle the allocatable array attribute:
        allocate(obj_array(i)%data(data_size))     ! Allocate the data array
        obj_array(i)%data = 0.0                    ! Initialize the data array to 0
        !$acc enter data copyin(obj_array(i)%data) ! Copy the modified data array to device
    end do

    ! 1st kernel to initialize the object data, defined on the subroutine
    call aoo_kernel(obj_array, num_objs, data_size)

    ! 2nd kernel to modify the object data
    !$acc parallel loop gang present(obj_array) private(tmp)
    do i = 1, num_objs
        !$acc loop vector
        do j = 1, data_size
            tmp(j) = obj_array(i)%data(j)
        end do
        !$acc loop vector
        do j = 1, data_size
            tmp(j) = tmp(j) + real(i+j, 4)
            obj_array(i)%data(j) = tmp(j)
        end do
    end do
    !$acc end parallel loop

    ! Copy back to host
    do i = 1, num_objs
        !$acc exit data copyout(obj_array(i)%data, obj_array(i)%id)
    end do
    !$acc exit data copyout(obj_array)

    ! Print the object data
    do i = 1, num_objs
        print *, "i := ", i, " id = ", obj_array(i)%id
        print*, "Data array: "
        do j = 1, data_size
            print *, "  obj_array(", i, ")%data(", j, ") = ", obj_array(i)%data(j)
        end do
    end do
end program multiple_ddt_with_allocatable