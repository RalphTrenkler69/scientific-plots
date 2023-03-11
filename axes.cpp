// axes_text.cpp: Draw axes for surfplt(1) and xyzplt(1) with labels.
#include <GL/glut.h>
#include <FTGL/ftgl.h>
#include <cmath>
#include <cfloat>
#include <cstdio>
#include <cstring>
#include <string>
#include "text3d.h"

const char default_font_path[] = FONT;

FTPolygonFont polyfont(default_font_path);

extern "C" void init_ftgl();
extern "C" void draw_axes(void);

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
	  
void xaxis3d(float xmin,float xmax,float yloc,float zloc,float ticlen)
{
  float dd, dm, lmin, lmax, xloc;
  char s[maxlen], format[maxlen];
  text3d label(polyfont);
  axis_partitioning(xmin,xmax,dd,dm,lmin,lmax,format);
  for (float x = lmin; x <= lmax+dd/2; x += dd) {
    xloc = -box[0]+2*box[0]*(x-scaling[0][0])/(scaling[0][1]-scaling[0][0]);
    if (is_multible(x,dm,dd/2.0)) {
      glBegin(GL_LINE_STRIP);
      glVertex3f(xloc,zloc,-yloc);
      glVertex3f(xloc,zloc,-yloc+2*ticlen);
      glEnd();
      sprintf(s, format, x);
      std::string label_str{s};
      label.txt(label_str);
      label.txtpos(right_middle);
      label.pos({xloc,zloc,-yloc+3*ticlen});
      label.xv({0.0,0.0,-label_size});
      label.yv({-label_size,0.0,0.0});
      label.zv({0.0,-label_size,0.0});
      label.render();
    }
    else {
      glBegin(GL_LINE_STRIP);
      glVertex3f(xloc,zloc,-yloc);
      glVertex3f(xloc,zloc,-yloc+ticlen);
      glEnd();
    }
  }
}

void yaxis3d(float ymin,float ymax,float xloc,float zloc,float ticlen)
{
  float dd, dm, lmin, lmax, yloc;
  char s[maxlen], format[maxlen];
  text3d label(polyfont);
  axis_partitioning(ymin,ymax,dd,dm,lmin,lmax,format);
  for (float y = lmin; y <= lmax+dd/2; y += dd) {
    yloc = -box[1]+2*box[1]*(y-scaling[1][0])/(scaling[1][1]-scaling[1][0]);
    if (is_multible(y,dm,dd/2.0)) {
      glBegin(GL_LINE_STRIP);
      glVertex3f(xloc,zloc,-yloc);
      glVertex3f(xloc+2*ticlen,zloc,-yloc);
      glEnd();
      sprintf(s, format, y);
      std::string label_str{s};
      label.txt(label_str);
      label.txtpos(left_middle);
      label.pos({xloc+3*ticlen,zloc,-yloc});
      label.xv({label_size,0.0,0.0});
      label.yv({0.0,0.0,-label_size});
      label.zv({0.0,-label_size,0.0});
      label.render();
    }
    else {
      glBegin(GL_LINE_STRIP);
      glVertex3f(xloc,zloc,-yloc);
      glVertex3f(xloc+ticlen,zloc,-yloc);
      glEnd();
    }
  }
}      

void zaxis3d(float zmin,float zmax,float xloc,float yloc,float ticlen)
{
  float dd, dm, lmin, lmax, zloc;
  char s[maxlen], format[maxlen];
  text3d label(polyfont);
  axis_partitioning(zmin,zmax,dd,dm,lmin,lmax,format);
  for (float z = lmin; z <= lmax+dd/2; z += dd) {
    zloc = -box[2]+2*box[2]*(z-scaling[2][0])/(scaling[2][1]-scaling[2][0]);
    if (is_multible(z,dm,dd/2.0)) {
      glBegin(GL_LINE_STRIP);
      glVertex3f(xloc,zloc,-yloc);
      glVertex3f(xloc-sqrt(2.0)*ticlen,zloc,-yloc+sqrt(2.0)*ticlen);
      glEnd();
      sprintf(s, format, z);
      std::string label_str{s};
      label.txt(label_str);
      label.txtpos(right_middle);
      label.pos({xloc-3*ticlen/sqrtf(2.0f),zloc,-yloc+3*ticlen/sqrtf(2.0f)});
      label.xv({label_size/sqrtf(2.0f),0.0,-label_size/sqrtf(2.0f)});
      label.yv({0.0,label_size,0.0});
      label.zv({0.0,0.0,label_size/sqrtf(2.0f)});
      label.render();
    }
    else {
      glBegin(GL_LINE_STRIP);
      glVertex3f(xloc,zloc,-yloc);
      glVertex3f(xloc-ticlen/sqrtf(2.0f),zloc,-yloc+ticlen/sqrtf(2.0f));
      glEnd();
    }
  }
}      
    
void init_ftgl()
{
  if (polyfont.Error()) {
    
    fprintf(stderr,"error: font '%s' not found.\n",default_font_path);
    exit(1);
  }
}
  
void draw_axes()
{
  
  xaxis3d(scaling[0][0],scaling[0][1],-box[1],-box[2],ticlength);
  yaxis3d(scaling[1][0],scaling[1][1],box[0],-box[2],ticlength);
  zaxis3d(scaling[2][0],scaling[2][1],-box[0],-box[1],ticlength);
  // draw x axis label.
  const float factor = 12.0;
  const float sizefactor = 1.5;
  text3d xlabel_txt(polyfont, xlabel, middle_upper,
		    {0.0,-box[2],box[1]+factor*ticlength},
		    {sizefactor*label_size,0.0,0.0},
		    {0.0,0.0,-sizefactor*label_size},
		    {0.0,-sizefactor*label_size,0.0});
  text3d ylabel_txt(polyfont, ylabel, middle_upper,
		    {box[0]+factor*ticlength,-box[2],0.0},
		    {0.0,0.0,-sizefactor*label_size},
		    {-sizefactor*label_size,0.0,0.0},
		    {0.0,-sizefactor*label_size,0.0});
  text3d zlabel_txt(polyfont, zlabel, middle_lower,
		    {-box[0]-factor*ticlength/sqrtf(2.0f),0.0,
		     box[1]+factor*ticlength/sqrtf(2.0f)},
		    {0.0,sizefactor*label_size,0.0},
		    {-sizefactor*label_size/sqrtf(2.0f),0.0,
		     sizefactor*label_size/sqrtf(2.0f)},
		    {-sizefactor*label_size/sqrtf(2.0f),0.0,
		     -sizefactor*label_size/sqrtf(2.0f)});
  xlabel_txt.render();
  ylabel_txt.render();
  zlabel_txt.render();
}
