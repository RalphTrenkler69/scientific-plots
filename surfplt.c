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
#define MAXSTRLEN 512

extern void draw_axes(void), init_ftgl(void);

#define AVG(f1,f3,dx) (((f3)-(f1))/(2*(dx)))

struct plot {
  int matx, maty;
  float *matrix;
  float (*vertices)[3], (*normals)[3];
};

struct animation {
  int size;
  int animate, motion, render, alpha_blending;
  struct plot *plt;
};

struct animation anim = {1, FALSE, FALSE, TRUE, FALSE, (void *) NULL};

float spinangle,thetaangle,camera,spinincr,sleeptime,box[3],scaling[3][2];
int width,height,x_mouse,y_mouse;
float alpha = 1.0; /* alpha value for transparency */
int iplot = 0;

char xlabel[MAXSTRLEN], ylabel[MAXSTRLEN], zlabel[MAXSTRLEN];
char font_path[MAXSTRLEN] = "";

/* Set ambient and diffuse material color, which is set 
in the function "init()".*/
GLfloat material_color[4] = {1.0,0.3,0.3, 1.0};

void init(void) 
{
   GLfloat mat_specular[] = {1.0,1.0,1.0, 1.0};
   GLfloat mat_shininess[] = {30.0};
   GLfloat light_position[] = {1.0,1.0,1.0,0.0};
   GLfloat white_light[] = {1.0,1.0,1.0, 1.0};
   GLfloat lmodel_ambient[] = {0.5,0.5,0.5, 1.0};
   if (anim.alpha_blending)
      material_color[3] = alpha;
   glClearColor (0.0, 0.0, 0.0, 0.0);
   if (! anim.render) {
     glShadeModel (GL_FLAT);
   } else {
     glShadeModel(GL_SMOOTH);
     glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
     glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
     glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
     glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, material_color);
     glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material_color);
     glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
     glLightfv(GL_LIGHT0, GL_POSITION, light_position);
     glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light);
     glLightfv(GL_LIGHT0, GL_SPECULAR, white_light);
   }
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

void displayWireframe(struct plot *plt)
  {
    int ix,iy;
    glBegin(GL_LINES);
    for (ix=0; ix<plt->matx; ix++)
      for (iy=0; iy<plt->maty-1; iy++) {
	glVertex3f(-box[0]+ix*2*box[0]/((float) (plt->matx-1)),
	   -box[2]+2*box[2]*(plt->matrix[iy*plt->matx+ix]-scaling[2][0])/
		   (scaling[2][1]-scaling[2][0]),
           -box[1]+iy*2*box[1]/((float) (plt->maty-1)));
	glVertex3f(-box[0]+ix*2*box[0]/((float) (plt->matx-1)),
	   -box[2]+2*box[2]*(plt->matrix[(iy+1)*plt->matx+ix]-scaling[2][0])/
		   (scaling[2][1]-scaling[2][0]),
           -box[1]+(iy+1)*2*box[1]/((float) (plt->maty-1)));
      }
    for (iy=0; iy<plt->maty; iy++)
      for (ix=0; ix<plt->matx-1; ix++) {
	glVertex3f(-box[0]+ix*2*box[0]/((float) (plt->matx-1)),
	   -box[2]+2*box[2]*(plt->matrix[iy*plt->matx+ix]-scaling[2][0])/
		   (scaling[2][1]-scaling[2][0]),
           -box[1]+iy*2*box[1]/((float) (plt->maty-1)));
	glVertex3f(-box[0]+(ix+1)*2*box[0]/((float) (plt->matx-1)),
	   -box[2]+2*box[2]*(plt->matrix[iy*plt->matx+ix+1]-scaling[2][0])/
		   (scaling[2][1]-scaling[2][0]),
           -box[1]+iy*2*box[1]/((float) (plt->maty-1)));
      }
    glEnd();
  }

void computevertices(struct plot *plt)
  {
    int ix,iy;
    plt->vertices=(float (*)[3]) calloc((size_t) plt->matx*(plt->maty),
				   sizeof *plt->vertices);
    for (iy=0; iy<plt->maty; iy++)
      for (ix=0; ix<plt->matx; ix++) {
	plt->vertices[iy*(plt->matx)+ix][0]=-box[0]+ix*2*box[0]/
	  ((float) (plt->matx-1));
	plt->vertices[iy*(plt->matx)+ix][2]=-(-box[1]+iy*2*box[1]/
					 ((float) (plt->maty-1)));
	plt->vertices[iy*(plt->matx)+ix][1]=-box[2]+2*box[2]*
	  (plt->matrix[iy*(plt->matx)+ix]-scaling[2][0])/
	  (scaling[2][1]-scaling[2][0]);
      }
  }

void vectorminus(float *a, float *b, float *c)
  {
    int i;
    for (i=0; i<3; i++)
      c[i]=a[i]-b[i];
  }

void crossprod(float *a, float *b, float *c)
  {
    c[0]=a[1]*b[2]-a[2]*b[1];
    c[1]=a[2]*b[0]-a[0]*b[2];
    c[2]=a[0]*b[1]-a[1]*b[0];
  }

void normalize(float a[], float n[])
{
  float norm2,norm;
  int k;

  norm2 = 0.0;
  for (k = 0; k < 3; k++)
    norm2 += a[k]*a[k];
  norm = sqrt(norm2);
  for (k = 0; k < 3; k++)
    n[k] = a[k]/norm;
}

void normalavg4(float *a1,float *a2,float *a3,float *a4,float *b, float *n,
		int matx, int maty)
  {
    n[1] = 1.0;
    n[0] = -AVG(a4[1],a2[1],2*box[0]/(matx-1));
    n[2] = AVG(a1[1],a3[1],2*box[1]/(maty-1));
    normalize(n,n);
  }

void normalavg3(float *a1,float *a2,float *a3,float *b,float *n)
  {
    float diff1[3],diff2[3],diff3[3];
    float n1[3],n2[3];
    int i;
    vectorminus(a1,b,diff1);
    vectorminus(a2,b,diff2);
    vectorminus(a3,b,diff3);
    crossprod(diff1,diff2,n1);
    crossprod(diff2,diff3,n2);
    normalize(n1,n1);
    normalize(n2,n2);
    for (i=0; i<3; i++)
      n[i]=n1[i]+n2[i];
    normalize(n,n);
  }

void normalavg2(float *a1,float *a2, float *b,float *n)
  {
    float diff1[3],diff2[3];
    vectorminus(a1,b,diff1);
    vectorminus(a2,b,diff2);
    crossprod(diff1,diff2,n);
    normalize(n,n);
  }

    

void computenormals(struct plot *plt)
  {
    int ix,iy,i;
    float n[3];
    plt->normals=(float (*)[3]) calloc((size_t) plt->matx*plt->maty,sizeof *plt->normals);
    for (ix=1; ix<plt->matx-1; ix++)
      for (iy=1; iy<plt->maty-1; iy++){
	normalavg4(plt->vertices[(iy-1)*plt->matx+ix],
		   plt->vertices[iy*plt->matx+ix+1],
		   plt->vertices[(iy+1)*plt->matx+ix],
		   plt->vertices[iy*plt->matx+ix-1],
		   plt->vertices[iy*plt->matx+ix],
		   n,plt->matx,plt->maty);
	for (i=0; i<3; i++) plt->normals[iy*plt->matx+ix][i]=n[i];
      }
    for (ix=1; ix<plt->matx-1; ix++) {
      normalavg3(plt->vertices[ix+1],plt->vertices[plt->matx+ix],plt->vertices[ix-1],
		 plt->vertices[ix],n);
      for (i=0; i<3; i++) plt->normals[ix][i]=n[i];
      normalavg3(plt->vertices[(plt->maty-1)*plt->matx+ix-1],
		 plt->vertices[(plt->maty-2)*plt->matx+ix],
		 plt->vertices[(plt->maty-1)*plt->matx+ix+1],
		 plt->vertices[(plt->maty-1)*plt->matx+ix],
		 n);
      for (i=0; i<3; i++) plt->normals[(plt->maty-1)*plt->matx+ix][i]=n[i];
    }
    for (iy=1; iy<plt->maty-1; iy++) {
      normalavg3(plt->vertices[(iy-1)*plt->matx],
		 plt->vertices[iy*plt->matx+1],
		 plt->vertices[(iy+1)*plt->matx],
		 plt->vertices[iy*plt->matx],
		 n);
      for (i=0; i<3; i++) plt->normals[iy*plt->matx][i]=n[i];
      normalavg3(plt->vertices[(iy+1)*plt->matx+plt->matx-1],
		 plt->vertices[(iy)*plt->matx+plt->matx-2],
		 plt->vertices[(iy-1)*plt->matx+plt->matx-1],
		 plt->vertices[iy*plt->matx+plt->matx-1],
		 n);
      for (i=0; i<3; i++) plt->normals[iy*plt->matx+plt->matx-1][i]=n[i];
    }
    normalavg2(plt->vertices[1],plt->vertices[1*plt->matx],plt->vertices[0],n);
    for (i=0; i<3; i++) plt->normals[0][i]=n[i];
    normalavg2(plt->vertices[1*plt->matx+plt->matx-1],plt->vertices[plt->matx-2],plt->vertices[plt->matx-1],n);
    for (i=0; i<3; i++) plt->normals[plt->matx-1][i]=n[i];
    normalavg2(plt->vertices[(plt->maty-1)*plt->matx+plt->matx-2],
	       plt->vertices[(plt->maty-2)*plt->matx+plt->matx-1],
	       plt->vertices[(plt->maty-1)*plt->matx+plt->matx-1],
	       n);
    for (i=0; i<3; i++) plt->normals[(plt->maty-1)*plt->matx+plt->matx-1][i]=n[i];
    normalavg2(plt->vertices[(plt->maty-2)*plt->matx],
	       plt->vertices[(plt->maty-1)*plt->matx+1],
	       plt->vertices[(plt->maty-1)*plt->matx],
	       n);
    for (i=0; i<3; i++) plt->normals[(plt->maty-1)*plt->matx][i]=n[i];
  }
         


void displayRender(struct plot *plt)
  {
    int ix,iy;
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
    glNormalPointer(GL_FLOAT,0,plt->normals);
    glVertexPointer(3,GL_FLOAT,0,plt->vertices);
    for (ix=0; ix<plt->matx-1; ix++)
      for (iy=0; iy<plt->maty-1; iy++) {
	glBegin(GL_QUADS);
        glArrayElement(iy*plt->matx+ix);
	glArrayElement((iy+1)*plt->matx+ix);
	glArrayElement((iy+1)*plt->matx+ix+1);
	glArrayElement(iy*plt->matx+ix+1);
	glEnd();
      }
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
  }


void display(void)
{
   if (anim.render) {
     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
     if (anim.alpha_blending) {
        glEnable(GL_BLEND);
        /* glDepthMask(GL_FALSE);*/
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(1.0, 1.0, 1.0, 1.0);
     } else
       glColor3f(1.0, 1.0, 1.0);
   } else {
     glClear (GL_COLOR_BUFFER_BIT);
     glColor3f(1.0, 1.0, 1.0);
   }
   glLoadIdentity ();             /* clear the matrix */
           /* viewing transformation  */
   gluLookAt (0.0, camera, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
   init();
   glRotatef(thetaangle,1.0,0.0,0.0);
   glRotatef(spinangle,0.0,1.0,0.0);
   /* Draw the surrounding box */
   glShadeModel(GL_FLAT);
   glDisable(GL_LIGHTING);
   glDisable(GL_LIGHT0);
   displayBox();
   draw_axes();
   if (anim.render) {
     glShadeModel(GL_SMOOTH);
     glEnable(GL_LIGHTING);
     glEnable(GL_LIGHT0);
     glEnable(GL_DEPTH_TEST);
     glFrontFace(GL_CW);
   }
   if (anim.render) {
     displayRender(&anim.plt[iplot]);
   } else {
     displayWireframe(&anim.plt[iplot]);
   }
   if (anim.alpha_blending) {
     glDepthMask(GL_TRUE);
     glDisable(GL_BLEND);
   }
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

void read_string(FILE *ifile,char* str,int nmax)
{
  int i;
  char c;
  while ((c = getc(ifile)) != EOF && isspace(c))
    ;
  if (feof(ifile)) {
    fprintf(stderr,"error: unexpected end of file.\n");
    exit(1);
  }
  if (c != '\"') {
    fprintf(stderr,"error: double quote expected.\n");
    exit(1);
  }
  
  for (i = 0; (c = getc(ifile)) != EOF && c != '\"' && i < nmax-1; i++)
    str[i] = c;
  str[i] = '\0';
  if (feof(ifile)) {
    fprintf(stderr,"error: unexpected end of file.\n");
    exit(1);
  }
  if (i >= nmax-1) {
    fprintf(stderr,"error: input string too long.\n");
    exit(1);
  }
}

void read_plot(FILE *ifile, struct plot *plt)
{
   int ix,iy;
   float value;
  
   if (2!=fscanf(ifile,"%d %d",&plt->matx,&plt->maty)) {
      fprintf(stderr,"surfplt: arg for 'data' statement missing.\n");
      exit(1);
   }
   if ((plt->matx<3) || (plt->maty<3)) {
      fprintf(stderr,"surfplt: array dims must be greater than 2!\n");
      exit(1);
   }
   plt->matrix=calloc((size_t) plt->matx*plt->maty,sizeof(float));
   if (NULL==plt->matrix) {
     fprintf(stderr,"surfplt: Cannot allocate memory.\n");
     exit(1);
   }
   for (iy=0; iy<plt->maty;iy++)
      for (ix=0; ix<plt->matx; ix++){
	if (fscanf(ifile,"%f",&value) != 1) {
	  fprintf(stderr,
		  "surfplt: float array expected after 'data' statement.\n");
	  exit(1);
	}
	plt->matrix[iy*plt->matx+ix]=value;
      }
}

void find_min_max(struct plot *plt, float *min, float *max)
{
  int ix,iy;
  float value;
  for (iy = 0; iy < plt->maty; iy++)
    for (ix = 0; ix < plt->matx; ix++) {
      value = plt->matrix[iy*plt->matx+ix];
      if (value < *min) *min = value;
      if (value > *max) *max = value;
    }
}
  
      
void readinput(FILE *ifile)
  {
    int scanreturn,iplt;
    char token[MAXSTRLEN];
    char c;
    int data_seen = FALSE;
    while ((scanreturn=fscanf(ifile,"%s",token))!= EOF && scanreturn == 1) {
      if (token[0] == '#') {
	while ((c = getc(ifile)) != EOF && c != '\n') ;
      } else if (0==strcmp(token,"box")) {
	if (3!=fscanf(ifile,"%f %f %f",&(box[0]),&(box[1]),&(box[2]))) {
	  fprintf(stderr,"surfplt: arg for box missing.\n");
	  exit(1);
	}
      } else if (0==strcmp(token,"scale")) {
	if (4!=fscanf(ifile,"%f %f %f %f",&(scaling[0][0]),&(scaling[0][1]),
		     &(scaling[1][0]),&(scaling[1][1]))) {
	  fprintf(stderr, "surfplt: arg for scale missing.\n");
	  exit(1);
	}
      } else if (0 == strcmp(token,"xlabel")) {
	read_string(ifile,xlabel,MAXSTRLEN);
      } else if (0 == strcmp(token,"ylabel")) {
	read_string(ifile,ylabel,MAXSTRLEN);
      } else if (0 == strcmp(token,"zlabel")) {
	read_string(ifile,zlabel,MAXSTRLEN);
      } else if (0 == strcmp(token,"font_path")) {
	read_string(ifile,font_path,MAXSTRLEN);
      } else if (0 == strcmp(token,"color")) {
        if (3 != fscanf(ifile,"%f %f %f",&material_color[0],
          &material_color[1],&material_color[2])) {
            fprintf(stderr,"surfplt: args for color missing.\n");
            exit(1);
          }
      } else if (0==strcmp(token,"data")) {
        anim.animate = FALSE;
        anim.size = 1;
        anim.plt = (struct plot *) malloc((size_t) sizeof(struct plot));
	if (anim.plt == NULL) {
	  fprintf(stderr,"surfplt: Unable to allocate memory.\n");
	  exit(2);
	}
	read_plot(ifile, &anim.plt[0]);
        data_seen = TRUE;
      } else if (0 == strcmp(token,"animate") &&
		 1 == fscanf(ifile,"%d",&anim.size) &&
		 anim.size >=1) {
        anim.animate = TRUE;
	anim.plt = (struct plot *) calloc((size_t) anim.size,
					  sizeof(struct plot));
	if (anim.plt == NULL) {
	  fprintf(stderr,"surfplt: unable to allocate memory.\n");
	  exit(2);
	}
	for (iplt = 0; iplt < anim.size; iplt++) {
	  if (1 != fscanf(ifile,"%s",token) ||
	      0 != strcmp(token,"data")) {
	    fprintf(stderr,"surfplt: syntax error in input file.\n");
	    exit(1);
	  }
	  read_plot(ifile, &anim.plt[iplt]);
          data_seen = TRUE;
	}
      } else { /* no known token found. */
	fprintf(stderr,"surfplt: illegal token '%s' in input file.\n",token);
	exit(1);
      }
    }
    if (!data_seen) {
      fprintf(stderr, "surfplt: error, not data read.\n");
      exit(1);
    }
  }

int main(int argc, char** argv)
{
  int argi,i;
  float min, max;
   char filename[MAXSTRLEN] = "";
   int read_from_file = FALSE;
   FILE *infile;
   iplot = 0;
   width=500;
   height=500;
   spinangle=-45.0;
   thetaangle = 0.0;
   x_mouse = width/2;
   y_mouse = height/2;
   anim.motion=FALSE;
   spinincr=0.1;
   alpha = 1.0;
   anim.alpha_blending = FALSE;
   camera = 0.7;
   box[0]=1.0;
   box[1]=1.0;
   box[2]=1.0;
   scaling[0][0]=0.0;
   scaling[0][1]=1.0;
   scaling[1][0]=0.0;
   scaling[1][1]=1.0;
   strcpy(xlabel,"x");
   strcpy(ylabel,"y");
   strcpy(zlabel,"z");
   for (argi=1; argi<argc; argi++) {
     if (*(argv[argi]) != '-') {
       if (!read_from_file) {
         strcpy(filename,argv[argi]);
         read_from_file = TRUE;
       } else {
	 fprintf(stderr,"surfplt: too many filenames in command line.\n");
	 exit(1);
       }
     } else if (0==strcmp(argv[argi],"-w")) {
	anim.render=FALSE;
      } else if (0==strcmp(argv[argi],"-m")) {
	if (1!=sscanf(argv[++argi],"%f",&spinincr)) {
	  fprintf(stderr,"surfplt: error parsing command line.\n");
	  exit(1);
	}
	if (spinincr>360.0 || spinincr<-360.0) {
	  fprintf(stderr,"surfplt: strange increment angle.\n");
	}
	if (1!=sscanf(argv[++argi],"%f",&sleeptime)) {
	  fprintf(stderr,"surfplt: error parsing command line.\n");
	}
	anim.motion=TRUE;
     } else if (0 == strcmp(argv[argi],"-s")) {
       if (1 != sscanf(argv[++argi],"%f",&sleeptime) || sleeptime < 0.0) {
	 fprintf(stderr,"surfplt: error reading cmd line args.\n");
	 exit(1);
       }
     } else if (0==strcmp(argv[argi],"-h")) {
printf("usage: surfplt [-h] [-c] [-w] [-m <angle> <sleep>] [-a <alpha>] [-display <display>] <file>\n");
	printf("   -h   print this help.\n");
	printf("   -c <height>\n");
	printf("        set vertical camera position.\n");
	printf("   -w   draw wireframe only, do not render.\n");
        printf("   -a   make surface tranlucent by factor <alpha>.\n");
        printf("   -s  <sleep>  set sleeptime in msecs for animation.\n");
	printf("   -m <angle> <sleep>\n");
	printf("        rotate the surface by increment <angle> every\n");
	printf("        <sleep> milliseconds.\n");
	exit(0);
      } else if (0==strcmp(argv[argi],"-c")) {
	if (1!=sscanf(argv[++argi],"%f",&camera)) {
	  fprintf(stderr,"surfplt: error parsing command line.\n");
	  exit(1);
	}
      } else if (0 == strcmp(argv[argi],"-a")) {
	if (1 != sscanf(argv[++argi],"%f",&alpha)) {
	  fprintf(stderr,"surfplt: error parsing command line.\n");
	  exit(1);
	} else if (alpha < 0.0 || alpha > 1.0) {
	  fprintf(stderr,"surfplt: illegal alpha value in -a option.\n");
	  exit(1);
	} else if (!anim.render) {
	  fprintf(stderr, "surfplt: no alpha blending with wireframe.\n");
	  exit(1);
	} else {
	  anim.alpha_blending = TRUE;
	  // fprintf(stderr,"surfplt: alpha blending is experimental!\n");
        }
     } else { /* unknown command line option found. */
       fprintf(stderr,"surfplt: unknown command line option '%s' found.\n",
	       argv[argi]);
       exit(1);
     }
   }
   if (read_from_file)
     infile = fopen(filename, "r");
   else
     infile = stdin;
   if (!infile) {
     fprintf(stderr, "surfplt: unable to open input file '%s'.\n",
	     filename);
     exit(2);
   }
   readinput(infile);

   /* Compute min and max value of all matrices.. */
   min = anim.plt[0].matrix[0];
   max = anim.plt[0].matrix[0];
   for (i = 0; i < anim.size; i++)
     find_min_max(&anim.plt[i],&min,&max);
   scaling[2][0] = min;
   scaling[2][1] = max;
   
   init_ftgl();
   for (i = 0; i < anim.size; i++) {
     computevertices(&anim.plt[i]);
     computenormals(&anim.plt[i]);
   }
   glutInit(&argc, argv);
   if (anim.render) {
     if (anim.alpha_blending) 
       glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
     else
       glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
   } else {
     glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
   }
   glutInitWindowSize(width,height); 
   glutInitWindowPosition(100, 100);
   glutCreateWindow(argv[0]);
   if (anim.render) glEnable(GL_DEPTH_TEST);
   glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
   glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
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
