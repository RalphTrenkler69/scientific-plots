// xyplt.cpp: Plot 2D curves and points with axes and labels.
#include <GL/glut.h>
#include <iostream>
#include <cmath>
#include <cstdio>
#include <cstring>

extern void init_ftgl(), draw_axes();

const int maxstrlen = 128;
const int maxnlines = 512;

struct pltobject {
  float color[3];
  int   isline;
  int   npoints;
  float (*vertices)[2];
}; 

pltobject lines[maxnlines];

char xlabel[maxstrlen], ylabel[maxstrlen];
float box[2], scaling[2][2];
int width, height,nlines;

void init(void) 
{
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel (GL_FLAT);
}

void displayBox(void)
  {
   glBegin(GL_LINE_STRIP);
     glVertex2f(-box[0],-box[1]);
     glVertex2f(box[0],-box[1]);
     glVertex2f(box[0],box[1]);
     glVertex2f(-box[0],box[1]);
     glVertex2f(-box[0],-box[1]);
   glEnd();
 }

void doScaling(float *a, float *b)
  {
    int k;
    for (k=0; k<2; k++) {
      b[k]=-box[k]+2*box[k]*(a[k]-scaling[k][0])/
	(scaling[k][1]-scaling[k][0]);
    }
  }

void displayData(void)
  {
    int iline,i;
    float vertex[2];
    for (iline=0; iline<nlines; iline++) {
      if (lines[iline].isline) {
	glBegin(GL_LINE_STRIP);
      } else {
	glBegin(GL_POINTS);
      }
      glColor3fv(lines[iline].color);
      for (i=0; i<lines[iline].npoints; i++) {
	doScaling(lines[iline].vertices[i],vertex);
	glVertex2f(vertex[0],vertex[1]);
      }
      glEnd();
    }
  }

void enable_clipping()
{
   GLdouble eqn0[4] = {1.0,0.0,0.0,(double) box[0]};
   GLdouble eqn1[4] = {-1.0,0.0,0.0,(double) box[0]};
   GLdouble eqn2[4] = {0.0,1.0,0.0,(double) box[1]};
   GLdouble eqn3[4] = {0.0,-1.0,0.0,(double) box[1]};
   glClipPlane(GL_CLIP_PLANE0, eqn0);
   glClipPlane(GL_CLIP_PLANE1, eqn1);
   glClipPlane(GL_CLIP_PLANE2, eqn2);
   glClipPlane(GL_CLIP_PLANE3, eqn3);
   glEnable(GL_CLIP_PLANE0);
   glEnable(GL_CLIP_PLANE1);
   glEnable(GL_CLIP_PLANE2);
   glEnable(GL_CLIP_PLANE3);
}

void disable_clipping()
{
  glDisable(GL_CLIP_PLANE0);
  glDisable(GL_CLIP_PLANE1);
  glDisable(GL_CLIP_PLANE2);
  glDisable(GL_CLIP_PLANE3);
}
void reshape(int,int);

void display(void)
{
   glClear (GL_COLOR_BUFFER_BIT);
   glColor3f (1.0, 1.0, 1.0);
   //glLoadIdentity ();             /* clear the matrix */
           /* viewing transformation  */
   /* Draw the surrounding box */
   // reshape(width,height);
   displayBox();
   draw_axes();
   enable_clipping();
   displayData();
   disable_clipping();
   glFlush ();
   glutSwapBuffers();
}

void reshape (int w, int h)
{
   GLdouble aspect, window = 1.5;
   width=w;
   height=h;
   aspect = ((GLdouble) w)/((GLdouble) h);
   glViewport (0, 0, (GLsizei) w, (GLsizei) h); 
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity ();
   if (aspect >= 1.0)
     gluOrtho2D(-window*aspect, window*aspect, -window, window);
   else 
     gluOrtho2D(-window,window,-window/aspect,window/aspect);
   glMatrixMode (GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y)
{
   switch (key) {
      case 27:
         exit(0);
         break;
   }
}

void read_string(FILE *file, char* str,int nmax)
{
  int i;
  char c;
  while ((c = getc(file)) != EOF && isspace(c))
    ;
  if (feof(file)) {
    fprintf(stderr,"error: unexpected end of file.\n");
    exit(1);
  }
  if (c != '\"') {
    fprintf(stderr,"error: double quote expected.\n");
    exit(1);
  }
  for (i = 0; (c = getc(file)) != EOF && c != '\"' && i < nmax-1; i++)
    str[i] = c;
  str[i] = '\0';
  if (feof(file)) {
    fprintf(stderr,"error: unexpected end of file.\n");
    exit(1);
  }
}

void readinput(FILE *file)
  {
    int i,k,iline,scanreturn;
    char token[20];
    while ((scanreturn=fscanf(file,"%s",token))!= EOF) {
      if (0==strcmp(token,"box")) {
	if (2!=fscanf(file,"%f %f",&(box[0]),&(box[1]))) {
	  fprintf(stderr,"error: arg for box missing.\n");
	  exit(1);
	}
      } else if (0==strcmp(token,"scale")) {
	if (4!=fscanf(file,"%f %f %f %f",&(scaling[0][0]),&(scaling[0][1]),
		     &(scaling[1][0]),&(scaling[1][1]))) {
	  fprintf(stderr, "error: arg for scale missing.\n");
	  exit(1);
	}
      } else if (0 == strcmp(token,"xlabel")) {
	read_string(file, xlabel, maxstrlen);
      } else if (0 == strcmp(token,"ylabel")) {
	read_string(file, ylabel, maxstrlen);
      } else if (0==strcmp(token,"data")) {
	    if (1!=fscanf(file,"%d",&nlines)) {
	      fprintf(stderr,"error: arg for data missing.\n");
	      exit(1);
	    }
	    for (iline = 0; (EOF!=fscanf(file,"%s",token)) && (iline<nlines);
		 iline++) {
	      if (0==strcmp(token,"points")) {
	        lines[iline].isline=0;
             } else if (0==strcmp(token,"line")) {
		lines[iline].isline=1;
             } else {
		fprintf(stderr,"error: no 'line' or 'points' directive.\n");
		exit(1);
             }
	     if (4==fscanf(file,"%f %f %f %d",&(lines[iline].color[0]),
		     &(lines[iline].color[1]),
		     &(lines[iline].color[2]),
		     &(lines[iline].npoints))) {
	        lines[iline].vertices=
		  //(float (*)[3]) calloc((size_t) lines[iline].npoints,
		  //		   (size_t) sizeof(float[3]));
                  new float[lines[iline].npoints][2];
	        for (i=0; i<lines[iline].npoints; i++)
		   for (k=0; k<2; k++)
		     if (1 != fscanf(file,"%f",
				     &(lines[iline].vertices[i][k]))) {
		       fprintf(stderr, "xyzplt: float in input expected.\n");
		       exit(1);
		     }
                } else {
		  fprintf(stderr,
			  "error: missing arg for 'points' or 'line'.\n");
		  exit(1);
		}
	     }
      }
    }
  }

int main(int argc, char** argv)
{
   char *filename;
   FILE *infile;
   if (argc > 2) {
     fprintf(stderr,"xyplt: too many arguments.\n");
     exit(1);
   }
   if (argc == 2) {
     filename = argv[1];
     infile = fopen(filename, "r");
   } else
     infile = stdin;
   width=500;
   height=500;
   box[0]=1.0;
   box[1]=1.0;
   scaling[0][0]=0.0;
   scaling[0][1]=1.0;
   scaling[1][0]=0.0;
   scaling[1][1]=1.0;
   strcpy(xlabel,"x");
   strcpy(ylabel,"y");
   readinput(infile);
   init_ftgl();
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
   glutInitWindowSize(width,height); 
   glutInitWindowPosition(100, 100);
   glutCreateWindow(argv[0]);
   init();
   glutDisplayFunc(display); 
   glutReshapeFunc(reshape);
   glutKeyboardFunc(keyboard);
   glutMainLoop();
   return 0;
}
