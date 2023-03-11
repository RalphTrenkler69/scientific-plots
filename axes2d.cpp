// axes_text.cpp: Draw axes for surfplt(1) and xyzplt(1) with labels.
#include <iostream>
#include <GL/glut.h>
#include <FTGL/ftgl.h>
#include <cmath>
#include <cfloat>
#include <cstdio>
#include <string>
#include "text3d.h"

const char font_path[] = FONT;
FTPolygonFont polyfont(font_path);

const int maxlen = 512;
const float label_size = 0.12;
const float ticlength = 0.05;

extern float box[3], scaling[3][2];
extern char xlabel[], ylabel[], zlabel[];

inline int ilog10(float x)
{
  if (abs(x) < 100*FLT_MIN)
    return 0;
  else
    return static_cast<int>(floor(log10(abs(x))));
}

inline int Max(int a, int b)
{
  return (a>=b) ? a : b;
}

bool is_multible(float a, float b,float p)
{
  float remainder = abs(fmod(a,b));
  return (remainder < abs(p)) || ((b-remainder) < abs(p));
}

void axis_partitioning(float min, float max, float& dd, float& ddm,
		       float& lmin, float& lmax, char* format)
{
  const float ddwall1 = 2.0, ddwall2 = 4.0;
  float d, ed, md;
  d = max - min; // compute difference d.
  ed = ilog10(d); // find the exponent of d.
  md = d / pow(10.0,ed); // find mantissa of d.
  if (md < ddwall1)
    dd = pow(10.0,ed-1.0);
  else if (md < ddwall2)
    dd = pow(10.0,ed) / 2.0;
  else
    dd = pow(10.0,ed);
  if (md < ddwall1)
    ddm = pow(10.0,ed) / 2.0;
  else
    ddm = pow(10.0,ed);
  lmin = ceil(min/dd)*dd;
  lmax = floor(max/dd)*dd;
  sprintf(format,"%%.%dG",
	  1+Max(static_cast<int>(abs(ilog10(min) - ilog10(ddm))),
	        static_cast<int>(abs(ilog10(max) - ilog10(ddm)))));
}
	  
void xaxis2d(float xmin,float xmax,float yloc,float ticlen)
{
  float dd, dm, lmin, lmax, xloc;
  char s[maxlen], format[maxlen];
  text3d label(polyfont);
  axis_partitioning(xmin,xmax,dd,dm,lmin,lmax,format);
  for (float x = lmin; x <= lmax+dd/2; x += dd) {
    xloc = -box[0]+2*box[0]*(x-scaling[0][0])/(scaling[0][1]-scaling[0][0]);
    if (is_multible(x,dm,dd/2.0)) {
      glBegin(GL_LINE_STRIP);
      glVertex2f(xloc,yloc);
      glVertex2f(xloc,yloc-2*ticlen);
      glEnd();
      sprintf(s, format, x);
      std::string label_str{s};
      label.txt(label_str);
      label.txtpos(middle_upper);
      label.pos({xloc,yloc-3*ticlen});
      label.xv({label_size,0.0,0.0});
      label.yv({0.0,label_size,0.0});
      label.zv({0.0,0.0,label_size});
      label.render();
    }
    else {
      glBegin(GL_LINE_STRIP);
      glVertex2f(xloc,yloc);
      glVertex2f(xloc,yloc-ticlen);
      glEnd();
    }
  }
}

void yaxis2d(float ymin,float ymax,float xloc,float ticlen)
{
  float dd, dm, lmin, lmax, yloc;
  char s[maxlen], format[maxlen];
  text3d label(polyfont);
  axis_partitioning(ymin,ymax,dd,dm,lmin,lmax,format);
  for (float y = lmin; y <= lmax+dd/2; y += dd) {
    yloc = -box[1]+2*box[1]*(y-scaling[1][0])/(scaling[1][1]-scaling[1][0]);
    if (is_multible(y,dm,dd/2.0)) {
      glBegin(GL_LINE_STRIP);
      glVertex2f(xloc,yloc);
      glVertex2f(xloc-2*ticlen,yloc);
      glEnd();
      sprintf(s, format, y);
      std::string label_str{s};
      label.txt(label_str);
      label.txtpos(right_middle);
      label.pos({xloc-3*ticlen,yloc});
      label.xv({label_size,0.0,0.0});
      label.yv({0.0,label_size,0.0});
      label.zv({0.0,0.0,label_size});
      label.render();
    }
    else {
      glBegin(GL_LINE_STRIP);
      glVertex2f(xloc,yloc);
      glVertex2f(xloc-ticlen,yloc);
      glEnd();
    }
  }
}      

void init_ftgl()
{
  if (polyfont.Error()) {
    
    std::cerr << "error: font '" << font_path << "' not found.\n";
    exit(1);
  }
}
  
void draw_axes()
{
  
  xaxis2d(scaling[0][0],scaling[0][1],-box[1],ticlength);
  yaxis2d(scaling[1][0],scaling[1][1],-box[0],ticlength);
  // draw x axis label.
  const float factor = 8.0;
  const float sizefactor = 1.5;
  text3d xlabel_txt(polyfont, xlabel, middle_upper,
		    {0.0,-box[1]-factor*ticlength,0.0},
		    {sizefactor*label_size,0.0,0.0},
		    {0.0,sizefactor*label_size,0.0},
		    {0.0,0.0,sizefactor*label_size});
  text3d ylabel_txt(polyfont, ylabel, middle_lower,
		    {-box[0]-factor*ticlength,0.0,0.0},
		    {0.0,sizefactor*label_size,0.0},
		    {-sizefactor*label_size,0.0,0.0},
		    {0.0,0.0,sizefactor*label_size});
  xlabel_txt.render();
  ylabel_txt.render();
}
