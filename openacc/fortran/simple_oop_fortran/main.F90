! Module for containing the DDT definition
module ddt_mod
    implicit none

    ! Default to private
    private

    ! Public type name and methods
    public :: simpleObject
    public :: getScalar, setScalar

    ! Define the type
    type :: simpleObject
        private
        integer :: scalar ! Simple scalar attribute
        contains
            procedure :: getScalar
            procedure :: setScalar
    end type simpleObject

    ! Define the methods
    contains

        ! Method to get the scalar value
        pure function getScalar(self) result(value)
            implicit none
            class(simpleObject), intent(in) :: self
            integer :: value
            value = self%scalar
        end function getScalar

        ! Method to set the scalar value
        pure subroutine setScalar(self, value)
            implicit none
            class(simpleObject), intent(inout) :: self
            integer, intent(in) :: value
            self%scalar = value
        end subroutine setScalar

end module ddt_mod

! Driver program for testing the derived data type with OpenACC
program main
    use openacc
    use cudafor
    use ddt_mod
    implicit none

    ! Instantiate the object
    type(simpleObject) :: obj

    ! Host side: set the scalar value
    call obj%setScalar(10)
    print *, "Host: obj%scalar = ", obj%getScalar()

    ! Copy the object to the device
    !$acc enter data copyin(obj)

    ! Simple kernel to modify the scalar value on the device
    ! NOTE: Given Fortran OpenACC limitations, this doesnt work!
    !$acc kernels present(obj)
    call obj%setScalar(20)
    !$acc end kernels

    ! Copy the modified object back to the host
    !$acc exit data copyout(obj)
    print *, "Device: obj%scalar = ", obj%getScalar()
end program main