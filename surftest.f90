program surftest
  integer, parameter :: nx=100,ny=100,n = 100
  real, parameter :: xmin=-9.0,xmax=9.0,ymin=-9.0,ymax=9.0
  real :: x,y,lambda
  integer :: ix,iy,i

  print *,"animate ",n
  do i=0,n-1
     lambda = real(i)/(n-1)
     print *,"data ",nx," ",ny
     do iy=0,ny-1
        do ix=0,nx-1
           x=xmin+ix*(xmax-xmin)/(nx-1)
           y=ymin+iy*(ymax-ymin)/(ny-1)
           print *,(1-lambda)*f(x,y)+lambda*g(x,y)
        end do
     end do
   end do
contains
  real function f(x,y)
    real, intent(in) :: x,y
    real :: r
    r = sqrt(x*x+y*y)
    f = sin(r)/r
  end function f
  real function g(x,y)
    real, intent(in) :: x,y
    g = sin(x)*sin(y/2)
  end function g
end program surftest
