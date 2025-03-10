#include <GL/freeglut.h>
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>

#include "vector.h" 

#define POINT_COLOR 1.0f, 0.0f, 0.0f
#define LINE_COLOR 0.0f, 0.0f, 1.0f
#define POINT_COLOR_B 0.0f, 1.0f, 0.0f
#define LINE_COLOR_B 0.5f, 0.0f, 0.5f
#define POINT_COLOR_C 0.1f, 0.1f, 0.1f
#define LINE_COLOR_C 1.0f, 1.0f, 1.0
#define WIDTH 640
#define HEIGHT 480 
#define PATERN_SCALE 20

typedef struct {
   PointList list; 
   int mouseDown;
   Point newList[MAX_POINT_NUM];
   Point normalizedList[MAX_POINT_NUM];
   int isNewListEmpty;
   Point resolution;
   int check;
   Point checkList[MAX_POINT_NUM];
} AppState;

void display(void);
void mouseFunc(int button, int state, int x, int y);
void motionFunc(int x, int y);
void reshape(int w, int h);

void appStateInit(AppState *state);
void normalizeNewList(Point array[MAX_POINT_NUM], 
      Point normalizedArray[MAX_POINT_NUM]);

void copyArray(Point a[MAX_POINT_NUM], Point b[MAX_POINT_NUM]);
void drawPointList(PointList list);
void drawPointArray(Point array[MAX_POINT_NUM]);
void drawPointArrayFromCenter(Point array[MAX_POINT_NUM], Point center);

int main(int argc, char **argv) {
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
   glutInitWindowSize(WIDTH, HEIGHT);
   glutCreateWindow("runes");


   AppState *state = (AppState *)calloc(1, sizeof(AppState));
   appStateInit(state);
   glutSetWindowData(state);


   glutDisplayFunc(display);
   glutMouseFunc(mouseFunc);
   glutMotionFunc(motionFunc);
   glutReshapeFunc(reshape);

   glutMainLoop();
   return 0;
}

void mouseFunc(int button, int state, int x, int y) {
   AppState *stateVars = (AppState *)glutGetWindowData();
   if (state == GLUT_DOWN) {
      printf("Mouse button %d pressed at (%d, %d)\n", button, x, y);
      stateVars->mouseDown = 1;
      stateVars->isNewListEmpty = 1;
      if(stateVars->list.points != NULL){
	 free(stateVars->list.points);
	 stateVars->list = (PointList){NULL, 0};
      }
   } else if (state == GLUT_UP) {
      printf("Mouse button %d released at (%d, %d)\n", button, x, y);
      stateVars->mouseDown = 0;
      printPointList(stateVars->list);

      resample(stateVars->list, stateVars->newList);
      stateVars->isNewListEmpty = 0;

      printPointArray(stateVars->newList);
      Point center = getCenterPoint(stateVars->newList);
      printPoint(center);

      normalizeNewList(stateVars->newList, stateVars->normalizedList);

      if(!stateVars->check){
	 copyArray(stateVars->checkList, stateVars->normalizedList);
	 printf("check arr: ");
	 printPointArray(stateVars->checkList);
	 stateVars->check = 1;
      }
      else {
	 stateVars->check = 0;
      }
      
      glutPostRedisplay();
   }
}

void copyArray(Point a[MAX_POINT_NUM], Point b[MAX_POINT_NUM]){
   for (size_t i = 0; i < MAX_POINT_NUM; i++) {
      a[i].x = b[i].x;
      a[i].y = b[i].y;
   }
}


void normalizeNewList(Point array[MAX_POINT_NUM], 
      Point normalizedArray[MAX_POINT_NUM]) {
   Point min = array[0];
   Point max = array[0];

   for (size_t i = 1; i < MAX_POINT_NUM; i++) {
      if (array[i].x > max.x)
	 max.x = array[i].x;
      if (array[i].y > max.y)
	 max.y = array[i].y;

      if (array[i].x < min.x)
	 min.x = array[i].x;
      if (array[i].y < min.y)
	 min.y = array[i].y;
   }

   float rangeX = (max.x - min.x);
   float rangeY = (max.y - min.y);
   if (rangeX == 0) rangeX = 1;
   if (rangeY == 0) rangeY = 1;

   float aspectRatio = (max.x - min.x) / (max.y - min.y);
   printf("%f\n",aspectRatio);

   for (size_t i = 0; i < MAX_POINT_NUM; i++) {
      normalizedArray[i].x = (2 * (array[i].x - min.x) / rangeX - 1) 
	 * PATERN_SCALE;
      normalizedArray[i].y = (2 * (array[i].y - min.y) / rangeY - 1)
	 * PATERN_SCALE / aspectRatio;
   }
}

void motionFunc(int x, int y) {
   printf("Mouse dragged to (%d, %d)\n", x, y);
   AppState *state = (AppState *)glutGetWindowData();
   if (state->mouseDown) {
      appendPointList(&state->list, (Point){(float)x, (float)y});
   }

   glutPostRedisplay();
}


void display(void) {
   glClear(GL_COLOR_BUFFER_BIT);

   AppState *state = (AppState *)glutGetWindowData();


   if (state->list.points != NULL && state->list.size > 0)
      drawPointList(state->list);

   if (!state->isNewListEmpty)
      drawPointArray(state->newList);

   Point center = (Point){(int)state->resolution.x/2, 
      (int)state->resolution.y/2};

   if (!state->isNewListEmpty)
      drawPointArrayFromCenter(state->normalizedList, center);


   glutSwapBuffers();
}


void reshape(int w, int h) {
   AppState *state = (AppState *)glutGetWindowData();
   state->resolution = (Point){w, h};

   glViewport(0, 0, w, h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(0, w, h, 0, -1, 1);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

void appStateInit(AppState *state){
   state->list = (PointList){NULL, 0};
   state->mouseDown = 0;
   state->resolution = (Point){WIDTH, HEIGHT};
   state->check = 0;
   
   //for (size_t i = 0; i < MAX_POINT_NUM; i++) {
   //   state->newList[i] = NULL;
   //}
   state->isNewListEmpty = 1;
}

void drawPointList(PointList list){
   glColor3f(LINE_COLOR);
   glLineWidth(2.f);
   glBegin(GL_LINE_STRIP);
   for (size_t i = 0; i < list.size; i++) {
      glVertex2f(list.points[i].x, list.points[i].y);
   }
   glEnd();

   glColor3f(POINT_COLOR);
   glPointSize(5.f);
   glBegin(GL_POINTS);
   for (size_t i = 0; i < list.size; i++) {
      glVertex2f(list.points[i].x, list.points[i].y);
   }
   glEnd();
}

void drawPointArrayFromCenter(Point array[MAX_POINT_NUM], Point center){
   glColor3f(LINE_COLOR_C);
   glLineWidth(5.f);
   glBegin(GL_LINE_STRIP);
   for (size_t i = 0; i < MAX_POINT_NUM; i++) {
      glVertex2f(array[i].x + center.x, array[i].y + center.y);
   }
   glEnd();

   glColor3f(POINT_COLOR_C);
   glPointSize(6.f);
   glBegin(GL_POINTS);
   for (size_t i = 0; i < MAX_POINT_NUM; i++) {
      glVertex2f(array[i].x + center.x, array[i].y + center.y);
   }
   glEnd();
}

void drawPointArray(Point array[MAX_POINT_NUM]){
   glColor3f(LINE_COLOR_B);
   glLineWidth(5.f);
   glBegin(GL_LINE_STRIP);
   for (size_t i = 0; i < MAX_POINT_NUM; i++) {
      glVertex2f(array[i].x, array[i].y);
   }
   glEnd();

   glColor3f(POINT_COLOR_B);
   glPointSize(6.f);
   glBegin(GL_POINTS);
   for (size_t i = 0; i < MAX_POINT_NUM; i++) {
      glVertex2f(array[i].x, array[i].y);
   }
   glEnd();
}
