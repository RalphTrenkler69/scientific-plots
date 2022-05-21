/* surfplt.c: Three dim surface plot */

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#define TRUE 1
#define FALSE 0
#define ROTMIN 20
#define ROTFACTOR 0.4
#define NLINES 262144 
#define MAXSTRLEN 512


typedef struct {
  float color[3];
  int   isline;
  int   npoints;
  float (*vertices)[3];
} pltobject; 

pltobject lines[NLINES];

float camera,spinangle,thetaangle,spinincr,sleeptime,box[3],scaling[3][2];
int width,height,nlines,x_mouse,y_mouse;
char xlabel[MAXSTRLEN], ylabel[MAXSTRLEN], zlabel[MAXSTRLEN];

int motion;

GLuint dlist_base;

extern void init_ftgl(), draw_axes();

void init(void) 
{
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel (GL_FLAT);
}

void displayBox(void)
  {
   glBegin(GL_LINE_STRIP);
     glVertex3f(-box[0],-box[2],-box[1]);
     glVertex3f(box[0],-box[2],-box[1]);
     glVertex3f(box[0],-box[2],box[1]);
     glVertex3f(-box[0],-box[2],box[1]);
     glVertex3f(-box[0],-box[2],-box[1]);
   glEnd();
   glBegin(GL_LINE_STRIP);
     glVertex3f(-box[0],box[2],-box[1]);
     glVertex3f(box[0],box[2],-box[1]);
     glVertex3f(box[0],box[2],box[1]);
     glVertex3f(-box[0],box[2],box[1]);
     glVertex3f(-box[0],box[2],-box[1]);
   glEnd();
   glBegin(GL_LINES);
     glVertex3f(-box[0],box[2],-box[1]);
     glVertex3f(-box[0],-box[2],-box[1]);
     glVertex3f(box[0],box[2],-box[1]);
     glVertex3f(box[0],-box[2],-box[1]);
     glVertex3f(box[0],box[2],box[1]);
     glVertex3f(box[0],-box[2],box[1]);
     glVertex3f(-box[0],box[2],box[1]);
     glVertex3f(-box[0],-box[2],box[1]);
   glEnd();
 }

void doScaling(float *a, float *b)
  {
    int k;
    for (k=0; k<3; k++) {
      b[k]=-box[k]+2*box[k]*(a[k]-scaling[k][0])/
	(scaling[k][1]-scaling[k][0]);
    }
  }

void displayData(void)
  {
    int iline,i;
    float vertex[3];
    for (iline=0; iline<nlines; iline++) {
      if (lines[iline].isline) {
	glBegin(GL_LINE_STRIP);
      } else {
	glBegin(GL_POINTS);
      }
      glColor3fv(lines[iline].color);
      for (i=0; i<lines[iline].npoints; i++) {
	doScaling(lines[iline].vertices[i],vertex);
	glVertex3f(vertex[0],vertex[2],-vertex[1]);
      }
      glEnd();
    }
  }

void enable_clipping()
{
   GLdouble eqn0[4] = {1.0,0.0,0.0,(double) box[0]};
   GLdouble eqn1[4] = {-1.0,0.0,0.0,(double) box[0]};
   GLdouble eqn2[4] = {0.0,1.0,0.0,(double) box[2]};
   GLdouble eqn3[4] = {0.0,-1.0,0.0,(double) box[2]};
   GLdouble eqn4[4] = {0.0,0.0,1.0,(double) box[1]};
   GLdouble eqn5[4] = {0.0,0.0,-1.0,(double) box[1]};
   glClipPlane(GL_CLIP_PLANE0, eqn0);
   glClipPlane(GL_CLIP_PLANE1, eqn1);
   glClipPlane(GL_CLIP_PLANE2, eqn2);
   glClipPlane(GL_CLIP_PLANE3, eqn3);
   glClipPlane(GL_CLIP_PLANE4, eqn4);
   glClipPlane(GL_CLIP_PLANE5, eqn5);
   glEnable(GL_CLIP_PLANE0);
   glEnable(GL_CLIP_PLANE1);
   glEnable(GL_CLIP_PLANE2);
   glEnable(GL_CLIP_PLANE3);
   glEnable(GL_CLIP_PLANE4);
   glEnable(GL_CLIP_PLANE5);
}

void disable_clipping()
{
  glDisable(GL_CLIP_PLANE0);
  glDisable(GL_CLIP_PLANE1);
  glDisable(GL_CLIP_PLANE2);
  glDisable(GL_CLIP_PLANE3);
  glDisable(GL_CLIP_PLANE4);
  glDisable(GL_CLIP_PLANE5);
}

void display(void)
{
   glClear (GL_COLOR_BUFFER_BIT);
   glColor3f (1.0, 1.0, 1.0);
   glLoadIdentity ();             /* clear the matrix */
           /* viewing transformation  */
   gluLookAt (0.0, camera, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
   glRotatef(thetaangle,1.0,0.0,0.0);
   glRotatef(spinangle,0.0,1.0,0.0);
   /* Draw the surrounding box */
   displayBox();
   draw_axes();
   glCallList(dlist_base);
   glFlush ();
   glutSwapBuffers();
}

void reshape (int w, int h)
{
  float aspect, window = 0.9, near = 2.0, far= 8.0;
   width=w;
   height=h;
   aspect = ((float) w)/((float) h);
   glViewport (0, 0, (GLsizei) w, (GLsizei) h); 
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity ();
   if (aspect >= 1.0)
     glFrustum (-window*aspect, window*aspect, -window, window, near, far);
   else 
     glFrustum(-window,window,-window/aspect,window/aspect,near,far);
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

void spinDisplay(void)
  {
    spinangle+=spinincr;
    if (spinangle > 360.0) 
      spinangle-=360.0;
    usleep((unsigned long) sleeptime*1000.0);
    glutPostRedisplay();
  }

void mouse(int button, int state, int x, int y)
{
  if (state == GLUT_DOWN) {
    x_mouse = x;
    y_mouse = y;
  }
}

void mouseMotion(int x, int y)
{
  int deltax = x - x_mouse;
  int deltay = y - y_mouse;

  if (abs(deltax) > ROTMIN || abs(deltay) > ROTMIN) {
    spinangle += ROTFACTOR * deltax;
    thetaangle += ROTFACTOR * deltay;
    x_mouse = x;
    y_mouse = y;
    glutPostRedisplay();
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
  if (i >= nmax-1) {
    fprintf(stderr,"error: input string too long.\n");
    exit(1);
  }
}

void readinput(FILE *file)
  {
    int i,k,iline,scanreturn;
    char token[20];
    while ((scanreturn=fscanf(file,"%s",token))!= EOF) {
      if (0==strcmp(token,"box")) {
	if (3!=fscanf(file,"%f %f %f",&(box[0]),&(box[1]),&(box[2]))) {
	  fprintf(stderr,"error: arg for box missing.\n");
	  exit(1);
	}
      } else if (0==strcmp(token,"scale")) {
	if (6!=fscanf(file,"%f %f %f %f %f %f",
		      &(scaling[0][0]),&(scaling[0][1]),
		     &(scaling[1][0]),&(scaling[1][1]),
		     &(scaling[2][0]),&(scaling[2][1]))) {
	  fprintf(stderr, "error: arg for scale missing.\n");
	  exit(1);
	}
      } else if (0 == strcmp(token,"xlabel")) {
	read_string(file,xlabel,MAXSTRLEN);
      } else if (0 == strcmp(token,"ylabel")) {
	read_string(file,ylabel,MAXSTRLEN);
      } else if (0 == strcmp(token,"zlabel")) {
	read_string(file,zlabel,MAXSTRLEN);
      } else if (0==strcmp(token,"data")) {
	    if (1!=fscanf(file,"%d",&nlines)) {
	      fprintf(stderr,"error: arg for data missing.\n");
	      exit(1);
	    }
            if (nlines > NLINES) {
	      fprintf(stderr,"error: max number of lines is %d.\n",NLINES);
	      exit(2);
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
		  (float (*)[3]) calloc((size_t) lines[iline].npoints,
				   (size_t) sizeof(float[3]));
	        for (i=0; i<lines[iline].npoints; i++)
		   for (k=0; k<3; k++)
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
   int argi;
   char *filename;
   FILE *infile;
   int read_from_file = FALSE;
   width=500;
   height=500;
   spinangle=-45.0;
   thetaangle = 0.0;
   x_mouse = width/2;
   y_mouse = height/2;
   motion=FALSE;
   spinincr=0.1;
   camera = 0.7;
   box[0]=1.0;
   box[1]=1.0;
   box[2]=1.0;
   scaling[0][0]=0.0;
   scaling[0][1]=1.0;
   scaling[1][0]=0.0;
   scaling[1][1]=1.0;
   scaling[2][0]=0.0;
   scaling[2][1]=1.0;
   strcpy(xlabel,"x");
   strcpy(ylabel,"y");
   strcpy(zlabel,"z");
   for (argi=1; argi<argc; argi++) {
      if (*(argv[argi]) != '-') {
	if (!read_from_file) {
	  filename = argv[argi];
          read_from_file = TRUE;
	} else {
	  fprintf(stderr,"xyzplt: too many filename arguments.\n");
	  exit(1);
	}
      } else if (0==strcmp(argv[argi],"-m")) {
	if (1!=sscanf(argv[++argi],"%f",&spinincr)) {
	  fprintf(stderr,"error parsing command line.\n");
	  exit(1);
	}
	if (spinincr>360.0 || spinincr<-360.0) {
	  fprintf(stderr,"warning: strange increment angle.\n");
	}
	if (1!=sscanf(argv[++argi],"%f",&sleeptime)) {
	  fprintf(stderr,"error parsing command line.\n");
	}
	motion=1;
      } else if (0 == strcmp(argv[argi],"-c")) {
	if (1 != sscanf(argv[++argi],"%f", &camera)) {
	  fprintf(stderr,"xyzplt: float in args expected.\n");
	  exit(1);
	}
      } else if (0==strcmp(argv[argi],"-h")) {
printf("usage: surfplt [-h] [-c <height>] [-m <angle> <sleep>] [-display <display>] <file>\n");
	printf("   -h   print this help.\n");
	printf("   -c <height>   set camera height.\n");
	printf("   -m <angle> <sleep>\n");
	printf("        rotate the surface by increment <angle> every\n");
	printf("        <sleep> milliseconds.\n");
	exit(0);
      } else {
	fprintf(stderr, "xyzplt: unknown option %s.\n",argv[argi]);
	exit(1);
      }
   }
   if (read_from_file)
     infile = fopen(filename, "r");
   else
     infile = stdin;
   readinput(infile);
   init_ftgl();
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
   glutInitWindowSize (width,height); 
   glutInitWindowPosition (100, 100);
   glutCreateWindow (argv[0]);
   dlist_base = glGenLists(1);
   glNewList(dlist_base, GL_COMPILE);
   enable_clipping();
   displayData();
   disable_clipping();
   glEndList();
   init ();
   glutDisplayFunc(display); 
   glutReshapeFunc(reshape);
   glutKeyboardFunc(keyboard);
   if (motion) {
      glutIdleFunc(spinDisplay);
   } else {
     glutMouseFunc(mouse);
     glutMotionFunc(mouseMotion);
   }
   glutMainLoop();
   return 0;
}
