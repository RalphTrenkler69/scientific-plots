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

float spinangle,thetaangle,camera,spinincr,sleeptime,box[3],scaling[3][2];
int width,height,x_mouse,y_mouse;
float alpha;
GLuint dlist_base;

int matx,maty;
float *matrix;
float (*vertices)[3],(*normals)[3];
char xlabel[MAXSTRLEN], ylabel[MAXSTRLEN], zlabel[MAXSTRLEN];


int render,motion,alpha_blending;

void init(void) 
{
   GLfloat mat_specular[] = {1.0,1.0,1.0,1.0};
   GLfloat mat_ambient[] = {0.5,0.0,0.0,1.0};
   GLfloat mat_diffuse[] = {0.8,0.6,0.6,1.0};
   GLfloat mat_shininess[] = {30.0};
   GLfloat light_position[] = {1.0,1.0,1.0,0.0};
   GLfloat white_light[] = {1.0,1.0,1.0,1.0};
   GLfloat lmodel_ambient[] = {0.9,0.0,0.0,1.0};
   glClearColor (0.0, 0.0, 0.0, 0.0);
   if (! render) {
     glShadeModel (GL_FLAT);
   } else {
     glShadeModel(GL_SMOOTH);
     glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
     glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
     glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
     glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
     glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
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

void displayWireframe(void)
  {
    int ix,iy;
    glBegin(GL_LINES);
    for (ix=0; ix<matx; ix++)
      for (iy=0; iy<maty-1; iy++) {
	glVertex3f(-box[0]+ix*2*box[0]/((float) (matx-1)),
	   -box[2]+2*box[2]*(matrix[iy*matx+ix]-scaling[2][0])/(scaling[2][1]-scaling[2][0]),
           -box[1]+iy*2*box[1]/((float) (maty-1)));
	glVertex3f(-box[0]+ix*2*box[0]/((float) (matx-1)),
	   -box[2]+2*box[2]*(matrix[(iy+1)*matx+ix]-scaling[2][0])/(scaling[2][1]-scaling[2][0]),
           -box[1]+(iy+1)*2*box[1]/((float) (maty-1)));
      }
    for (iy=0; iy<maty; iy++)
      for (ix=0; ix<matx-1; ix++) {
	glVertex3f(-box[0]+ix*2*box[0]/((float) (matx-1)),
	   -box[2]+2*box[2]*(matrix[iy*matx+ix]-scaling[2][0])/(scaling[2][1]-scaling[2][0]),
           -box[1]+iy*2*box[1]/((float) (maty-1)));
	glVertex3f(-box[0]+(ix+1)*2*box[0]/((float) (matx-1)),
	   -box[2]+2*box[2]*(matrix[iy*matx+ix+1]-scaling[2][0])/(scaling[2][1]-scaling[2][0]),
           -box[1]+iy*2*box[1]/((float) (maty-1)));
      }
    glEnd();
  }

void computevertices(void)
  {
    int ix,iy;
    vertices=(float (*)[3]) calloc((size_t) matx*maty,sizeof *vertices);
    for (iy=0; iy<maty; iy++)
      for (ix=0; ix<matx; ix++) {
	vertices[iy*matx+ix][0]=-box[0]+ix*2*box[0]/((float) (matx-1));
	vertices[iy*matx+ix][2]=-(-box[1]+iy*2*box[1]/((float) (maty-1)));
	vertices[iy*matx+ix][1]=-box[2]+2*box[2]*(matrix[iy*matx+ix]-scaling[2][0])/
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

void normalavg4(float *a1,float *a2,float *a3,float *a4,float *b, float *n)
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

    

void computenormals(void)
  {
    int ix,iy,i;
    float n[3];
    normals=(float (*)[3]) calloc((size_t) matx*maty,sizeof *normals);
    for (ix=1; ix<matx-1; ix++)
      for (iy=1; iy<maty-1; iy++){
	normalavg4(vertices[(iy-1)*matx+ix],
		   vertices[iy*matx+ix+1],
		   vertices[(iy+1)*matx+ix],
		   vertices[iy*matx+ix-1],
		   vertices[iy*matx+ix],
		   n);
	for (i=0; i<3; i++) normals[iy*matx+ix][i]=n[i];
      }
    for (ix=1; ix<matx-1; ix++) {
      normalavg3(vertices[ix+1],vertices[matx+ix],vertices[ix-1],
		 vertices[ix],n);
      for (i=0; i<3; i++) normals[ix][i]=n[i];
      normalavg3(vertices[(maty-1)*matx+ix-1],
		 vertices[(maty-2)*matx+ix],
		 vertices[(maty-1)*matx+ix+1],
		 vertices[(maty-1)*matx+ix],
		 n);
      for (i=0; i<3; i++) normals[(maty-1)*matx+ix][i]=n[i];
    }
    for (iy=1; iy<maty-1; iy++) {
      normalavg3(vertices[(iy-1)*matx],
		 vertices[iy*matx+1],
		 vertices[(iy+1)*matx],
		 vertices[iy*matx],
		 n);
      for (i=0; i<3; i++) normals[iy*matx][i]=n[i];
      normalavg3(vertices[(iy+1)*matx+matx-1],
		 vertices[(iy)*matx+matx-2],
		 vertices[(iy-1)*matx+matx-1],
		 vertices[iy*matx+matx-1],
		 n);
      for (i=0; i<3; i++) normals[iy*matx+matx-1][i]=n[i];
    }
    normalavg2(vertices[1],vertices[1*matx],vertices[0],n);
    for (i=0; i<3; i++) normals[0][i]=n[i];
    normalavg2(vertices[1*matx+matx-1],vertices[matx-2],vertices[matx-1],n);
    for (i=0; i<3; i++) normals[matx-1][i]=n[i];
    normalavg2(vertices[(maty-1)*matx+matx-2],
	       vertices[(maty-2)*matx+matx-1],
	       vertices[(maty-1)*matx+matx-1],
	       n);
    for (i=0; i<3; i++) normals[(maty-1)*matx+matx-1][i]=n[i];
    normalavg2(vertices[(maty-2)*matx],
	       vertices[(maty-1)*matx+1],
	       vertices[(maty-1)*matx],
	       n);
    for (i=0; i<3; i++) normals[(maty-1)*matx][i]=n[i];
  }
         


void displayRender(void)
  {
    int ix,iy;
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
    glNormalPointer(GL_FLOAT,0,normals);
    glVertexPointer(3,GL_FLOAT,0,vertices);
    for (ix=0; ix<matx-1; ix++)
      for (iy=0; iy<maty-1; iy++) {
	glBegin(GL_QUADS);
        glArrayElement(iy*matx+ix);
	glArrayElement((iy+1)*matx+ix);
	glArrayElement((iy+1)*matx+ix+1);
	glArrayElement(iy*matx+ix+1);
	glEnd();
      }
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
  }


void display(void)
{
   if (render) {
     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
     if (alpha_blending)
       glColor4f(0.7, 0.7, 0.7, alpha);
     else
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
   if (render) {
     glShadeModel(GL_SMOOTH);
     glEnable(GL_LIGHTING);
     glEnable(GL_LIGHT0);
     glEnable(GL_DEPTH_TEST);
     glFrontFace(GL_CW);
     if (alpha_blending) {
       glEnable(GL_BLEND);
       glDepthMask(GL_FALSE);
       glBlendFunc(GL_SRC_ALPHA, GL_ONE);
     }
   }
   glCallList(dlist_base);
   if (alpha_blending) {
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

void readinput(FILE *ifile)
  {
    int ix,iy,scanreturn;
    float value;
    char token[20];
    while ((scanreturn=fscanf(ifile,"%s",token))!= EOF) {
      if (0==strcmp(token,"box")) {
	if (3!=fscanf(ifile,"%f %f %f",&(box[0]),&(box[1]),&(box[2]))) {
	  fprintf(stderr,"error: arg for box missing.\n");
	  exit(1);
	}
      } else if (0==strcmp(token,"scale")) {
	if (4!=fscanf(ifile,"%f %f %f %f",&(scaling[0][0]),&(scaling[0][1]),
		     &(scaling[1][0]),&(scaling[1][1]))) {
	  fprintf(stderr, "error: arg for scale missing.\n");
	  exit(1);
	}
      } else if (0 == strcmp(token,"xlabel")) {
	read_string(ifile,xlabel,MAXSTRLEN);
      } else if (0 == strcmp(token,"ylabel")) {
	read_string(ifile,ylabel,MAXSTRLEN);
      } else if (0 == strcmp(token,"zlabel")) {
	read_string(ifile,zlabel,MAXSTRLEN);
      } else if (0==strcmp(token,"data")) {
	if (2!=fscanf(ifile,"%d %d",&matx,&maty)) {
	      fprintf(stderr,"error: arg for data missing.\n");
	      exit(1);
	    }
	    if ((matx<3) || (maty<3)) {
	      fprintf(stderr,"error: array dims must be greater than 2!\n");
	      exit(1);
	    }
	    matrix=calloc((size_t) matx*maty,sizeof(float));
	    if (NULL==matrix) {
	      fprintf(stderr,"Cannot allocate memory.\n");
              exit(1);
	    }
	    for (iy=0; iy<maty;iy++)
	      for (ix=0; ix<matx; ix++){
		if (fscanf(ifile,"%f",&value) != 1) {
		  fprintf(stderr,"float expected in input.\n");
		  exit(1);
		}
		matrix[iy*matx+ix]=value;
	      }
	    scaling[2][0]=matrix[0];
	    scaling[2][1]=matrix[0];
	    for (ix=0; ix<matx; ix++)
	      for (iy=0; iy<maty; iy++) {
		value=matrix[iy*matx+ix];
		if (value < scaling[2][0]) scaling[2][0]=value;
		if (value > scaling[2][1]) scaling[2][1]=value;
	      }
	    return;
      }
    }
  }

int main(int argc, char** argv)
{
   int argi;
   char filename[MAXSTRLEN] = "";
   int read_from_file = FALSE;
   FILE *infile;
   render=TRUE;
   width=500;
   height=500;
   spinangle=-45.0;
   thetaangle = 0.0;
   x_mouse = width/2;
   y_mouse = height/2;
   motion=FALSE;
   spinincr=0.1;
   alpha = 1.0;
   alpha_blending = FALSE;
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
	 fprintf(stderr,"error: too many filenames in command line.\n");
	 exit(1);
       }
     } else if (0==strcmp(argv[argi],"-w")) {
	render=0;
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
      } else if (0==strcmp(argv[argi],"-h")) {
printf("usage: surfplt [-h] [-c] [-w] [-m <angle> <sleep>] [-a <alpha>] [-display <display>] <file>\n");
	printf("   -h   print this help.\n");
	printf("   -c <height>\n");
	printf("        set vertical camera position.\n");
	printf("   -w   draw wireframe only, do not render.\n");
        printf("   -a   make surface tranlucent by factor <alpha>.\n");
	printf("   -m <angle> <sleep>\n");
	printf("        rotate the surface by increment <angle> every\n");
	printf("        <sleep> milliseconds.\n");
	exit(0);
      } else if (0==strcmp(argv[argi],"-c")) {
	if (1!=sscanf(argv[++argi],"%f",&camera)) {
	  fprintf(stderr,"error parsing command line.\n");
	  exit(1);
	}
      } else if (0 == strcmp(argv[argi],"-a")) {
	if (1 != sscanf(argv[++argi],"%f",&alpha)) {
	  fprintf(stderr,"error parsing command line.\n");
	  exit(1);
	} else if (alpha < 0.0 || alpha > 1.0) {
	  fprintf(stderr,"surfplt: illegal alpha value in -a option.\n");
	  exit(1);
	} else if (!render) {
	  fprintf(stderr, "surfplt: no alpha blending with wireframe.\n");
	  exit(1);
	} else {
	  alpha_blending = TRUE;
	  fprintf(stderr,"surfplt: alpha blending is experimental!\n");
        }
      }
   }
   if (read_from_file)
     infile = fopen(filename, "r");
   else
     infile = stdin;
   if (!infile) {
     fprintf(stderr, "error: unable to open input file.\n");
     exit(2);
   }
   readinput(infile);
   init_ftgl();
   computevertices();
   computenormals();
   glutInit(&argc, argv);
   if (render) {
     if (alpha_blending) 
       glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
     else
       glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
   } else {
     glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
   }
   glutInitWindowSize (width,height); 
   glutInitWindowPosition (100, 100);
   glutCreateWindow (argv[0]);
   if (render) glEnable(GL_DEPTH_TEST);
   glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
   glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
   /* Compile display list for surface. */
   dlist_base = glGenLists(1);
   if (dlist_base == 0) {
     fprintf(stderr,"surfplt: Error, display list cannot be created.\n");
     exit(1);
   }
   glNewList(dlist_base, GL_COMPILE);
   if (render) {
     displayRender();
   } else {
     displayWireframe();
   }
   glEndList();
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
   glDeleteLists(dlist_base,1);
   return 0;
}
