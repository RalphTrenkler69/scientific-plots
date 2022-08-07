  ! xyztest.f03: Generate test data for xyzplt.

program xyztest
  real(4), parameter :: pi = 3.141592654
  real(4) :: t, lambda
  integer(4), parameter :: n = 10000, l = 10000
  integer(4) :: i,j

  print *,"scale",-1.0,1.0,-1.0,1.0,-1.0,1.0
  print *,"animate", n
  do i=0,n-1
     lambda = real(i)/(n-1)
     print *,"data",1
     print *,"line",1.0,1.0,0.0,l
     do j=0,l-1
        t = 2*pi*real(j)/(l-1)
        print *,(1.0-lambda)*cos(t)+lambda*sin(t)
        print *,(1.0-lambda)*sin(t)
        print *,lambda*cos(t)
     end do
  end do
end program
