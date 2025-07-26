/* surfplt.c: Three dim surface plot */

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include "fort_record.h"

#define TRUE 1
#define FALSE 0
#define ROTMIN 20
#define ROTFACTOR 0.4
#define MAXSTRLEN 512
#define FORTSTRLEN 24

#define in_interval(a,b,x)  ((a) <= (x) && (x) <= (b))

struct pltobject {
  float color[4];
  int   isline;
  int   with_blending;
  int   npoints;
  float (*vertices)[3];
}; 

struct plot {
  int size;
  struct pltobject *lines;
};

struct animation {
  int motion;
  int animate;
  int size;
  struct plot *plt;
};

struct fort_record_1 {
  float box[3];
  float scaling[3][2];
  char xlabel[FORTSTRLEN];
  char ylabel[FORTSTRLEN];
  char zlabel[FORTSTRLEN];
  int animate;
};

struct fort_record_2 {
  int num_lines;
};

struct fort_record_3 {
  char isline[8];
  char with_blending[8];
  float color[4];
  int npoints;
};

struct animation anim = {FALSE, FALSE, 1, (void *) NULL};

float camera,spinangle,thetaangle,spinincr,sleeptime,box[3],scaling[3][2];
int width,height,x_mouse,y_mouse,iplot;
char xlabel[MAXSTRLEN], ylabel[MAXSTRLEN], zlabel[MAXSTRLEN];
int fort_input = FALSE;

extern void init_ftgl(), draw_axes();

void init(void) 
{
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel (GL_FLAT);
   iplot = 0;
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
    struct plot *plt = &anim.plt[iplot];
    for (iline=0; iline<plt->size; iline++) {
      if (plt->lines[iline].isline) {
	glBegin(GL_LINE_STRIP);
      } else {
	glBegin(GL_POINTS);
      }
      glColor4fv(plt->lines[iline].color);
      for (i=0; i<plt->lines[iline].npoints; i++) {
	doScaling(plt->lines[iline].vertices[i],vertex);
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
   enable_clipping();
   displayData();
   disable_clipping();
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

void idleDisplay(void)
  {
    if (anim.motion) {
      spinangle+=spinincr;
      if (spinangle > 360.0) 
        spinangle-=360.0;
    }
    if (anim.animate)
      iplot = (iplot + 1) % anim.size;
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

void read_plot(FILE *file, struct plot *plt)
{
  int iline,i,k;
  char token[MAXSTRLEN];
  
  for (iline = 0; iline < plt->size &&
	 1 == fscanf(file,"%s",token); iline++) {
      if (0==strcmp(token,"points")) {
	plt -> lines[iline].isline=FALSE;
  plt -> lines[iline].with_blending=FALSE;
      } else if (0==strcmp(token,"points_a")) {
  plt -> lines[iline].isline=FALSE;
  plt -> lines[iline].with_blending=TRUE;
     } else if (0==strcmp(token,"line")) {
	plt -> lines[iline].isline=TRUE;
  plt -> lines[iline].with_blending=FALSE;
     } else if (0==strcmp(token,"line_a")) {
	plt -> lines[iline].isline=TRUE;
  plt -> lines[iline].with_blending=TRUE;
     } else {
	fprintf(stderr,"xyzplt: no 'line' or 'points' directive.\n");
	exit(1);
     }
      if (plt -> lines[iline].with_blending) {
          /* With blending. */
          if (5==fscanf(file,"%f %f %f %f %d",
                  &(plt->lines[iline].color[0]),
            &(plt->lines[iline].color[1]),
            &(plt->lines[iline].color[2]),
            &(plt->lines[iline].color[3]),
            &(plt->lines[iline].npoints))) {
              if (!in_interval(0.0,1.0,plt->lines[iline].color[0])||
                  !in_interval(0.0,1.0,plt->lines[iline].color[1])||
                  !in_interval(0.0,1.0,plt->lines[iline].color[2])||
                  !in_interval(0.0,1.0,plt->lines[iline].color[3])) {
                    fprintf(stderr,
                      "xyzplt: color value not in range [0.0...1.0].\n");
                    exit(1);
              }
        plt->lines[iline].vertices=
          (float (*)[3]) calloc((size_t) plt->lines[iline].npoints,
              (size_t) sizeof(float[3]));
        for (i=0; i<plt->lines[iline].npoints; i++)
          for (k=0; k<3; k++)
            if (1 != fscanf(file,"%f",
                &(plt->lines[iline].vertices[i][k]))) {
              fprintf(stderr, "xyzplt: float in input expected.\n");
              exit(1);
            }
        } else {
        fprintf(stderr,
          "xyzplt: missing arg for 'points_a'.\n");
        exit(1);
        }
      } else {
        /* no blending */
          if (4==fscanf(file,"%f %f %f %d",
                  &(plt->lines[iline].color[0]),
            &(plt->lines[iline].color[1]),
            &(plt->lines[iline].color[2]),
            &(plt->lines[iline].npoints))) {
              if (!in_interval(0.0,1.0,plt->lines[iline].color[0])||
                  !in_interval(0.0,1.0,plt->lines[iline].color[1])||
                  !in_interval(0.0,1.0,plt->lines[iline].color[2])) {
                    fprintf(stderr,
                      "xyzplt: color value not in range [0.0...1.0].\n");
                    exit(1);
              }
        plt -> lines[iline].color[3]=1.0; /* opaque */
        plt->lines[iline].vertices=
          (float (*)[3]) calloc((size_t) plt->lines[iline].npoints,
              (size_t) sizeof(float[3]));
        for (i=0; i<plt->lines[iline].npoints; i++)
          for (k=0; k<3; k++)
            if (1 != fscanf(file,"%f",
                &(plt->lines[iline].vertices[i][k]))) {
              fprintf(stderr, "xyzplt: float in input expected.\n");
              exit(1);
            }
        } else {
        fprintf(stderr,
          "xyzplt: missing arg for 'points' or 'line'.\n");
        exit(1);
        }
      }
     }
}
  

void readinput(FILE *file)
  {
    int scanreturn,iplt;
    char token[MAXSTRLEN];
    char c;
    int data_seen = FALSE;
    anim.animate = FALSE;
    while ((scanreturn=fscanf(file,"%s",token))!= EOF && scanreturn == 1) {
      if (token[0] == '#') {
	while ((c = getc(file)) != EOF && c != '\n') ;
      } else if (0==strcmp(token,"box")) {
	if (3!=fscanf(file,"%f %f %f",&(box[0]),&(box[1]),&(box[2]))) {
	  fprintf(stderr,"xyzplt: arg for 'box' missing.\n");
	  exit(1);
	}
      } else if (0==strcmp(token,"scale")) {
	if (6!=fscanf(file,"%f %f %f %f %f %f",
		      &(scaling[0][0]),&(scaling[0][1]),
		     &(scaling[1][0]),&(scaling[1][1]),
		     &(scaling[2][0]),&(scaling[2][1]))) {
	  fprintf(stderr, "xyzplt: arg for 'scale' missing.\n");
	  exit(1);
	}
      } else if (0 == strcmp(token,"xlabel")) {
	read_string(file,xlabel,MAXSTRLEN);
      } else if (0 == strcmp(token,"ylabel")) {
	read_string(file,ylabel,MAXSTRLEN);
      } else if (0 == strcmp(token,"zlabel")) {
	read_string(file,zlabel,MAXSTRLEN);
      } else if (0 == strcmp(token,"animate")) {
	anim.animate = TRUE;
	if (1 != fscanf(file, "%d",&anim.size) || anim.size <= 0) {
	  fprintf(stderr,
	    "xyzplt: positive integer after 'animate' keyword expected.\n");
	  exit(1);
	} else {
	  anim.plt = (struct plot *) calloc((size_t) anim.size,
					     sizeof(struct plot));
	  if (anim.plt == NULL) {
	    fprintf(stderr,"xyzplt: unable to allocate memory.\n");
	    exit(2);
	  }
          for (iplt = 0; iplt < anim.size; iplt++) {
	    if ((1 != fscanf(file, "%s", token)) ||
		(0 != strcmp(token,"data"))) {
	      fprintf(stderr,
		      "xyzplt: error reading file, 'data' expected.\n");
	      exit(1);
	    }
	    if (1 != fscanf(file,"%d",&anim.plt[iplt].size) ||
			    anim.plt[iplt].size <= 0) {
		fprintf(stderr,"xyzplt: pos integer after 'data' expected.\n");
		exit(1);
	    }
	    anim.plt[iplt].lines = (struct pltobject *)
	       calloc((size_t) anim.plt[iplt].size, sizeof(struct pltobject));
	    if (anim.plt[iplt].lines == NULL) {
	       fprintf(stderr,"xyzplt: unable to allocate memory.\n");
	       exit(2);
	    }
	    read_plot(file, &anim.plt[iplt]);
            data_seen = TRUE;
	  }
      }
      } else if (0 == strcmp(token, "data")) {
	anim.animate = FALSE;
        anim.size = 1;
        anim.plt = (struct plot *) calloc((size_t) 1,
					   sizeof(struct plot));
	if (anim.plt == NULL) {
	  fprintf(stderr, "xyzplt: unable to allocate memory.\n");
	  exit(2);
	}
	if (1 != fscanf(file, "%d", &anim.plt[0].size) ||
	    anim.plt[0].size <= 0) {
          fprintf(stderr,"data size = %d\n",anim.plt[0].size);
	  fprintf(stderr, "xyzplt: pos integer after 'data' expected.\n");
	  exit(1);
	}
        anim.plt[0].lines = (struct pltobject *)
	       calloc((size_t) anim.plt[0].size, sizeof(struct pltobject));
        if (anim.plt[0].lines == NULL) {
          fprintf(stderr,"xyzplt: unable to allocate memory.\n");
          exit(2);
        }
        read_plot(file, &anim.plt[0]);
        data_seen = TRUE;
      } else { /* unkown token encountered */
	fprintf(stderr,"xyzplt: unkown token '%s'.\n",token);
	exit(1);
      }
  }
    if (!data_seen) {
      fprintf(stderr,"xyzplt: no data read in.\n");
      exit(1);
    }
}

void read_fort_input(FILE *file) {
  struct fort_record_1 rec1;
  struct fort_record_2 rec2;
  struct fort_record_3 rec3;
  int i, j, k;

  if (fread_fort_record(file, &rec1, sizeof(rec1)) != 0) {
    fprintf(stderr,"xyzplt: error reading Fortran record 1.\n");
    exit(1);
  }
  for (i = 0; i < 3; i++) {
    box[i] = rec1.box[i];
    for (j = 0; j < 2; j++) {
      scaling[i][j] = rec1.scaling[i][j];
    }
  }
  fort_str_to_c(rec1.xlabel, FORTSTRLEN);
  fort_str_to_c(rec1.ylabel, FORTSTRLEN);
  fort_str_to_c(rec1.zlabel, FORTSTRLEN);
  strncpy(xlabel, rec1.xlabel, FORTSTRLEN);
  strncpy(ylabel, rec1.ylabel, FORTSTRLEN);
  strncpy(zlabel, rec1.zlabel, FORTSTRLEN);
  
  if (rec1.animate > 0) {
    anim.animate = TRUE;
    anim.size = rec1.animate;
  } else if (rec1.animate == 0) {
    anim.animate = FALSE;
    anim.size = 1;
  } else {
    fprintf(stderr,
      "xyzplt: invalid animate value %d in fortran bin file.\n", rec1.animate);
    exit(1);
  }
  if (!(anim.plt = (struct plot *) calloc((size_t) anim.size,
               sizeof(struct plot)))) {
    fprintf(stderr,"xyzplt: unable to allocate memory.\n");
    exit(2);
  }
  for (i = 0; i < anim.size; i++) {
    if (fread_fort_record(file, &rec2, sizeof(rec2)) != 0) {
      fprintf(stderr,"xyzplt: error reading Fortran record 2.\n");
      exit(1);
    }
    anim.plt[i].size = rec2.num_lines;
    if (anim.plt[i].size <= 0) {
      fprintf(stderr,"xyzplt: invalid number of lines %d in fortran bin file.\n",
              anim.plt[i].size);
      exit(1);
    }
    if (!(anim.plt[i].lines = (struct pltobject *)
          calloc((size_t) anim.plt[i].size, sizeof(struct pltobject)))) {
      fprintf(stderr,"xyzplt: unable to allocate memory.\n");
      exit(2);
    }
    for (j = 0; j < anim.plt[i].size; j++) {
      if (fread_fort_record(file, &rec3, sizeof(rec3)) != 0) {
        fprintf(stderr,"xyzplt: error reading Fortran record 3.\n");
        exit(1);
      }
      fort_str_to_c(rec3.isline, 8);
      fort_str_to_c(rec3.with_blending, 8);
      switch ((char) toupper(rec3.isline[0])) {
        case 'P':
          anim.plt[i].lines[j].isline = FALSE;
          break;
        case 'L':
          anim.plt[i].lines[j].isline = TRUE;
          break;
        default:
          fprintf(stderr,"xyzplt: unknown line type '%s'.\n", rec3.isline);
          exit(1);
      }
      switch ((char) toupper(rec3.with_blending[0])) {
        case 'N':
          anim.plt[i].lines[j].with_blending = FALSE;
          break;
        case 'A':
        case 'Y':
          anim.plt[i].lines[j].with_blending = TRUE;
          break;
        default:
          fprintf(stderr,"xyzplt: unknown blending type '%s'.\n", rec3.with_blending);
          exit(1);
      }
      for (k = 0; k < 4; k++) {
        if (!in_interval(0.0, 1.0, rec3.color[k])) {
          fprintf(stderr,"xyzplt: binary fort file: color value not in range [0.0...1.0].\n");
          exit(1);
        }
        anim.plt[i].lines[j].color[k] = rec3.color[k];
        if (k == 3 && !anim.plt[i].lines[j].with_blending) 
          anim.plt[i].lines[j].color[k] = 1.0; /* opaque */
      }
      anim.plt[i].lines[j].npoints = rec3.npoints;
      if (rec3.npoints <= 0) {
        fprintf(stderr,"xyzplt: invalid number of points %d in fortran bin file.\n",
                rec3.npoints);
        exit(1);
      } 
      if (!(anim.plt[i].lines[j].vertices =
            (float (*)[3]) calloc((size_t) rec3.npoints,
                                  sizeof(float[3])))) {
        fprintf(stderr,"xyzplt: unable to allocate memory.\n");
        exit(2);
      }
      if (fread_fort_record(file,(void *) anim.plt[i].lines[j].vertices,
        3*rec3.npoints*sizeof(float)) != 0) {
        fprintf(stderr,"xyzplt: error reading vertices.\n");
        exit(1);
      }
    }
  }
}

int main(int argc, char** argv)
{
   int argi;
   char *filename = "";
   FILE *infile;
   int read_from_file = FALSE;
   width=500;
   height=500;
   spinangle=-45.0;
   thetaangle = 0.0;
   x_mouse = width/2;
   y_mouse = height/2;
   anim.motion=FALSE;
   sleeptime = 40.0;
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
	  fprintf(stderr,"xyzplt: error parsing command line.\n");
	  exit(1);
	}
	if (spinincr>360.0 || spinincr<-360.0) {
	  fprintf(stderr,"xyzplt: warning: strange increment angle.\n");
	}
	if (1!=sscanf(argv[++argi],"%f",&sleeptime)) {
	  fprintf(stderr,"xyzplt: error parsing command line.\n");
	}
	anim.motion=TRUE;
      } else if (0 == strcmp(argv[argi],"-c")) {
	if (1 != sscanf(argv[++argi],"%f", &camera)) {
	  fprintf(stderr,"xyzplt: float in args expected.\n");
	  exit(1);
	}
      } else if (0 == strcmp(argv[argi],"-s")) {
	if (1 != sscanf(argv[++argi],"%f",&sleeptime) || sleeptime < 0.0) {
	  fprintf(stderr,"xyzplt: float in args expected.\n");
	  exit(1);
	}
      } else if (0 == strcmp(argv[argi],"-f")) {
        fort_input = TRUE;
      } else if (0==strcmp(argv[argi],"-h")) {
printf("usage: surfplt [-h] [-c <height>] [-m <angle> <sleep>] [-s <sleep>>] <file>\n");
	printf("   -h   print this help.\n");
	printf("   -c <height>   set camera height.\n");
        printf("   -s <sleeptime>  set sleeptime for animation,\n");
	printf("   -m <angle> <sleep>\n");
	printf("        rotate the surface by increment <angle> every\n");
	printf("        <sleep> milliseconds.\n");
  printf("   -s <sleep> set sleeptime for animation,\n");
  printf("   -f  read input in Fortran binary format.\n");
	exit(0);
      } else {
	fprintf(stderr, "xyzplt: unknown option %s.\n",argv[argi]);
	exit(1);
      }
   }
   if (read_from_file)
     infile = fopen(filename, (fort_input ? "rb" : "r"));
   else if (fort_input) {
     fprintf(stderr,"xyzplt: no fortran binary input file specified.\n");
     exit(1);
   } else
     infile = stdin;
   if (!infile) {
     fprintf(stderr, "xyzplt: cannot open file '%s'.\n",filename);
     exit(3);
   }
   if (fort_input)
      read_fort_input(infile);
   else 
      readinput(infile);
   fclose(infile);
   init_ftgl();
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
   glutInitWindowSize (width,height); 
   glutInitWindowPosition (100, 100);
   glutCreateWindow (argv[0]);
   init ();
   glutDisplayFunc(display); 
   glutReshapeFunc(reshape);
   glutKeyboardFunc(keyboard);
   if (!anim.motion) {
     glutMouseFunc(mouse);
     glutMotionFunc(mouseMotion);
   }
   if (anim.motion || anim.animate) 
     glutIdleFunc(idleDisplay);
   glutMainLoop();
   return 0;
}
