  ! xytest.f03: Generate test data file for xyplt.
program xytest
  real(4), parameter :: pi = 3.141592654
  real(4), parameter :: xmin = 0.0, xmax = 2*pi
  integer(4), parameter :: n = 50
  real(4), parameter :: dx = (xmax-xmin)/(n-1)
  integer(4) :: i

  print *,"scale ",xmin," ",xmax," ",-0.8," ",0.8
  print *, "data 1 points 1.0 0.0 0.0 ",n
  do i=0,n-1
     x = i*dx+xmin
     print *,x," ",sin(x)
  end do
end program xytest
