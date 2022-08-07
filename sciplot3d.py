# sciplot3d.py: 3D plot programs by Ralph Trenkler.

import os

class Surfplt:
    """
    This is the python class for the call of the scientific plots 3d program
    'surfplt'. The surface is passed by a matrix.
    """
    def __init__(self, mat, nx, ny):
        self.mat = mat
        self.nx, self.ny = nx, ny
        self.box_x, self.box_y, self.box_z = 1.0, 1.0, 1.0
        self.xmin, self.xmax, self.ymin, self.ymax = 0.0, 1.0, 0.0, 1.0
        self.x_label, self.y_label, self.z_label = "x", "y", "z"

    def box(self, x, y, z):
        self.box_x, self.box_y, self.box_z = x, y, z

    def scale(self, xmin, xmax, ymin, ymax):
        self.xmin, self.xmax, self.ymin, self.ymax = xmin, xmax, ymin, ymax

    def xlabel(self, label):
        self.x_label = label

    def ylabel(self, label):
        self.y_label = label

    def zlabel(self, label):
        self.z_label = label

    def render(self, filename="", spin = False):
        if filename == "":
            if spin:
                cmd = "surfplt -m 1 50"
            else:
                cmd = "surfplt"
            fobj = os.popen(cmd,"w")
        else:
            fobj = open(filename, "w")
        print('xlabel "{}"'.format(self.x_label),file=fobj)
        print('ylabel "{}"'.format(self.y_label),file=fobj)
        print('zlabel "{}"'.format(self.z_label),file=fobj)
        print("box {} {} {}".format(self.box_x, self.box_y, self.box_z), \
              file=fobj)
        print("scale {} {} {} {}".format(self.xmin, self.xmax, self.ymin, \
                                           self.ymax), file=fobj)
        print("data {} {}".format(self.nx, self.ny), file=fobj)
        for i in range(self.ny):
            for j in range(self.nx):
                print(self.mat[i][j], file=fobj)
        fobj.close()

def funplot3d(fun, xmin, xmax, ymin, ymax, npoints = 30, xlabel = "x", \
              ylabel = "y", zlabel = "z", boxratio = (1,1,1), filename = "", \
              spin = False):
    """
    This is the python function for displaying a surface given by a real
    function of two real variables. It uses the class 'Surfplt'.
    """
    arr = []
    for i in range(npoints):
        row = []
        for j in range(npoints):
            x = j*(xmax-xmin)/(npoints-1)+xmin
            y = i*(ymax-ymin)/(npoints-1)+ymin
            row.append(fun(x,y))
        arr.append(row)
    plot = Surfplt(arr,npoints,npoints)
    plot.box(boxratio[0],boxratio[1],boxratio[2])
    plot.scale(xmin,xmax,ymin,ymax)
    plot.xlabel(xlabel)
    plot.ylabel(ylabel)
    plot.zlabel(zlabel)
    plot.render(filename, spin)
            
class Curve:
    """
    This python class describes a list of 3d points, which is a curve in 
    space. It can be made of points or be a line. The color can also be set.
    """

    def __init__(self, points, is_line = True, color = (1,1,1)):
        self.is_line, self.color, self.points = is_line, color, points

    def ch_color(self, color):
        self.color = color

    def ch_is_line(self, is_line):
        assert type(is_line) is bool, \
            "error: Curve.ch_is_line() argument must be bool."
        self.is_line = is_line

    def add_point(self, point):
        assert type(point) is tuple and len(point) == 3, \
            "error: Curve.add_point() argument must be  triple."
        self.points.append(point)

    def set_points(self, points):
        assert type(points) is list, \
            "Error: Curve.set_points(): Arg must be list."
        self.points = points
    
class Xyzplt:
    """
    This is the python class for calling the graphics program 'xyzplt' with
    a list of curves. The calling program of this class must use the 'Curve'
    class for construction of the line.
    """
    def __init__(self):
        self.data = []
        self.box_x, self.box_y, self.box_z = 1.0, 1.0, 1.0
        self.xmin, self.xmax, self.ymin, self.ymax = 0.0, 1.0, 0.0, 1.0
        self.zmin, self.zmax = 0.0, 1.0
        self.x_label, self.y_label, self.z_label = "x", "y", "z"

    def add_curve(self, c):
        assert type(c) == Curve, \
            "Xyzplt.add_curve(): Only a Curve can be added to data."
        self.data.append(c)

    def box(self, x, y, z):
        self.box_x, self.box_y, self.box_z = x, y, z

    def scale(self, xmin, xmax, ymin, ymax, zmin, zmax):
        self.xmin, self.xmax = xmin, xmax
        self.ymin, self.ymax = ymin, ymax
        self.zmin, self.zmax = zmin, zmax

    def xlabel(self, label):
        self.x_label = label

    def ylabel(self, label):
        self.y_label = label

    def zlabel(self, label):
        self.z_label = label

    def render(self, filename = "", spin = False):
        if filename == "":
            if spin:
                cmd = "xyzplt -m 1 50"
            else:
                cmd = "xyzplt"
            fobj = os.popen(cmd,"w")
        else:
            fobj = open(filename, "w")
        print('xlabel "{}"'.format(self.x_label),file=fobj)
        print('ylabel "{}"'.format(self.y_label),file=fobj)
        print('zlabel "{}"'.format(self.z_label),file=fobj)
        print("box {} {} {}".format(self.box_x, self.box_y, self.box_z), \
              file=fobj)
        print("scale {} {} {} {} {} {}".\
              format(self.xmin, self.xmax, self.ymin, self.ymax, \
                     self.zmin, self.zmax), file=fobj)
        print("data {}".format(len(self.data)), file=fobj)
        for curve in self.data:
            if curve.is_line:
                print("line", file=fobj, end=" ")
            else:
                print("points", file=fobj, end=" ")
            print("{} {} {} {}".\
                  format(curve.color[0], curve.color[1], curve.color[2],\
                         len(curve.points)), file = fobj)
            for p in curve.points:
                print("{} {} {}".format(*p), file = fobj)
        fobj.close()       

def paramplot3d(fun, tmin, tmax, npoints = 30, xlabel = "x", ylabel = "y", \
                  zlabel = "z", is_line = True, color = (1,0,0), \
                  scale = (0,1,0,1,0,1), boxratio = (1,1,1), filename = "", \
                spin = False):
    curve = Curve(is_line,color,[])
    dt = (tmax-tmin)/(npoints-1)
    for i in range(npoints):
        point = fun(i*dt+tmin)
        curve.add_point(point)
    plot = Xyzplt()
    plot.add_curve(curve)
    plot.box(*boxratio)
    plot.scale(*scale)
    plot.xlabel(xlabel)
    plot.ylabel(ylabel)
    plot.zlabel(zlabel)
    plot.render(filename, spin)
    
