module pointClass
    implicit none

    type :: Point
        integer(4) :: pID = 0
        integer(4) :: dataSize = 0
        real(4), pointer :: data(:) => null()
    end type Point

    contains

        subroutine initPoint(this, id, size)
            class(Point), intent(inout) :: this
            integer(4), value :: id, size

            this%pID = id
            this%dataSize = size
            allocate(this%data(size))
            this%data = 0.0

            !$acc enter data copyin(this)
            !$acc enter data create(this%data(1:this%dataSize))
            !$acc enter data attach(this%data)
        end subroutine initPoint

        subroutine printPoint(this)
            class(Point), intent(in) :: this
            integer :: i

            !$acc update self(this%data(1:this%dataSize))
            print *, "---------------------"
            print *, "Point ID: ", this%pID
            print *, "Data: "
            do i = 1, this%dataSize
                print *, this%data(i)
            end do
            print *, "---------------------"
        end subroutine printPoint
end module pointClass

program driver

    use pointClass
    implicit none
    integer :: i, j, numPoints, dSize
    type(Point), allocatable :: points(:)

    numPoints = 3
    dSize = 5
    allocate(points(numPoints))
    !$acc enter data copyin(points(1:numPoints))
    do i = 1, numPoints
        call initPoint(points(i), i, dSize)
    end do

    !$acc parallel loop gang present(points) default(present)
    do i = 1, numPoints
        !$acc loop vector
        do j = 1, dSize
            points(i)%data(j) = 1.0
        end do
    end do
    !$acc end parallel loop

    do i = 1, numPoints
        call printPoint(points(i))
    end do

    !$acc parallel loop gang present(points) default(present)
    do i = 1, numPoints
        !$acc loop vector
        do j = 1, dSize
            points(i)%data(j) = real(i*j, kind=4)
        end do
    end do
    !$acc end parallel loop

    do i = 1, numPoints
        call printPoint(points(i))
    end do
end program driver