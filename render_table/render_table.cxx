// based on glpuzzle.cxx from FLTK

/*
 * Renderer des Airhockey Tischs:
 * Input:  Position x, y des Pucks und "Schubser" in mm
 * Output: Perspektivische Ansicht
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <math.h>
#include <FL/glut.H>	// changed for fltk
#include <FL/glu.h>   // added for fltk

#define WIDTH 4
#define HEIGHT 5
#define PIECES 10
#define OFFSETX -2
#define OFFSETY -2.5
#define OFFSETZ -0.5

typedef char Config[HEIGHT][WIDTH];

struct puzzle {
  struct puzzle *backptr;
  struct puzzle *solnptr;
  Config pieces;
  struct puzzle *next;
  unsigned hashvalue;
};

#define HASHSIZE 10691

struct puzzlelist {
  struct puzzle *puzzle;
  struct puzzlelist *next;
};

static char convert[PIECES + 1] =
{0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 4};

static unsigned char colors[PIECES + 1][3] =
{
  {0, 0, 0},
  {255, 255, 127},
  {255, 255, 127},
  {255, 255, 127},
  {255, 255, 127},
  {255, 127, 255},
  {255, 127, 255},
  {255, 127, 255},
  {255, 127, 255},
  {255, 127, 127},
  {255, 255, 255},
};

void changeState(void);

static struct puzzle *hashtable[HASHSIZE];

int curX, curY, visible;

#define MOVE_SPEED 0.2
static unsigned char movingPiece;
static int doubleBuffer = 1;
static int depth = 1;

static char xsize[PIECES + 1] =
{0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2};
static char ysize[PIECES + 1] =
{0, 1, 1, 1, 1, 2, 2, 2, 2, 1, 2};
static float zsize[PIECES + 1] =
{0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0.6};

static Config startConfig =
{
  {8, 10, 10, 7},
  {8, 10, 10, 7},
  {6, 9, 9, 5},
  {6, 4, 3, 5},
  {2, 0, 0, 1}
};

static Config thePuzzle =
{
  {8, 10, 10, 7},
  {8, 10, 10, 7},
  {6, 9, 9, 5},
  {6, 4, 3, 5},
  {2, 0, 0, 1}
};

static long W = 400, H = 300;
static GLint viewport[4];

#define srandom srand
#define random() (rand() >> 2)

unsigned
hash(Config config)
{
  int i, j, value;

  value = 0;
  for (i = 0; i < HEIGHT; i++) {
    for (j = 0; j < WIDTH; j++) {
      value = value + convert[(int)config[i][j]];
      value *= 6;
    }
  }
  return (value);
}

int
solution(Config config)
{
  if (config[4][1] == 10 && config[4][2] == 10)
    return (1);
  return (0);
}

float boxcoords[][3] =
{
  {0.2, 0.2, 0.9},
  {0.8, 0.2, 0.9},
  {0.8, 0.8, 0.9},
  {0.2, 0.8, 0.9},
  {0.2, 0.1, 0.8},
  {0.8, 0.1, 0.8},
  {0.9, 0.2, 0.8},
  {0.9, 0.8, 0.8},
  {0.8, 0.9, 0.8},
  {0.2, 0.9, 0.8},
  {0.1, 0.8, 0.8},
  {0.1, 0.2, 0.8},
  {0.2, 0.1, 0.2},
  {0.8, 0.1, 0.2},
  {0.9, 0.2, 0.2},
  {0.9, 0.8, 0.2},
  {0.8, 0.9, 0.2},
  {0.2, 0.9, 0.2},
  {0.1, 0.8, 0.2},
  {0.1, 0.2, 0.2},
  {0.2, 0.2, 0.1},
  {0.8, 0.2, 0.1},
  {0.8, 0.8, 0.1},
  {0.2, 0.8, 0.1},
};

float boxnormals[][3] =
{
  {0, 0, 1},            /* 0 */
  {0, 1, 0},
  {1, 0, 0},
  {0, 0, -1},
  {0, -1, 0},
  {-1, 0, 0},
  {0.7071, 0.7071, 0.0000},  /* 6 */
  {0.7071, -0.7071, 0.0000},
  {-0.7071, 0.7071, 0.0000},
  {-0.7071, -0.7071, 0.0000},
  {0.7071, 0.0000, 0.7071},  /* 10 */
  {0.7071, 0.0000, -0.7071},
  {-0.7071, 0.0000, 0.7071},
  {-0.7071, 0.0000, -0.7071},
  {0.0000, 0.7071, 0.7071},  /* 14 */
  {0.0000, 0.7071, -0.7071},
  {0.0000, -0.7071, 0.7071},
  {0.0000, -0.7071, -0.7071},
  {0.5774, 0.5774, 0.5774},  /* 18 */
  {0.5774, 0.5774, -0.5774},
  {0.5774, -0.5774, 0.5774},
  {0.5774, -0.5774, -0.5774},
  {-0.5774, 0.5774, 0.5774},
  {-0.5774, 0.5774, -0.5774},
  {-0.5774, -0.5774, 0.5774},
  {-0.5774, -0.5774, -0.5774},
};

int boxfaces[][4] =
{
  {0, 1, 2, 3},         /* 0 */
  {9, 8, 16, 17},
  {6, 14, 15, 7},
  {20, 23, 22, 21},
  {12, 13, 5, 4},
  {19, 11, 10, 18},
  {7, 15, 16, 8},       /* 6 */
  {13, 14, 6, 5},
  {18, 10, 9, 17},
  {19, 12, 4, 11},
  {1, 6, 7, 2},         /* 10 */
  {14, 21, 22, 15},
  {11, 0, 3, 10},
  {20, 19, 18, 23},
  {3, 2, 8, 9},         /* 14 */
  {17, 16, 22, 23},
  {4, 5, 1, 0},
  {20, 21, 13, 12},
  {2, 7, 8, -1},        /* 18 */
  {16, 15, 22, -1},
  {5, 6, 1, -1},
  {13, 21, 14, -1},
  {10, 3, 9, -1},
  {18, 17, 23, -1},
  {11, 4, 0, -1},
  {20, 12, 19, -1},
};

#define NBOXFACES (sizeof(boxfaces)/sizeof(boxfaces[0]))

/* Draw a box.  Bevel as desired. */
void
drawBox(int piece, float xoff, float yoff)
{
  int xlen, ylen;
  int i, k;
  float x, y, z;
  float zlen;
  float *v;

  xlen = xsize[piece];
  ylen = ysize[piece];
  zlen = zsize[piece];

  glColor3ubv(colors[piece]);
  glBegin(GL_QUADS);
  for (i = 0; i < 18; i++) {
    glNormal3fv(boxnormals[i]);
    for (k = 0; k < 4; k++) {
      if (boxfaces[i][k] == -1)
        continue;
      v = boxcoords[boxfaces[i][k]];
      x = v[0] + OFFSETX;
      if (v[0] > 0.5)
        x += xlen - 1;
      y = v[1] + OFFSETY;
      if (v[1] > 0.5)
        y += ylen - 1;
      z = v[2] + OFFSETZ;
      if (v[2] > 0.5)
        z += zlen - 1;
      glVertex3f(xoff + x, yoff + y, z);
    }
  }
  glEnd();
  glBegin(GL_TRIANGLES);
  for (i = 18; i < int(NBOXFACES); i++) {
    glNormal3fv(boxnormals[i]);
    for (k = 0; k < 3; k++) {
      if (boxfaces[i][k] == -1)
        continue;
      v = boxcoords[boxfaces[i][k]];
      x = v[0] + OFFSETX;
      if (v[0] > 0.5)
        x += xlen - 1;
      y = v[1] + OFFSETY;
      if (v[1] > 0.5)
        y += ylen - 1;
      z = v[2] + OFFSETZ;
      if (v[2] > 0.5)
        z += zlen - 1;
      glVertex3f(xoff + x, yoff + y, z);
    }
  }
  glEnd();
}

float containercoords[][3] =
{
  {-0.1, -0.1, 1.0},
  {-0.1, -0.1, -0.1},
  {4.1, -0.1, -0.1},
  {4.1, -0.1, 1.0},
  {1.0, -0.1, 0.6},     /* 4 */
  {3.0, -0.1, 0.6},
  {1.0, -0.1, 0.0},
  {3.0, -0.1, 0.0},
  {1.0, 0.0, 0.0},      /* 8 */
  {3.0, 0.0, 0.0},
  {3.0, 0.0, 0.6},
  {1.0, 0.0, 0.6},
  {0.0, 0.0, 1.0},      /* 12 */
  {4.0, 0.0, 1.0},
  {4.0, 0.0, 0.0},
  {0.0, 0.0, 0.0},
  {0.0, 5.0, 0.0},      /* 16 */
  {0.0, 5.0, 1.0},
  {4.0, 5.0, 1.0},
  {4.0, 5.0, 0.0},
  {-0.1, 5.1, -0.1},    /* 20 */
  {4.1, 5.1, -0.1},
  {4.1, 5.1, 1.0},
  {-0.1, 5.1, 1.0},
};

float containernormals[][3] =
{
  {0, -1, 0},
  {0, -1, 0},
  {0, -1, 0},
  {0, -1, 0},
  {0, -1, 0},
  {0, 1, 0},
  {0, 1, 0},
  {0, 1, 0},
  {1, 0, 0},
  {1, 0, 0},
  {1, 0, 0},
  {-1, 0, 0},
  {-1, 0, 0},
  {-1, 0, 0},
  {0, 1, 0},
  {0, 0, -1},
  {0, 0, -1},
  {0, 0, 1},
  {0, 0, 1},
  {0, 0, 1},
  {0, 0, 1},
  {0, 0, 1},
  {0, 0, 1},
  {0, 0, 1},
};

int containerfaces[][4] =
{
  {1, 6, 4, 0},
  {0, 4, 5, 3},
  {1, 2, 7, 6},
  {7, 2, 3, 5},
  {16, 19, 18, 17},

  {23, 22, 21, 20},
  {12, 11, 8, 15},
  {10, 13, 14, 9},

  {15, 16, 17, 12},
  {2, 21, 22, 3},
  {6, 8, 11, 4},

  {1, 0, 23, 20},
  {14, 13, 18, 19},
  {9, 7, 5, 10},

  {12, 13, 10, 11},

  {1, 20, 21, 2},
  {4, 11, 10, 5},

  {15, 8, 19, 16},
  {19, 8, 9, 14},
  {8, 6, 7, 9},
  {0, 3, 13, 12},
  {13, 3, 22, 18},
  {18, 22, 23, 17},
  {17, 23, 0, 12},
};

#define NCONTFACES (sizeof(containerfaces)/sizeof(containerfaces[0]))

/* Draw the container */
void
drawContainer(void)
{
  int i, k;
  float *v;

  /* Y is reversed here because the model has it reversed */

  /* Arbitrary bright wood-like color */
  glColor3ub(209, 103, 23);
  glBegin(GL_QUADS);
  for (i = 0; i < int(NCONTFACES); i++) {
    v = containernormals[i];
    glNormal3f(v[0], -v[1], v[2]);
    for (k = 3; k >= 0; k--) {
      v = containercoords[containerfaces[i][k]];
      glVertex3f(v[0] + OFFSETX, -(v[1] + OFFSETY), v[2] + OFFSETZ);
    }
  }
  glEnd();
}

void
drawAll(void)
{
  int i, j;
  int piece;
  char done[PIECES + 1];

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0, 0, -10);

  glRotatef(-45, 1, 0, 0);

  if (depth) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  } else {
    glClear(GL_COLOR_BUFFER_BIT);
  }
  for (i = 1; i <= PIECES; i++) {
    done[i] = 0;
  }
  drawContainer();
  for (i = 0; i < HEIGHT; i++) {
    for (j = 0; j < WIDTH; j++) {
      piece = thePuzzle[i][j];
      if (piece == 0)
        continue;
      if (done[piece])
        continue;
      done[piece] = 1;
      drawBox(piece, j, i);
    }
  }
}

void
redraw(void)
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45, viewport[2]*1.0/viewport[3], 0.1, 100.0);

  drawAll();

  if (doubleBuffer)
    glutSwapBuffers();
  else
    glFinish();
}

void
multMatrices(const GLfloat a[16], const GLfloat b[16], GLfloat r[16])
{
  int i, j;

  for (i = 0; i < 4; i++) {
    for (j = 0; j < 4; j++) {
      r[i * 4 + j] =
        a[i * 4 + 0] * b[0 * 4 + j] +
        a[i * 4 + 1] * b[1 * 4 + j] +
        a[i * 4 + 2] * b[2 * 4 + j] +
        a[i * 4 + 3] * b[3 * 4 + j];
    }
  }
}

void
makeIdentity(GLfloat m[16])
{
  m[0 + 4 * 0] = 1;
  m[0 + 4 * 1] = 0;
  m[0 + 4 * 2] = 0;
  m[0 + 4 * 3] = 0;
  m[1 + 4 * 0] = 0;
  m[1 + 4 * 1] = 1;
  m[1 + 4 * 2] = 0;
  m[1 + 4 * 3] = 0;
  m[2 + 4 * 0] = 0;
  m[2 + 4 * 1] = 0;
  m[2 + 4 * 2] = 1;
  m[2 + 4 * 3] = 0;
  m[3 + 4 * 0] = 0;
  m[3 + 4 * 1] = 0;
  m[3 + 4 * 2] = 0;
  m[3 + 4 * 3] = 1;
}

/*
   ** inverse = invert(src)
 */
int
invertMatrix(const GLfloat src[16], GLfloat inverse[16])
{
  int i, j, k, swap;
  double t;
  GLfloat temp[4][4];

  for (i = 0; i < 4; i++) {
    for (j = 0; j < 4; j++) {
      temp[i][j] = src[i * 4 + j];
    }
  }
  makeIdentity(inverse);

  for (i = 0; i < 4; i++) {
    /* 
       ** Look for largest element in column */
    swap = i;
    for (j = i + 1; j < 4; j++) {
      if (fabs(temp[j][i]) > fabs(temp[i][i])) {
        swap = j;
      }
    }

    if (swap != i) {
      /* 
         ** Swap rows. */
      for (k = 0; k < 4; k++) {
        t = temp[i][k];
        temp[i][k] = temp[swap][k];
        temp[swap][k] = t;

        t = inverse[i * 4 + k];
        inverse[i * 4 + k] = inverse[swap * 4 + k];
        inverse[swap * 4 + k] = t;
      }
    }
    if (temp[i][i] == 0) {
      /* 
         ** No non-zero pivot.  The matrix is singular, which
         shouldn't ** happen.  This means the user gave us a
         bad matrix. */
      return 0;
    }
    t = temp[i][i];
    for (k = 0; k < 4; k++) {
      temp[i][k] /= t;
      inverse[i * 4 + k] /= t;
    }
    for (j = 0; j < 4; j++) {
      if (j != i) {
        t = temp[j][i];
        for (k = 0; k < 4; k++) {
          temp[j][k] -= temp[i][k] * t;
          inverse[j * 4 + k] -= inverse[i * 4 + k] * t;
        }
      }
    }
  }
  return 1;
}

/*
   ** This is a screwball function.  What it does is the following:
   ** Given screen x and y coordinates, compute the corresponding object space 
   **   x and y coordinates given that the object space z is 0.9 + OFFSETZ.
   ** Since the tops of (most) pieces are at z = 0.9 + OFFSETZ, we use that 
   **   number.
 */
int
computeCoords(int piece, int mousex, int mousey,
  GLfloat * selx, GLfloat * sely)
{
  GLfloat modelMatrix[16];
  GLfloat projMatrix[16];
  GLfloat finalMatrix[16];
  GLfloat in[4];
  GLfloat a, b, c, d;
  GLfloat top, bot;
  GLfloat z;
  GLfloat w;
  GLfloat height;

  if (piece == 0)
    return 0;
  height = zsize[piece] - 0.1 + OFFSETZ;

  glGetFloatv(GL_PROJECTION_MATRIX, projMatrix);
  glGetFloatv(GL_MODELVIEW_MATRIX, modelMatrix);
  multMatrices(modelMatrix, projMatrix, finalMatrix);
  if (!invertMatrix(finalMatrix, finalMatrix))
    return 0;

  in[0] = (2.0 * (mousex - viewport[0]) / viewport[2]) - 1;
  in[1] = (2.0 * ((H - mousey) - viewport[1]) / viewport[3]) - 1;

  a = in[0] * finalMatrix[0 * 4 + 2] +
    in[1] * finalMatrix[1 * 4 + 2] +
    finalMatrix[3 * 4 + 2];
  b = finalMatrix[2 * 4 + 2];
  c = in[0] * finalMatrix[0 * 4 + 3] +
    in[1] * finalMatrix[1 * 4 + 3] +
    finalMatrix[3 * 4 + 3];
  d = finalMatrix[2 * 4 + 3];

  /* 
     ** Ok, now we need to solve for z: **   (a + b z) / (c + d 

     z) = height. ** ("height" is the height in object space we 

     want to solve z for) ** ** ==>  a + b z = height c +
     height d z **      bz - height d z = height c - a ** z =
     (height c - a) / (b - height d) */
  top = height * c - a;
  bot = b - height * d;
  if (bot == 0.0)
    return 0;

  z = top / bot;

  /* 
     ** Ok, no problem. ** Now we solve for x and y.  We know
     that w = c + d z, so we compute it. */
  w = c + d * z;

  /* 
     ** Now for x and y: */
  *selx = (in[0] * finalMatrix[0 * 4 + 0] +
    in[1] * finalMatrix[1 * 4 + 0] +
    z * finalMatrix[2 * 4 + 0] +
    finalMatrix[3 * 4 + 0]) / w - OFFSETX;
  *sely = (in[0] * finalMatrix[0 * 4 + 1] +
    in[1] * finalMatrix[1 * 4 + 1] +
    z * finalMatrix[2 * 4 + 1] +
    finalMatrix[3 * 4 + 1]) / w - OFFSETY;
  return 1;
}

static int solving;
static int spinning;

static void
Reshape(int width, int height)
{

  W = width;
  H = height;
  glViewport(0, 0, W, H);
  glGetIntegerv(GL_VIEWPORT, viewport);
}

void reset(void)
{
    if (solving) {
      //freeSolutions();
      solving = 0;
      glutChangeToMenuEntry(1, (char *)"Solving", 1);
      glutSetWindowTitle((char *)"glpuzzle");
      movingPiece = 0;
      changeState();
    }
    memcpy(thePuzzle, startConfig, HEIGHT * WIDTH);
    glutPostRedisplay();
}

void
keyboard(unsigned char c, int x, int y)
{
  switch (c) {
  case 27:
    exit(0);
    break;
  case 'R':
  case 'r':
    reset();
    break;
  case 'b':
  case 'B':
    depth = 1 - depth;
    if (depth) {
      glEnable(GL_DEPTH_TEST);
    } else {
      glDisable(GL_DEPTH_TEST);
    }
    glutPostRedisplay();
    break;
  default:
    break;
  }
}

void
changeState(void)
{
  if (visible) {
    if (!solving && !spinning) {
      glutIdleFunc(NULL);
    } else {
      //glutIdleFunc(animate);
    }
  } else {
    glutIdleFunc(NULL);
  }
}

void
init(void)
{
  static float lmodel_ambient[] =
  {0.0, 0.0, 0.0, 0.0};
  static float lmodel_twoside[] =
  {GL_FALSE};
  static float lmodel_local[] =
  {GL_FALSE};
  static float light0_ambient[] =
  {0.1, 0.1, 0.1, 1.0};
  static float light0_diffuse[] =
  {1.0, 1.0, 1.0, 0.0};
  static float light0_position[] =
  {0.8660254, 0.5, 1, 0};
  static float light0_specular[] =
  {0.0, 0.0, 0.0, 0.0};
  static float bevel_mat_ambient[] =
  {0.0, 0.0, 0.0, 1.0};
  static float bevel_mat_shininess[] =
  {40.0};
  static float bevel_mat_specular[] =
  {0.0, 0.0, 0.0, 0.0};
  static float bevel_mat_diffuse[] =
  {1.0, 0.0, 0.0, 0.0};

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glEnable(GL_DEPTH_TEST);
  glClearDepth(1.0);

  glClearColor(0.5, 0.5, 0.5, 0.0);
  glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
  glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
  glEnable(GL_LIGHT0);

  glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, lmodel_local);
  glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, lmodel_twoside);
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
  glEnable(GL_LIGHTING);

  glMaterialfv(GL_FRONT, GL_AMBIENT, bevel_mat_ambient);
  glMaterialfv(GL_FRONT, GL_SHININESS, bevel_mat_shininess);
  glMaterialfv(GL_FRONT, GL_SPECULAR, bevel_mat_specular);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, bevel_mat_diffuse);

  glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
  glEnable(GL_COLOR_MATERIAL);
  glShadeModel(GL_FLAT);

  //trackball(curquat, 0.0, 0.0, 0.0, 0.0);
  srandom(time(NULL));
}

static void
Usage(void)
{
  puts("Usage: puzzle [-s]");
  puts("   -s:  Run in single buffered mode");
  exit(-1);
}

void
visibility(int v)
{
  if (v == GLUT_VISIBLE) {
    visible = 1;
  } else {
    visible = 0;
  }
  changeState();
}

void
menu(int choice)
{
   switch(choice) {
   case 2:
      reset();
      break;
   case 3:
      exit(0);
      break;
   }
}

int
main(int argc, char **argv)
{
  long i;

  Fl::use_high_res_GL(1);
  glutInit(&argc, argv);
  for (i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      switch (argv[i][1]) {
      case 's':
        doubleBuffer = 0;
        break;
      default:
        Usage();
      }
    } else {
      Usage();
    }
  }

  glutInitWindowSize(W, H);
  if (doubleBuffer) {
    glutInitDisplayMode(GLUT_DEPTH | GLUT_RGB | GLUT_DOUBLE | GLUT_MULTISAMPLE);
  } else {
    glutInitDisplayMode(GLUT_DEPTH | GLUT_RGB | GLUT_SINGLE | GLUT_MULTISAMPLE);
  }

  glutCreateWindow("glpuzzle");
  visible = 1; // added for fltk, bug in original program?

  init();

  glGetIntegerv(GL_VIEWPORT, viewport);

  puts("");
  puts("r   Reset puzzle");
  puts("s   Solve puzzle (may take a few seconds to compute)");
  puts("d   Destroy a piece - makes the puzzle easier");
  puts("b   Toggles the depth buffer on and off");
  puts("");
  puts("Left mouse moves pieces");
  puts("Middle mouse spins the puzzle");
  puts("Right mouse has menu");

  glutReshapeFunc(Reshape);
  glutDisplayFunc(redraw);
  glutKeyboardFunc(keyboard);
  glutVisibilityFunc(visibility);
  glutCreateMenu(menu);
  glutAddMenuEntry((char *)"Solve", 1);
  glutAddMenuEntry((char *)"Reset", 2);
  glutAddMenuEntry((char *)"Quit", 3);
  glutAttachMenu(GLUT_RIGHT_BUTTON);
  glutMainLoop();
  return 0;             /* ANSI C requires main to return int. */
}

