C File: arrays-basic-04.f
C This program has a 2-D array with the implicit lower bounds of 1.

      program main
      implicit none

C     arr is a 2-D array (5x5) of integers with implicit lower-bounds 
C     of 1 and upper bounds of 5.  
      integer, dimension(5,5) :: arr 
      integer :: i, j

      do i = 1, 5
        do j = 1, 5
          arr(i,j) = i+j     ! initialize the array
        end do
      end do

      ! write out the array
      do i = 1, 5
          write (*,10) arr(i,1), arr(i,2), arr(i,3), arr(i,4), arr(i,5)
      end do

 10   format(5(I5,X))

      stop
      end program main
