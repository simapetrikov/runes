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
#define DEBUG 0

typedef struct {
   PointList list; 
   int mouseDown;
   Point newList[MAX_POINT_NUM];
   Point normalizedList[MAX_POINT_NUM];
   int isNewListEmpty;
   Point resolution;
   int check;
   Point patern[MAX_POINT_NUM];
} AppState;

void display(void);
void mouseFunc(int button, int state, int x, int y);
void motionFunc(int x, int y);
void reshape(int w, int h);

void appStateInit(AppState *state);
void normalizeNewList(Point array[MAX_POINT_NUM], 
      Point normalizedArray[MAX_POINT_NUM]);

Point getMaxPoint(Point array[MAX_POINT_NUM]);
Point getMin(Point array[MAX_POINT_NUM]);
float recognize(Point a[MAX_POINT_NUM], Point b[MAX_POINT_NUM]);
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
      if(DEBUG)
	 printf("Mouse button %d pressed at (%d, %d)\n", button, x, y);
      stateVars->mouseDown = 1;
      stateVars->isNewListEmpty = 1;
      if(stateVars->list.points != NULL){
	 free(stateVars->list.points);
	 stateVars->list = (PointList){NULL, 0};
      }
   } else if (state == GLUT_UP) {
      if(DEBUG)
	 printf("Mouse button %d released at (%d, %d)\n", button, x, y);

      stateVars->mouseDown = 0;

      if(DEBUG)
	 printPointList(stateVars->list);

      resample(stateVars->list, stateVars->newList);
      stateVars->isNewListEmpty = 0;

      if(DEBUG)
	 printPointArray(stateVars->newList);

      Point center = getCenterPoint(stateVars->newList);

      if(DEBUG)
	 printPoint(center);

      normalizeNewList(stateVars->newList, stateVars->normalizedList);

      if(!stateVars->check){
	 copyArray(stateVars->patern, stateVars->normalizedList);
	 if(DEBUG)
	    printPointArray(stateVars->patern);
	 stateVars->check = 1;
      }
      else {
	 float score = recognize(stateVars->normalizedList, stateVars->patern);
	 if(DEBUG)
	    printf("score = %f\n", score);
	 else 
	    printf("%f\n", score);
	 
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

Point getMaxPoint(Point array[MAX_POINT_NUM]){
   Point max = array[0];

   for (size_t i = 1; i < MAX_POINT_NUM; i++) {
      if (array[i].x > max.x)
	 max.x = array[i].x;
      if (array[i].y > max.y)
	 max.y = array[i].y;
   }
   return max;
}

Point getMinPoint(Point array[MAX_POINT_NUM]){
   Point min = array[0];

   for (size_t i = 1; i < MAX_POINT_NUM; i++) {
      if (array[i].x < min.x)
	 min.x = array[i].x;
      if (array[i].y < min.y)
	 min.y = array[i].y;
   }
   return min;
}

Point getRange(Point min, Point max) {
   Point range;
   range.x = (max.x - min.x) != 0 ? (max.x - min.x) : 1;
   range.y = (max.y - min.y) != 0 ? (max.y - min.y) : 1;
   return range;
}

void normalizeNewList(Point array[MAX_POINT_NUM], 
      Point normalizedArray[MAX_POINT_NUM]) {

   Point min = getMinPoint(array);
   Point max = getMaxPoint(array);

   Point range = getRange(min, max);

   float aspectRatio = (max.x - min.x) / (max.y - min.y);

   for (size_t i = 0; i < MAX_POINT_NUM; i++) {
      normalizedArray[i].x = (2 * (array[i].x - min.x) / range.x - 1) 
	 * PATERN_SCALE;
      normalizedArray[i].y = (2 * (array[i].y - min.y) / range.y - 1)
	 * PATERN_SCALE / aspectRatio;
   }
}

void motionFunc(int x, int y) {
   if(DEBUG)
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

   if(DEBUG){

      if (!state->isNewListEmpty)
	 drawPointArray(state->newList);

      Point center = (Point){(int)state->resolution.x/2, 
	 (int)state->resolution.y/2};

      if (!state->isNewListEmpty)
	 drawPointArrayFromCenter(state->normalizedList, center);
   }

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

float recognize(Point gesture[MAX_POINT_NUM], Point patern[MAX_POINT_NUM]){
   float diff = 0;
   for (size_t i = 0; i < MAX_POINT_NUM; i++) {
      diff += pointsDistance(gesture[i], patern[i]);
   }

   float avr_diff = diff / MAX_POINT_NUM;

   Point min = getMinPoint(gesture);
   Point max = getMaxPoint(gesture);

   Point range = getRange(min, max);

   float square = sqrt(range.x * range.x + range.y * range.y);
   float score = 1 - (2 * avr_diff/square);

   return score;
}
