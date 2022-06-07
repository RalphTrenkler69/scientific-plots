// xyplt.cpp: Plot 2D curves and points with axes and labels.
#include <GL/glut.h>
#include <iostream>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <unistd.h>

extern void init_ftgl(), draw_axes();

const int maxstrlen = 128;

struct pltobject {
  float color[3];
  bool  isline;
  int   npoints;
  float (*vertices)[2];
}; 

struct plot {
  int size;
  pltobject *lines;
};

struct animation {
  bool animate;
  int size;
  plot *plt;
};

animation anim = {false, 1, nullptr};
  
char xlabel[maxstrlen], ylabel[maxstrlen];
float box[2], scaling[2][2], sleeptime;
int width, height, iplot;

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
    for (iline=0; iline<anim.plt[iplot].size; iline++) {
      if (anim.plt[iplot].lines[iline].isline) {
	glBegin(GL_LINE_STRIP);
      } else {
	glBegin(GL_POINTS);
      }
      glColor3fv(anim.plt[iplot].lines[iline].color);
      for (i=0; i<anim.plt[iplot].lines[iline].npoints; i++) {
	doScaling(anim.plt[iplot].lines[iline].vertices[i],vertex);
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

void idleDisplay(void)
{
  if (anim.animate)
     iplot = (iplot + 1) % anim.size;
  usleep((unsigned long) sleeptime*1000);
  glutPostRedisplay();
}

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

void read_plot(FILE *file, plot *plt)
{
    int iline, i, k;
    char token[maxstrlen];

    for (iline = 0; (iline < plt->size) && (1 == fscanf(file,"%s",token));
	 iline++) {
      if (0==strcmp(token,"points")) {
	plt->lines[iline].isline = false;
     } else if (0==strcmp(token,"line")) {
	plt->lines[iline].isline = true;
     } else {
	fprintf(stderr,"error: no 'line' or 'points' directive.\n");
	exit(1);
     }
     if (4==fscanf(file,"%f %f %f %d",&(plt->lines[iline].color[0]),
	     &(plt->lines[iline].color[1]),
	     &(plt->lines[iline].color[2]),
	     &(plt->lines[iline].npoints))) {
       // fprintf(stderr,"reading '%s' block,\n", token);
	plt->lines[iline].vertices=
	  //(float (*)[3]) calloc((size_t) lines[iline].npoints,
	  //		   (size_t) sizeof(float[3]));
	  new float[plt->lines[iline].npoints][2];
	for (i=0; i<plt->lines[iline].npoints; i++)
	   for (k=0; k<2; k++)
	     if (1 != fscanf(file,"%f",
			     &(plt->lines[iline].vertices[i][k]))) {
	       fprintf(stderr, "xyzplt: float in input expected.\n");
	       exit(1);
	     }
        //fprintf(stderr,"%d x %d floats read.\n", plt->lines[iline].npoints,
	//	2);
	} else {
	  fprintf(stderr,
		  "error: missing arg for 'points' or 'line'.\n");
	  exit(1);
	}
     }
}
    

void readinput(FILE *file)
  {
    int scanreturn,iplt;
    char token[maxstrlen];
    char c;
    bool data_seen = false;
    anim.animate = false;
    while ((scanreturn=fscanf(file,"%s",token))!= EOF && scanreturn == 1) {
      if (token[0] == '#') {
	while ((c = getc(file)) != EOF && c != '\n') ;
      } else if (0==strcmp(token,"box")) {
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
	anim.animate = false;
	anim.size = 1;
	anim.plt = new plot[1];
	if (anim.plt == nullptr) {
	  fprintf(stderr,"xyplt: unable to allocate memory.\n");
          exit(2);
	}
        if (1!=fscanf(file,"%d",&anim.plt[0].size) ||
	    anim.plt[0].size <= 0) {
            fprintf(stderr,"xyplt: pos integer after 'data' expected.\n");
            exit(1);
        }
        anim.plt[0].lines = new pltobject[anim.plt[0].size];
	if (anim.plt[0].lines == nullptr) {
	  fprintf(stderr,"xyplt: unable to allocate memory.\n");
	  exit(2);
	}
	read_plot(file,&anim.plt[0]);
        data_seen = true;
        break;
      } else if (0 == strcmp(token,"animate")) {
        anim.animate = true;
	if (1 != fscanf(file, "%d", &anim.size) || anim.size <= 0) {
	  fprintf(stderr,
		  "xyplt: pos integer after 'animate' expected.\n");
	  exit(1);
	}
        //fprintf(stderr,"anim.size = %d.\n",anim.size);
	anim.plt = new plot[anim.size];
	if (anim.plt == nullptr) {
	  fprintf(stderr, "xyplt: can't allocate memory.\n");
	  exit(2);
	}
	for (iplt = 0; iplt < anim.size; iplt++) {
          //fprintf(stderr,"iplt = %d\n",iplt);
	  if ((1 != fscanf(file, "%s", token)) ||
	      (0 != strcmp(token,"data"))) {
	    fprintf(stderr,
  "xyplt: error reading file, 'data' expected, '%s' found.\n",token);
	    exit(1);
	  }
	  if (1 != fscanf(file, "%d", &anim.plt[iplt].size) ||
	      anim.plt[iplt].size <= 0) {
	    fprintf(stderr,"xyplt: pos int after 'data' expected.\n");
	    exit(1);
	  }
	  anim.plt[iplt].lines = new pltobject[anim.plt[iplt].size];
	  if (anim.plt[iplt].lines == nullptr) {
	    fprintf(stderr, "xyplt: can't allocate memory.\n");
	    exit(2);
	  }
	  read_plot(file, &anim.plt[iplt]);
	  data_seen = true;
        }
        break;
       } else {
	fprintf(stderr,"xyplt: unkown token '%s'.\n",token);
	exit(1);
      }
    }
    if (!data_seen) {
      fprintf(stderr,"xyplt: no data read in.\n");
      exit(1);
    }
  }

int main(int argc, char** argv)
{
   char *filename;
   FILE *infile = stdin;
   bool read_from_file = false;
   sleeptime = 100.0;
   for (int argi = 1; argi < argc; argi++) {
     if (*(argv[argi]) != '-' ) {
       if (!read_from_file) {
         filename = argv[argi];
         infile = fopen(filename, "r");
         if (infile == nullptr) {
	   fprintf(stderr,"xyplt: cannot open file '%s'.\n",filename);
	   exit(3);
	 }
	 read_from_file = true;
       } else {
	 fprintf(stderr, "xyplt: too many filename args on cmd line.\n");
	 exit(1);
       }
     } else if (0 == strcmp(argv[argi],"-s")) {
       if ((1 != sscanf(argv[++argi],"%f",&sleeptime)) ||
	   (sleeptime < 0.0)) {
	 fprintf(stderr,"xyplt: no pos float after '-s' in cmd line.\n");
	 exit(1);
       }
     } else {
       fprintf(stderr, "xyplt: unkown cmd line option '%s'.\n",argv[argi]);
       exit(1);
     }
   }
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
   iplot = 0;
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
   if (anim.animate)
     glutIdleFunc(idleDisplay);
   glutMainLoop();
   return 0;
}
