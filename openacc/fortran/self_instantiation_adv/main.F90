module pointClass
    implicit none

    type :: Point
        integer(4) :: pID = 0
        integer(4) :: dataSize = 0
        real(4), allocatable :: data(:)
    end type Point

    contains

        subroutine initPoint(this, id, size)
            type(Point), intent(inout) :: this
            integer(4), value :: id, size

            this%pID = id
            this%dataSize = size
            allocate(this%data(size))
            this%data = 0.0

            !$acc enter data copyin(this)
            !$acc enter data copyin(this%data(1:this%dataSize))
        end subroutine initPoint

        subroutine printPoint(this)
            type(Point), intent(in) :: this
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

module lineClass
    use pointClass
    implicit none
    type :: Line
        integer(4) :: lineID = 0
        integer(4) :: numPoints = 0
        type(Point), allocatable :: points(:)
    end type Line

    contains

        subroutine initLine(this, id, nPoints, pSize)
            implicit none
            type(Line), intent(inout) :: this
            integer(4), value :: id, nPoints, pSize
            integer :: i

            this%lineID = id
            this%numPoints = nPoints
            !$acc enter data copyin(this)

            allocate(this%points(nPoints))
            !$acc enter data create(this%points(1:this%numPoints))
            do i = 1, nPoints
                call initPoint(this%points(i), i, pSize)
            end do
        end subroutine initLine
end module lineClass

program driver
    use lineClass
    implicit none
    type(Line), allocatable :: lines(:)
    integer(4) :: nLines, nPoints, pSize, i, j, k

    nLines = 2
    nPoints = 3
    pSize = 4
    allocate(lines(nLines))
    !$acc enter data create(lines(1:nLines))
    do i = 1, nLines
        call initLine(lines(i), i, nPoints, pSize)
    end do

    !$acc parallel loop gang present(lines) default(present)
    do i = 1, nLines
        !$acc loop vector
        do j = 1, lines(i)%numPoints
            !$acc loop seq
            do k = 1, lines(i)%points(j)%dataSize
                lines(i)%points(j)%data(k) = real(i + j + k, 4)
            end do
        end do
    end do
    !$acc end parallel loop

    do i = 1, nLines
        do j = 1, lines(i)%numPoints
            call printPoint(lines(i)%points(j))
        end do
    end do
end program driver